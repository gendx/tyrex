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

#include "lzmastream.hpp"

#include "parse/parseexception.hpp"

namespace tyrex {
namespace parse {

LzmaStream::LzmaStream(const MemChunk& chunk) :
    mChunk(chunk),
    mPos(5),
    mRange(0xFFFFFFFF),
    mCode(chunk.getUint32BE(1)),
    mCorrupted(false)
{
    if (mChunk.size() < 5 || mChunk[0])
        mCorrupted = true;
}


void LzmaStream::normalize()
{
    if (mRange < 0x1000000)
    {
        mRange <<= 8;
        if (mPos >= mChunk.size())
            Except::reportError(mChunk.size(), "lzma stream", "unexpected end of data");
        mCode = (mCode << 8) ^ mChunk[mPos];
        ++mPos;
    }
}

unsigned int LzmaStream::get()
{
    mRange >>= 1;
    mCode -= mRange;
    unsigned int t = -(mCode >> 31);
    mCode += mRange & t;

    if (mCode == mRange)
        mCorrupted = true;

    this->normalize();
    return t + 1;
}

unsigned int LzmaStream::get(unsigned int count)
{
    unsigned int result = 0;
    for (unsigned int i = 0 ; i < count ; ++i)
        result = (result << 1) ^ this->get();
    return result;
}

unsigned int LzmaStream::decodeBit(unsigned short& prob)
{
    unsigned int bound = (mRange >> 11) * prob;

    if (mCode < bound)
    {
        prob += (0x800 - prob) >> 5;
        mRange = bound;

        this->normalize();
        return 0;
    }
    else
    {
        prob -= prob >> 5;
        mCode -= bound;
        mRange -= bound;

        this->normalize();
        return 1;
    }
}

unsigned int LzmaStream::parseReverseBitTree(unsigned int numBits, std::vector<unsigned short>& probs, unsigned int offset)
{
    unsigned int result = 0;
    unsigned int tmp = 1;

    for (unsigned int i = 0 ; i < numBits ; ++i)
    {
        unsigned int bit = this->decodeBit(probs[offset + tmp]);
        tmp = (tmp << 1) ^ bit;
        result ^= (bit << i);
    }

    return result;
}

}
}
