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

#include "gzip.hpp"

#include "deflate.hpp"
#include "misc/hash/hash.hpp"
#include "misc/util.hpp"

namespace tyrex {
namespace parse {

unsigned char Gzip::mMagic[2] = {0x1F, 0x8B};

Gzip::Gzip()
{
}


void Gzip::onError(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Gzip::doParse(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 2)
        Except::reportError(size, "gzip, magic", "unexpected end of data");
    if (chunk.uncompare(Gzip::mMagic, 2))
        Except::reportError(0, "gzip, magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 2, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(2, 2);

    // check header size
    if (size < 10)
        Except::reportError(size, "gzip, header", "unexpected end of data");

    mSrcColorizer.addSeparation(3, 1);
    mSrcColorizer.addSeparation(4, 1);
    mSrcColorizer.addSeparation(8, 1);
    mSrcColorizer.addSeparation(9, 1);

    unsigned int method = chunk[2];
    unsigned int flags = chunk[3];
    unsigned int mtime = chunk[4];
    unsigned int xfl = chunk[8];
    unsigned int os = chunk[9];
    unsigned int processed = 10;

    if (method != 8)
        Except::reportError(0, "gzip, header, method", "invalid method");
    if (flags >> 5)
        Except::reportError(0, "gzip, header, flags", "invalid flags");

    // extra field
    if (flags & 0x04)
    {
        if (!Util::checkRange(processed, 2, size))
            Except::reportError(processed, "gzip, header, extra field", "unexpected end of data");

        unsigned int len = chunk.getUint16LE(processed);
        mSrcColorizer.addSeparation(processed, 1);
        processed += 2;

        if (!Util::checkRange(processed, len, size))
            Except::reportError(processed, "gzip, header, extra field", "unexpected end of data");

        mExtraField = chunk.subChunk(processed, len);
        mSrcColorizer.addSeparation(processed, 1);
        processed += len;
    }

    // file name
    if (flags & 0x08)
    {
        unsigned int start = processed;
        mSrcColorizer.addSeparation(processed, 1);

        for (;;++processed)
        {
            if (processed >= size)
                Except::reportError(processed, "gzip, header, file name", "unexpected end of data");

            if (chunk[processed] == 0)
                break;
        }

        ++processed;
        mFileName = chunk.subChunk(start, processed - start);
    }

    // file comment
    if (flags & 0x10)
    {
        unsigned int start = processed;
        mSrcColorizer.addSeparation(start, 1);

        for (;;++processed)
        {
            if (processed >= size)
                Except::reportError(processed, "gzip, header, file comment", "unexpected end of data");

            if (chunk[processed] == 0)
                break;
        }

        ++processed;
        mFileComment = chunk.subChunk(start, processed - start);
    }

    // end of header
    mSrcColorizer.addHighlight(2, processed - 2, QColor(255, 0, 0, 64));
    mSrcColorizer.addSeparation(processed, 2);

    // check header crc
    if (flags & 0x02)
    {
        unsigned int crc = chunk.getUint16BE(processed);

        if (crc != (Hasher::getCRC32(chunk.subChunk(0, processed)) & 0xFFFF))
            Except::reportError(processed, "gzip, header", "invalid crc");
    }

    // deflate
    Deflate deflate(1 << 15);
    std::shared_ptr<data::Compress> deflateData;

    if (!deflate.parse(chunk.subChunk(processed, size - processed), deflateData))
        Except::reportError(size, "gzip, deflate", "error parsing deflate stream");

    mDecompChunk = deflateData->decomp().chunk();
    mDecompColorizer = deflateData->decomp().colorizer();

    mSrcColorizer.addSeparation(processed, 2);
    mSrcColorizer.addHighlight(processed, deflate.end(), QColor(128, 128, 255, 64));

    processed += deflate.end();
    if (!Util::checkRange(processed, 8, size))
        Except::reportError(size, "gzip, footer", "unexpected end of data");

    mSrcColorizer.addSeparation(processed, 2);

    unsigned int crc = chunk.getUint32LE(processed);
    if (crc != Hasher::getCRC32(mDecompChunk))
        Except::reportError(processed, "gzip, footer", "invalid crc");

    mSrcColorizer.addHighlight(processed, 8, QColor(0, 255, 0, 64));
    processed += 4;
    mSrcColorizer.addSeparation(processed, 1);

    unsigned int len = chunk.getUint32LE(processed);
    if (len != mDecompChunk.size())
        Except::reportError(processed, "gzip, footer", "invalid decompressed size");

    processed += 4;
    mSrcColorizer.addSeparation(processed, 2);

    if (size != processed)
        Except::reportError(processed, "gzip", "expected end of data");

    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

}
}
