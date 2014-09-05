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

#ifndef TYREX_PARSE_FORWARDSTREAM_HPP
#define TYREX_PARSE_FORWARDSTREAM_HPP

#include "misc/memchunk.hpp"
#include "bitstream.hpp"

namespace tyrex {
namespace parse {

class ForwardStream : public BitStream
{
public:
    ForwardStream(const MemChunk& chunk);

    void flushByte();
    void skipBytes(unsigned int count);
    unsigned int get();
    unsigned int get(unsigned int count);

    inline unsigned int pos() const;

private:
    MemChunk mChunk;
    unsigned int mBuffer;
    unsigned int mPos;
    unsigned int mOffset;
};

inline unsigned int ForwardStream::pos() const
    {return mPos;}

}
}

#endif // TYREX_PARSE_FORWARDSTREAM_HPP
