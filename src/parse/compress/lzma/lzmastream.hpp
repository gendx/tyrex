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

#ifndef TYREX_PARSE_LZMASTREAM_HPP
#define TYREX_PARSE_LZMASTREAM_HPP

#include "misc/memchunk.hpp"

namespace tyrex {
namespace parse {

class LzmaStream
{
public:
    LzmaStream(const MemChunk& chunk);

    unsigned int get();
    unsigned int get(unsigned int count);
    unsigned int decodeBit(unsigned short& prob);
    unsigned int parseReverseBitTree(unsigned int numBits, std::vector<unsigned short>& probs, unsigned int offset = 0);

    inline unsigned int pos() const;
    inline bool isFinishedOk() const;

private:
    void normalize();

    MemChunk mChunk;
    unsigned int mPos;
    unsigned int mRange;
    unsigned int mCode;
    bool mCorrupted;
};

inline unsigned int LzmaStream::pos() const
    {return mPos;}
inline bool LzmaStream::isFinishedOk() const
    {return mCode == 0 && mPos == mChunk.size();}

}
}

#endif // TYREX_PARSE_LZMASTREAM_HPP
