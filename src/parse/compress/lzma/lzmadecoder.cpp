/*
    Tyrex - the versatile file decoder.
    Copyright (C) 2014  G. Endignoux

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

#include "lzmadecoder.hpp"

namespace tyrex {
namespace parse {

LzmaDecoder::LzmaDecoder(unsigned int lc, unsigned int lp, unsigned int pb) :
    mPos(0),
    mLzParser(NULL),
    mVirtualDictStart(0),
    mUnpackSizeDefined(false),
    mUnpackSize(0),
    mMarkerIsMandatory(false),
    mLc(lc),
    mLp(lp),
    mPb(pb),
    mRep0(0),
    mRep1(0),
    mRep2(0),
    mRep3(0),
    mState(0),
    mLiteralProbs(1 << (lc + lp), std::vector<unsigned short>(0x300, 0x400)),
    mPosDecoders(115, 0x400),
    mIsMatch(192, 0x400),
    mIsRep(12, 0x400),
    mIsRepG0(12, 0x400),
    mIsRepG1(12, 0x400),
    mIsRepG2(12, 0x400),
    mIsRep0Long(192, 0x400)
{
}


void LzmaDecoder::sync(unsigned int virtualDictStart, bool unpackSizeDefined, unsigned int unpackSize, bool markerIsMandatory)
{
    mVirtualDictStart = virtualDictStart;
    mUnpackSizeDefined = unpackSizeDefined;
    mUnpackSize = unpackSize;
    mMarkerIsMandatory = markerIsMandatory;
}


void LzmaDecoder::parse(const MemChunk& chunk, Lz* lzParser)
{
    LzmaStream stream(chunk);
    mLzParser = lzParser;

    for (bool inside = false ;; inside = true)
    {
        if (mUnpackSizeDefined && mUnpackSize == 0 && !mMarkerIsMandatory)
        {
            if (stream.isFinishedOk())
                break;
        }

        unsigned int posState = (mLzParser->mDecompChunk.size() - mVirtualDictStart) & ((1 << mPb) - 1);

        if (!stream.decodeBit(mIsMatch[(mState << 4) + posState]))
        {
            if (mUnpackSizeDefined && mUnpackSize == 0)
                Except::reportError(stream.pos(), "lzma decoder", "marker expected at end");

            mLzParser->appendLiteral(inside, this->decodeLiteral(stream));
            // update state (literal)
            mState = (mState < 4 ? 0 : (mState < 10 ? (mState - 3) : (mState - 6)));
            --mUnpackSize;
            continue;
        }

        unsigned int len;

        if (stream.decodeBit(mIsRep[mState]))
        {
            if (mUnpackSizeDefined && mUnpackSize == 0)
                Except::reportError(stream.pos(), "lzma decoder", "unpack size does not match");
            if (mLzParser->mDecompChunk.size() == mVirtualDictStart)
                Except::reportError(stream.pos(), "lzma decoder", "unexpected start of dict size");

            if (!stream.decodeBit(mIsRepG0[mState]))
            {
                if (!stream.decodeBit(mIsRep0Long[(mState << 4) + posState]))
                {
                    // update state (short rep)
                    mState = (mState < 7 ? 9 : 11);
                    mLzParser->appendLz(inside, 1, mRep0 + 1, stream.pos());
                    --mUnpackSize;
                    continue;
                }
            }
            else
            {
                unsigned int dist;
                if (!stream.decodeBit(mIsRepG1[mState]))
                    dist = mRep1;
                else
                {
                    if (!stream.decodeBit(mIsRepG2[mState]))
                        dist = mRep2;
                    else
                    {
                        dist = mRep3;
                        mRep3 = mRep2;
                    }
                    mRep2 = mRep1;
                }
                mRep1 = mRep0;
                mRep0 = dist;
            }

            len = mRepLenDecoder.decode(stream, posState);
            // update state (rep)
            mState = (mState < 7 ? 8 : 11);
        }
        else
        {
            mRep3 = mRep2;
            mRep2 = mRep1;
            mRep1 = mRep0;
            len = mLenDecoder.decode(stream, posState);
            // update state (match)
            mState = (mState < 7 ? 7 : 10);

            mRep0 = this->decodeDistance(stream, len);
            if (mRep0 == 0xFFFFFFFF)
            {
                if (stream.isFinishedOk())
                    break;
                Except::reportError(stream.pos(), "lzma decoder", "input stream is corrupted");
            }

            if (mUnpackSizeDefined && mUnpackSize == 0)
                Except::reportError(stream.pos(), "lzma decoder", "unpack size does not match");
            if (mRep0 >= mLzParser->mDecompChunk.size() - mVirtualDictStart)
                Except::reportError(stream.pos(), "lzma decoder", "lz sequence is too far");
        }

        len += 2;

        if (mUnpackSizeDefined && mUnpackSize < len)
            Except::reportError(stream.pos(), "lzma decoder", "lz sequence is too long");

        mLzParser->appendLz(inside, len, mRep0 + 1, stream.pos());
        mUnpackSize -= len;
    }

    mPos = stream.pos();
}


unsigned char LzmaDecoder::decodeLiteral(LzmaStream& stream)
{
    unsigned int prevByte = 0;
    if (mLzParser->mDecompChunk.size() > 0)
        prevByte = mLzParser->mDecompChunk[mLzParser->mDecompChunk.size() - 1];

    unsigned int result = 1;
    unsigned int litState = (((mLzParser->mDecompChunk.size() - mVirtualDictStart) & ((1 << mLp) - 1)) << mLc) + (prevByte >> (8 - mLc));
    std::vector<unsigned short>& probs = mLiteralProbs[litState];

    if (mState >= 7)
    {
        unsigned int matchByte = mLzParser->mDecompChunk[mLzParser->mDecompChunk.size() - mRep0 - 1];
        do
        {
            unsigned matchBit = (matchByte >> 7) & 1;
            matchByte <<= 1;
            unsigned bit = stream.decodeBit(probs[((1 + matchBit) << 8) + result]);
            result = (result << 1) ^ bit;
            if (matchBit != bit)
                break;
        }
        while (result < 0x100);
    }

    while (result < 0x100)
        result = (result << 1) ^ stream.decodeBit(probs[result]);

    return result - 0x100;
}


unsigned int LzmaDecoder::decodeDistance(LzmaStream& stream, unsigned int length)
{
    unsigned int lenState = length;
    if (lenState > 3)
        lenState = 3;

    unsigned int posSlot = mPosSlotDecoder[lenState].parse(stream);
    if (posSlot < 4)
        return posSlot;

    unsigned int numDirectBits = ((posSlot >> 1) - 1);
    unsigned int result = ((2 ^ (posSlot & 1)) << numDirectBits);

    if (posSlot < 14)
        result += stream.parseReverseBitTree(numDirectBits, mPosDecoders, result - posSlot);
    else
    {
        result += (stream.get(numDirectBits - 4) << 4);
        result += mAlignDecoder.parseReverse(stream);
    }

    return result;
}

}
}
