/*
    Tyrex - the versatile file decoder.
    Copyright (C) 2014 - 2015  G. Endignoux

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/gpl-3.0.txt
*/

#include "xz.hpp"

#include "misc/hash/hash.hpp"
#include "misc/util.hpp"
#include "lzma2.hpp"

namespace tyrex {
namespace parse {

unsigned char Xz::mMagic[6] = {0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00};
unsigned char Xz::mMagicFooter[2] = {0x59, 0x5A};


void Xz::onError(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Xz::doParse(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 6)
        Except::reportError(size, "xz, magic", "unexpected end of data");
    if (chunk.uncompare(Xz::mMagic, 6))
        Except::reportError(0, "xz, magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 6, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(6, 2);

    // check header size
    if (size < 12)
        Except::reportError(size, "xz, header", "unexpected end of data");

    mSrcColorizer.addHighlight(6, 6, QColor(255, 0, 0, 64));
    mSrcColorizer.addSeparation(8, 1);
    mSrcColorizer.addSeparation(12, 2);

    if (chunk[6] || chunk[7] & 0xF0)
        Except::reportError(6, "xz, stream flags", "invalid stream flags");

    unsigned char checkMethod = chunk[7] & 0x0F;

    if (chunk.getUint32LE(8) != Hasher::getCRC32(chunk.subChunk(6, 2)))
        Except::reportError(8, "xz, stream flags crc", "invalid crc");

    // parse blocks
    unsigned int processed = 12;
    std::vector<std::pair<uint64_t, uint64_t> > records;
    for (;;)
    {
        if (processed >= size)
            Except::reportError(size, "xz, block", "unexpected end of data");

        if (chunk[processed] == 0)
            break;

        uint64_t unpaddedSize, uncompressedSize;
        this->parseBlock(chunk, processed, size, checkMethod, unpaddedSize, uncompressedSize);
        records.push_back(std::make_pair(unpaddedSize, uncompressedSize));
    }

    // parse index
    unsigned int beginIndex = processed;
    this->parseIndex(chunk, processed, size, records);
    unsigned int indexSize = processed - beginIndex;

    // parse footer
    if (!Util::checkRange(processed, 12, size))
        Except::reportError(size, "xz, stream footer", "unexpected end of data");

    mSrcColorizer.addHighlight(processed, 12, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(processed + 4, 1);
    mSrcColorizer.addSeparation(processed + 8, 1);
    mSrcColorizer.addSeparation(processed + 10, 1);
    mSrcColorizer.addSeparation(processed + 12, 2);

    if (chunk.getUint32LE(processed) != Hasher::getCRC32(chunk.subChunk(processed + 4, 6)))
        Except::reportError(processed, "xz, stream footer", "invalid crc");

    unsigned int backwardSize = chunk.getUint32LE(processed + 4);
    if (indexSize != (backwardSize + 1) << 2)
        Except::reportError(processed + 4, "xz, stream footer", "backward size does not match index size");

    if (chunk.subChunk(6, 2) != chunk.subChunk(processed + 8, 2))
        Except::reportError(processed + 8, "xz, stream footer", "stream flags don't match header");

    if (chunk.uncompare(Xz::mMagicFooter, 2, processed + 10))
        Except::reportError(processed + 10, "xz, stream footer", "invalid magic");

    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}


void Xz::parseIndex(const MemChunk& chunk, unsigned int& processed, unsigned int size, const std::vector<std::pair<uint64_t, uint64_t> >& records)
{
    unsigned int startIndex = processed;

    if (!Util::checkRange(processed, 1, size))
        Except::reportError(size, "xz, index", "unexpected end of data");
    if (chunk[processed])
        Except::reportError(size, "xz, index", "invalid index indicator");
    ++processed;

    uint64_t numRecords;
    if (!this->getMultibyte(chunk, processed, size, numRecords))
        Except::reportError(processed, "xz, index", "badly encoded number of records");
    if (numRecords != records.size())
        Except::reportError(processed, "xz, index", "number of records does not match number of blocks");

    mSrcColorizer.addSeparation(processed, 2);
    for (unsigned int i = 0 ; i < numRecords ; ++i)
    {
        uint64_t unpaddedSize;
        uint64_t uncompressedSize;

        if (!this->getMultibyte(chunk, processed, size, unpaddedSize))
            Except::reportError(processed, "xz, index", "badly encoded record unpadded size");
        mSrcColorizer.addSeparation(processed, 1);

        if (!this->getMultibyte(chunk, processed, size, uncompressedSize))
            Except::reportError(processed, "xz, index", "badly encoded record uncompressed size");
        mSrcColorizer.addSeparation(processed, 1);

        if (records[i].first != unpaddedSize)
            Except::reportError(processed, "xz, index", "record unpadded size does not match real size");
        if (records[i].second != uncompressedSize)
            Except::reportError(processed, "xz, index", "record uncompressed size does not match real size");
    }

    mSrcColorizer.addSeparation(processed, 2);
    this->checkPadding(chunk, processed, size, "xz, index padding");

    if (!Util::checkRange(processed, 4, size))
        Except::reportError(size, "xz, index", "unexpected end of data");

    if (chunk.getUint32LE(processed) != Hasher::getCRC32(chunk.subChunk(startIndex, processed - startIndex)))
        Except::reportError(processed, "xz, index", "invalid crc");
    processed += 4;

    mSrcColorizer.addSeparation(processed, 2);
    mSrcColorizer.addHighlight(startIndex, processed - startIndex, QColor(255, 0, 0, 64));
}

void Xz::parseBlock(const MemChunk& chunk, unsigned int& processed, unsigned int size, unsigned char checkMethod, uint64_t& unpaddedSize, uint64_t& uncompressedSize)
{
    std::vector<Filter> filters;
    uint64_t compressedSize;
    bool hasCompressedSize;
    uint64_t decompressedSize;
    bool hasDecompressedSize;

    unsigned int blockHeaderStart = processed;
    this->parseBlockHeader(chunk, processed, size, filters, compressedSize, hasCompressedSize, decompressedSize, hasDecompressedSize);


    unsigned int blockStart = processed;
    MemChunk tmpChunk;
    for (auto& filter : filters)
    {
        switch (filter.mId)
        {
        case 0x21:
            {
            unsigned int compSize = this->decodeLzma2(chunk, tmpChunk, processed, size, filter);
            if (hasCompressedSize && compressedSize != compSize)
                Except::reportError(blockStart, "xz, block", "compressed size field does not match actual size");
            break;
            }
        default:
            Except::reportError(filter.mPos, "xz, filter", "unsupported filter");
        }
    }

    if (hasDecompressedSize && decompressedSize != tmpChunk.size())
        Except::reportError(blockStart, "xz, block", "decompressed size field does not match actual size");
    uncompressedSize = tmpChunk.size();

    mSrcColorizer.addHighlight(blockStart, processed - blockStart, QColor(192, 192, 192, 64));
    mDecompChunk.append(tmpChunk);

    mSrcColorizer.addSeparation(processed, 2);
    unsigned int padding = this->checkPadding(chunk, processed, size, "xz, block padding");

    switch (checkMethod)
    {
    case 0x00: // none
        break;
    case 0x01: // CRC32
        if (!Util::checkRange(processed, 4, size))
            Except::reportError(size, "xz, block check", "unexpected end of data");
        if (chunk.getUint32LE(processed) != Hasher::getCRC32(tmpChunk))
            Except::reportError(processed, "xz, block check", "invalid check");

        mSrcColorizer.addHighlight(processed, 4, QColor(0, 255, 0, 64));
        processed += 4;
        mSrcColorizer.addSeparation(processed, 2);
        break;
    case 0x04: // CRC64
        if (!Util::checkRange(processed, 8, size))
            Except::reportError(size, "xz, block check", "unexpected end of data");
        if (chunk.getUint64LE(processed) != Hasher::getCRC64(tmpChunk))
            Except::reportError(processed, "xz, block check", "invalid check");

        mSrcColorizer.addHighlight(processed, 8, QColor(0, 255, 0, 64));
        processed += 8;
        mSrcColorizer.addSeparation(processed, 2);
        break;
    case 0x0A: // SHA-256
        if (!Util::checkRange(processed, 32, size))
            Except::reportError(size, "xz, block check", "unexpected end of data");
        if (chunk.subChunk(processed, 32) != Hasher::getSha256(tmpChunk).chunk())
            Except::reportError(processed, "xz, block check", "invalid check");

        mSrcColorizer.addHighlight(processed, 32, QColor(0, 255, 0, 64));
        processed += 32;
        mSrcColorizer.addSeparation(processed, 2);
        break;
    default:
        Except::reportError(processed, "xz, block check", "unknown check method");
    }

    unpaddedSize = processed - blockHeaderStart - padding;
}

unsigned int Xz::decodeLzma2(const MemChunk& src, MemChunk& dst, unsigned int& processed, unsigned int size, const Filter& filter)
{
    if (filter.mProperties.size() != 1)
        Except::reportError(filter.mPos, "xz, filter, lzma2", "invalid properties length");

    Lzma2 lzma2;
    std::shared_ptr<data::Compress> lzmaData;

    if (!lzma2.parse(src.subChunk(processed), lzmaData))
        Except::reportError(filter.mPos, "xz, filter, lzma2", "decoding error");

    unsigned int compressedSize = lzma2.end();
    processed += compressedSize;
    dst = lzmaData->decomp().chunk();
    return compressedSize;
}

void Xz::parseBlockHeader(const MemChunk& chunk, unsigned int& processed, unsigned int size, std::vector<Filter>& filters, uint64_t& compressedSize, bool& hasCompressedSize, uint64_t& decompressedSize, bool& hasDecompressedSize)
{
    if (processed >= size)
        Except::reportError(size, "xz, block", "unexpected end of data");

    unsigned int headerSize = chunk[processed] << 2;
    unsigned int startHeader = processed;
    unsigned int endHeader = processed + headerSize;

    if (!Util::checkRanges(processed, {headerSize, 4}, size))
        Except::reportError(size, "xz, block header", "unexpected end of data");

    mSrcColorizer.addHighlight(processed, headerSize + 4, QColor(128, 128, 255, 64));

    ++processed;
    mSrcColorizer.addSeparation(processed, 1);

    unsigned int flags = chunk[processed];
    unsigned int numFilters = (flags & 0x03) + 1;
    unsigned int reserved = flags & 0x3C;
    hasCompressedSize = flags & 0x40;
    hasDecompressedSize = flags & 0x80;

    if (reserved)
        Except::reportError(processed, "xz, block header flags", "invalid flags");
    ++processed;

    if (hasCompressedSize)
    {
        mSrcColorizer.addSeparation(processed, 1);

        unsigned int pos = processed;
        if (!this->getMultibyte(chunk, processed, endHeader, compressedSize))
            Except::reportError(pos, "xz, block header", "badly encoded compressed size");
    }

    if (hasDecompressedSize)
    {
        mSrcColorizer.addSeparation(processed, 1);

        unsigned int pos = processed;
        if (!this->getMultibyte(chunk, processed, endHeader, decompressedSize))
            Except::reportError(pos, "xz, block header", "badly encoded decompressed size");
    }

    for (unsigned int i = 0 ; i < numFilters ; ++i)
    {
        mSrcColorizer.addSeparation(processed, 2);

        Filter filter;
        filter.mPos = processed;

        unsigned int pos = processed;
        if (!this->getMultibyte(chunk, processed, endHeader, filter.mId))
            Except::reportError(pos, "xz, block header, filter", "badly encoded filter ID");
        mSrcColorizer.addSeparation(processed, 1);

        uint64_t sizeOfProperties;
        pos = processed;
        if (!this->getMultibyte(chunk, processed, endHeader, sizeOfProperties))
            Except::reportError(pos, "xz, block header, filter", "badly encoded size of properties");
        mSrcColorizer.addSeparation(processed, 1);

        if (!Util::checkRange(processed, sizeOfProperties, endHeader))
            Except::reportError(processed, "xz, block header, filter", "invalid size of properties");

        filter.mProperties = chunk.subChunk(processed, sizeOfProperties);
        filters.push_back(filter);

        processed += sizeOfProperties;
    }

    mSrcColorizer.addSeparation(processed, 2);
    for ( ; processed < endHeader ; ++processed)
        if (chunk[processed])
            Except::reportError(processed, "xz, block header padding", "invalid padding byte");

    mSrcColorizer.addSeparation(processed, 1);
    if (chunk.getUint32LE(processed) != Hasher::getCRC32(chunk.subChunk(startHeader, headerSize)))
        Except::reportError(startHeader, "xz, block header", "invalid crc");

    processed += 4;
    mSrcColorizer.addSeparation(processed, 2);
}


unsigned int Xz::checkPadding(const MemChunk& chunk, unsigned int& processed, unsigned int size, const std::string& who)
{
    unsigned int padding = ((0x03 ^ processed) + 1) & 0x03;
    if (!Util::checkRange(processed, padding, size))
        Except::reportError(size, who, "unexpected end of data");

    for (unsigned int i = 0 ; i < padding ; ++i)
    {
        if (chunk[processed])
            Except::reportError(processed, who, "invalid padding byte");
        ++processed;
    }
    if (padding)
        mSrcColorizer.addSeparation(processed, 1);
    return padding;
}

bool Xz::getMultibyte(const MemChunk& chunk, unsigned int& pos, unsigned int maxpos, uint64_t& result)
{
    result = 0;
    for (unsigned int i = 0 ; i < 9 && pos < maxpos ; ++i)
    {
        result ^= (uint64_t)(chunk[pos] & 0x7F) << (i * 7);
        if ((chunk[pos++] & 0x80) == 0)
            return true;
    }

    return false;
}

}
}
