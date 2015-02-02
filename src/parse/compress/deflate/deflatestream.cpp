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

#include "deflatestream.hpp"

#include "parse/parseexception.hpp"

namespace tyrex {
namespace parse {

DeflateStream::DeflateStream(const MemChunk& chunk) :
    mChunk(chunk),
    mPos(0),
    mOffset(0)
{
}


void DeflateStream::flushByte()
{
    if (mOffset)
    {
        ++mPos;
        mOffset = 0;
    }
}

void DeflateStream::skipBytes(unsigned int count)
{
    this->flushByte();
    mPos += count;
}

unsigned int DeflateStream::get()
{
    if (mOffset == 0)
    {
        if (mPos >= mChunk.size())
            Except::reportError(mChunk.size(), "deflate stream", "unexpected end of data");
        mBuffer = mChunk[mPos];
    }

    unsigned int result = (mBuffer >> mOffset) & 0x01;

    if (++mOffset == 8)
    {
        ++mPos;
        mOffset = 0;
    }

    return result;
}

unsigned int DeflateStream::get(unsigned int count)
{
    unsigned int result = 0;
    for (unsigned int i = 0 ; i < count ; ++i)
        result ^= (this->get() << i);
    return result;
}

}
}
