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

#include "lzma2.hpp"

#include "misc/util.hpp"

namespace tyrex {
namespace parse {

Lzma2::Lzma2() :
    mVirtualDictStart(0),
    mEnd(0)
{
}


void Lzma2::onError(const MemChunk& chunk, Shared<data::Compress>& data)
{
    data = makeShared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Lzma2::doParse(const MemChunk& chunk, Shared<data::Compress>& data)
{
    unsigned int size = chunk.size();
    unsigned int pos = 0;

    while (pos < size)
    {
        bool isEOS = false;
        bool isLzma = chunk[pos] & 0x80;
        bool resetDict;
        bool resetState;
        bool newProp;

        unsigned int unpackSize = 0;
        unsigned int packSize = 0;
        unsigned int lc = 0, lp = 0, pb = 0;

        // EOS
        if (chunk[pos] == 0)
        {
            isEOS = true;
            resetDict = false; resetState = false; newProp = false;
        }
        // uncompressed reset dict
        else if (chunk[pos] == 0x01)
        {
            resetDict = true; resetState = false; newProp = false;
        }
        // uncompressed no reset
        else if (chunk[pos] == 0x02)
        {
            resetDict = false; resetState = false; newProp = false;
        }
        else if (!isLzma)
            Except::reportError(pos, "lzma2, new state", "invalid state");
        else
        {
            unpackSize = chunk[pos] & 0x1F;

            switch ((chunk[pos] & 0x60) >> 5)
            {
            case 0:
                resetDict = false; resetState = false; newProp = false;
                break;
            case 1:
                resetDict = false; resetState = true; newProp = false;
                break;
            case 2:
                resetDict = false; resetState = true; newProp = true;
                break;
            case 3:
                resetDict = true; resetState = true; newProp = true;
                break;
            }
        }

        unsigned int needBytes = 1 + (isEOS ? 0 : 2) + (isLzma ? 2 : 0) + (newProp ? 1 : 0);
        if (!Util::checkRange(pos, needBytes, size))
            Except::reportError(size, "lzma2, new state", "unexpected end of data");

        mSrcColorizer.addSeparation(pos, 2);
        mSrcColorizer.addHighlight(pos, needBytes, QColor(128, 255, 0, 64));

        if (!isEOS)
        {
            mSrcColorizer.addSeparation(++pos, 1);
            unpackSize <<= 8;
            unpackSize ^= chunk[pos];
            unpackSize <<= 8;
            unpackSize ^= chunk[++pos];
        }

        if (isLzma)
        {
            mSrcColorizer.addSeparation(++pos, 1);
            packSize = chunk[pos];
            packSize <<= 8;
            packSize ^= chunk[++pos];
        }

        if (newProp)
        {
            mSrcColorizer.addSeparation(++pos, 1);
            pb = chunk[pos];

            if (pb >= 225)
                Except::reportError(pos, "lzma2, new props", "invalid props");

            lc = pb % 9;
            pb /= 9;
            lp = pb % 5;
            pb /= 5;

            if (lc + lp > 4)
                Except::reportError(pos, "lzma2, new props", "invalid props");
        }

        ++pos;
        mSrcColorizer.addSeparation(pos, 2);

        if (isEOS)
            break;

        ++unpackSize;
        ++packSize;

        needBytes = isLzma ? packSize : unpackSize;
        if (!Util::checkRange(pos, needBytes, size))
            Except::reportError(size, "lzma2, compressed data", "unexpected end of data");

        if (isLzma)
            this->parseLzma(chunk, pos, unpackSize, packSize, resetDict, resetState, newProp, lc, lp, pb);
        else
            this->parseUncompressed(chunk, pos, unpackSize, resetDict);

        mSrcColorizer.addHighlight(pos, needBytes, QColor(128, 128, 255, 64));
        pos += needBytes;
    }

    mEnd = pos;
    data = makeShared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}


void Lzma2::parseUncompressed(const MemChunk& chunk, unsigned int pos, unsigned int len, bool resetDict)
{
    if (resetDict)
        mVirtualDictStart = mDecompChunk.size();

    this->appendUncompressed(chunk.subChunk(pos, len));
}

void Lzma2::parseLzma(const MemChunk& chunk, unsigned int pos, unsigned int unpackSize, unsigned int packSize, bool resetDict, bool resetState, bool newProp, unsigned int lc, unsigned int lp, unsigned int pb)
{
    if (!(mLzmaDecoder || newProp))
        Except::reportError(pos, "lzma2, lzma chunk", "no previous properties to use");

    if (resetDict)
        mVirtualDictStart = mDecompChunk.size();

    if (!newProp)
    {
        lc = mLzmaDecoder->lc();
        lp = mLzmaDecoder->lp();
        pb = mLzmaDecoder->pb();
    }

    if (resetState)
        mLzmaDecoder = makeShared<LzmaDecoder>(lc, lp, pb);

    mLzmaDecoder->sync(mVirtualDictStart, true, unpackSize, false);
    MemChunk src = chunk.subChunk(pos, packSize);

    this->startHighlight();
    mLzmaDecoder->parse(src, this);
    this->endHighlight(QColor(128, 128, 255, 64));
}

}
}
