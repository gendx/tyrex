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

#ifndef TYREX_HASH_HPP
#define TYREX_HASH_HPP

#include "misc/memchunk.hpp"

#include <QString>

namespace tyrex {

template <unsigned int N>
class Hash
{
public:
    inline Hash(const MemChunk& chunk)
        {if (chunk.size() == N) mChunk = chunk;}

    inline MemChunk chunk() const
        {return mChunk;}

private:
    MemChunk mChunk;
};

class Hasher
{
public:
    static unsigned int getAdler32(const MemChunk& chunk);
    static unsigned int getCRC32(const MemChunk& chunk, unsigned int magic = 0xEDB88320);
    static uint64_t getCRC64(const MemChunk& chunk, uint64_t magic = 0xC96C5795D7870F42ull);
    static unsigned int getCRC32Reverse(const MemChunk& chunk, unsigned int magic = 0x04C11DB7);
    static Hash<32> getSha256(const MemChunk& chunk);

private:
    static const std::vector<unsigned int>& generateCRC32Table(unsigned int magic);
    static const std::vector<uint64_t>& generateCRC64Table(uint64_t magic);
    static const std::vector<unsigned int>& generateCRC32ReverseTable(unsigned int magic);
};

}

#endif // TYREX_HASH_HPP
