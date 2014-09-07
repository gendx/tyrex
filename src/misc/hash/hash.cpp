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

#include "hash.hpp"

#include "misc/memchunk.hpp"
#include "misc/hash/sha256.hpp"

namespace tyrex {

unsigned int Hasher::getAdler32(const MemChunk& chunk)
{
    unsigned int size = chunk.size();

    unsigned int s1 = 1;
    unsigned int s2 = 0;

    for (unsigned int i = 0 ; i < size ; ++i)
    {
        s1 += chunk[i];
        s1 %= 65521;
        s2 += s1;
        s2 %= 65521;
    }

    return (s2 << 16) | s1;
}

unsigned int Hasher::getCRC32(const MemChunk& chunk, unsigned int magic)
{
    unsigned int result = 0xFFFFFFFF;
    static const std::vector<unsigned int>& crcTable = Hasher::generateCRC32Table(magic);

    unsigned int size = chunk.size();
    for (unsigned int i = 0 ; i < size ; ++i)
        result = (result >> 8) ^ crcTable[(result ^ chunk[i]) & 0xFF];

    return result ^ 0xFFFFFFFF;
}

uint64_t Hasher::getCRC64(const MemChunk& chunk, uint64_t magic)
{
    uint64_t result = 0xFFFFFFFFFFFFFFFFull;
    static const std::vector<uint64_t>& crcTable = Hasher::generateCRC64Table(magic);

    unsigned int size = chunk.size();
    for (unsigned int i = 0 ; i < size ; ++i)
        result = (result >> 8) ^ crcTable[(result ^ chunk[i]) & 0xFF];

    return result ^ 0xFFFFFFFFFFFFFFFFull;
}

unsigned int Hasher::getCRC32Reverse(const MemChunk& chunk, unsigned int magic)
{
    unsigned int result = 0xFFFFFFFF;
    static const std::vector<unsigned int>& crcTable = Hasher::generateCRC32ReverseTable(magic);

    unsigned int size = chunk.size();
    for (unsigned int i = 0 ; i < size ; ++i)
        result = (result << 8) ^ crcTable[((result >> 24) ^ chunk[i]) & 0xFF];

    return result ^ 0xFFFFFFFF;
}

Hash<32> Hasher::getSha256(const MemChunk& chunk)
{
    hash::Sha256 sha(chunk);
    return sha.get();
}


const std::vector<unsigned int>& Hasher::generateCRC32Table(unsigned int magic)
{
    static std::vector<unsigned int> result;
    result.reserve(256);

    for (unsigned int i = 0 ; i < 256 ; ++i)
    {
        unsigned int crc = i;
        for (unsigned int k = 0 ; k < 8 ; ++k)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ magic;
            else
                crc >>= 1;
        }
        result.push_back(crc);
    }

    return result;
}

const std::vector<uint64_t>& Hasher::generateCRC64Table(uint64_t magic)
{
    static std::vector<uint64_t> result;
    result.reserve(256);

    for (unsigned int i = 0 ; i < 256 ; ++i)
    {
        uint64_t crc = i;
        for (unsigned int k = 0 ; k < 8 ; ++k)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ magic;
            else
                crc >>= 1;
        }
        result.push_back(crc);
    }

    return result;
}

const std::vector<unsigned int>& Hasher::generateCRC32ReverseTable(unsigned int magic)
{
    static std::vector<unsigned int> result;
    result.reserve(256);

    for (unsigned int i = 0 ; i < 256 ; ++i)
    {
        unsigned int crc = i << 24;
        for (unsigned int k = 0 ; k < 8 ; ++k)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ magic;
            else
                crc <<= 1;
        }
        result.push_back(crc);
    }

    return result;
}

}
