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

#include "deflate.hpp"

#include "deflatestream.hpp"
#include "parse/compress/huffmantree.hpp"

namespace tyrex {
namespace parse {

Deflate::Deflate(unsigned int windowSize) :
    Lz(windowSize),
    mEnd(0)
{
}


void Deflate::onError(const MemChunk& chunk, Shared<data::Compress>& data)
{
    data = makeShared<data::Compress>(mChunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Deflate::doParse(const MemChunk& chunk, Shared<data::Compress>& data)
{
    mChunk = chunk;
    DeflateStream stream(mChunk);

    unsigned int bfinal = 0;
    while (!bfinal)
    {
        bfinal = stream.get();

        unsigned int pos = stream.pos();
        unsigned int btype = stream.get(2);

        switch (btype)
        {
        case 0: // uncompressed data
            this->parseUncompressed(stream);
            break;
        case 1: // fixed Huffman
            this->parseFixedHuffman(stream);
            break;
        case 2: // dynamic Huffman
            this->parseDynamicHuffman(stream);
            break;
        default:
            Except::reportError(pos, "deflate, block type", "invalid type");
        }
    }

    stream.flushByte();
    mEnd = stream.pos();

    mSrcColorizer.addHighlight(0, mEnd, QColor(128, 128, 255, 64));
    mSrcColorizer.addSeparation(mEnd, 2);

    data = makeShared<data::Compress>(mChunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}


void Deflate::parseUncompressed(DeflateStream& stream)
{
    stream.flushByte();
    unsigned int pos = stream.pos();
    if (mChunk.size() < pos + 4)
        Except::reportError(mChunk.size(), "deflate, uncompressed bloc, size", "unexpected end of data");

    unsigned int len = mChunk[pos] + (mChunk[pos + 1] << 8);
    unsigned int nlen = mChunk[pos + 2] + (mChunk[pos + 3] << 8);

    if ((len ^ nlen) != 0xFFFF)
        Except::reportError(pos, "deflate, uncompressed bloc, size", "invalid size check");
    pos += 4;

    if (mChunk.size() < pos + len)
        Except::reportError(mChunk.size(), "deflate, uncompressed bloc", "unexpected end of data");

    this->appendUncompressed(mChunk.subChunk(pos, len));
    pos += len;
    stream.skipBytes(len + 4);
}

void Deflate::parseFixedHuffman(DeflateStream& stream)
{
    bool ok;
    std::vector<unsigned int> lensLit(288, 0);
    for (unsigned int i = 0 ; i < 144 ; ++i)
        lensLit[i] = 8;
    for (unsigned int i = 144 ; i < 256 ; ++i)
        lensLit[i] = 9;
    for (unsigned int i = 256 ; i < 280 ; ++i)
        lensLit[i] = 7;
    for (unsigned int i = 280 ; i < 288 ; ++i)
        lensLit[i] = 8;
    HuffmanTree litTree(lensLit, ok);

    std::vector<unsigned int> lensDist(32, 0);
    for (unsigned int i = 0 ; i < 32 ; ++i)
        lensDist[i] = 5;
    HuffmanTree distTree(lensDist, ok);

    this->startHighlight();
    this->parseBlock(stream, litTree, distTree);
    this->endHighlight(QColor(128, 128, 255, 64));
}

void Deflate::parseDynamicHuffman(DeflateStream& stream)
{
    unsigned int hlit = 257 + stream.get(5);
    unsigned int hdist = 1 + stream.get(5);
    unsigned int hclen = 4 + stream.get(4);

    unsigned int order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    std::vector<unsigned int> lens(19, 0);
    for (unsigned int i = 0 ; i < hclen ; ++i)
        lens[order[i]] = stream.get(3);

    bool ok = true;
    HuffmanTree lenTree(lens, ok);
    if (!ok)
        Except::reportError(mChunk.size(), "deflate, dynamic huffman tree", "invalid lengths tree");

    std::vector<unsigned int> lensLitDist(hlit + hdist, 0);
    this->getLensCompressed(lensLitDist, hlit + hdist, stream, lenTree);

    std::vector<unsigned int> lensLit(hlit, 0);
    for (unsigned int i = 0 ; i < hlit ; ++i)
        lensLit[i] = lensLitDist[i];
    HuffmanTree litTree(lensLit, ok);
    if (!ok)
        Except::reportError(mChunk.size(), "deflate, dynamic huffman tree", "invalid literal tree");

    std::vector<unsigned int> lensDist(hdist, 0);
    for (unsigned int i = 0 ; i < hdist ; ++i)
        lensDist[i] = lensLitDist[hlit + i];
    HuffmanTree distTree(lensDist, ok);
    if (!ok)
        Except::reportError(mChunk.size(), "deflate, dynamic huffman tree", "invalid distance tree");

    this->startHighlight();
    this->parseBlock(stream, litTree, distTree);
    this->endHighlight(QColor(128, 255, 0, 64));
}


void Deflate::parseBlock(DeflateStream& stream, const HuffmanTree& litTree, const HuffmanTree& distTree)
{
    static const std::vector<std::pair<unsigned int, unsigned int> >& lengthTable = Deflate::getLengthTable();
    static const std::vector<std::pair<unsigned int, unsigned int> >& distTable = Deflate::getDistTable();

    for (bool inside = false ;; inside = true)
    {
        unsigned int literal = litTree.parse(stream);

        if (literal < 256)
            this->appendLiteral(inside, literal);
        else if (literal == 256)
            break;
        else
        {
            literal -= 257;
            unsigned int length = lengthTable[literal].first;
            length += stream.get(lengthTable[literal].second);

            unsigned int pos = stream.pos();

            unsigned int distCode = distTree.parse(stream);
            unsigned int distance = distTable[distCode].first;
            distance += stream.get(distTable[distCode].second);

            this->appendLz(inside, length, distance, pos);
        }
    }
}


const std::vector<std::pair<unsigned int, unsigned int> >& Deflate::getLengthTable()
{
    static std::vector<std::pair<unsigned int, unsigned int> > result(29, std::make_pair(0, 0));

    unsigned int val = 3;
    unsigned int extra = 0;
    for (unsigned int i = 0 ; i < 29 ; ++i)
    {
        if (i % 4 == 0 && i >= 8)
            ++extra;

        result[i].first = val;
        result[i].second = extra;
        val += 1 << extra;
    }

    result[28].first = 258;
    result[28].second = 0;

    return result;
}

const std::vector<std::pair<unsigned int, unsigned int> >& Deflate::getDistTable()
{
    static std::vector<std::pair<unsigned int, unsigned int> > result(30, std::make_pair(0, 0));

    unsigned int val = 1;
    unsigned int extra = 0;
    for (unsigned int i = 0 ; i < 30 ; ++i)
    {
        if (i % 2 == 0 && i >= 4)
            ++extra;

        result[i].first = val;
        result[i].second = extra;
        val += 1 << extra;
    }

    return result;
}

void Deflate::getLensCompressed(std::vector<unsigned int>& lens, unsigned int count, DeflateStream& stream, const HuffmanTree& lenTree)
{
    unsigned int val = 0;
    for (unsigned int i = 0 ; i < count ; )
    {
        unsigned int tmp = lenTree.parse(stream);

        if (tmp < 16)
            lens[i++] = val = tmp;
        else if (tmp == 16)
        {
            unsigned int rep = 3 + stream.get(2);

            if (count < i + rep)
                Except::reportError(stream.pos(), "deflate, dynamic huffman tree", "invalid repeat count");

            for (unsigned int k = 0 ; k < rep ; ++k)
                lens[i++] = val;
        }
        else if (tmp == 17)
        {
            val = 0;
            unsigned int rep = 3 + stream.get(3);

            if (count < i + rep)
                Except::reportError(stream.pos(), "deflate, dynamic huffman tree", "invalid repeat count");

            for (unsigned int k = 0 ; k < rep ; ++k)
                lens[i++] = 0;
        }
        else if (tmp == 18)
        {
            val = 0;
            unsigned int rep = 11 + stream.get(7);

            if (count < i + rep)
                Except::reportError(stream.pos(), "deflate, dynamic huffman tree", "invalid repeat count");

            for (unsigned int k = 0 ; k < rep ; ++k)
                lens[i++] = 0;
        }
    }
}

}
}
