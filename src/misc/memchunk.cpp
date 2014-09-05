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

#include "memchunk.hpp"

#include <cstring>

namespace tyrex {

MemChunk::MemChunk(const std::string& str) :
    Chunk<unsigned char>()
{
    for (char c : str)
        mData->push_back(c);
    mSize = str.size();
}

bool MemChunk::operator==(const std::string& str) const
{
    if (mSize != str.size())
        return false;

    for (unsigned int i = 0 ; i < mSize ; ++i)
        if ((*mData)[mStart + i] != (unsigned char)str[i])
            return false;

    return true;
}


void MemChunk::append(const std::string& str)
{
    for (char c : str)
        mData->push_back(c);
    mSize += str.size();
}

bool MemChunk::append(std::ifstream& file)
{
    char c;
    while (file.good())
    {
        c = file.get();
        if (file.good())
        {
            mData->push_back(c);
            ++mSize;
        }
    }

    return true;
}

void MemChunk::append(const char* data, unsigned int size)
{
    for (unsigned int i = 0 ; i < size ; ++i)
        mData->push_back(data[i]);
    mSize += size;
}

void MemChunk::write(std::ostream& file) const
{
    file.write((const char*)(&(*mData)[mStart]), mSize);
}


bool MemChunk::uncompare(const std::string& str, unsigned int start) const
{
    if (start + str.size() > mSize)
        return true;
    return std::memcmp(&(*mData)[mStart + start], str.c_str(), str.size());
}

bool MemChunk::uncompare(const std::vector<unsigned char>& str, unsigned int start) const
{
    if (start + str.size() > mSize)
        return true;
    return std::memcmp(&(*mData)[mStart + start], &str[0], str.size());
}

bool MemChunk::uncompare(const unsigned char* data, unsigned int size, unsigned int start) const
{
    if (start + size > mSize)
        return true;
    return std::memcmp(&(*mData)[mStart + start], data, size);
}


bool MemChunk::getBitBE(unsigned int bitPos) const
{
    static constexpr unsigned char mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};

    return (*this)[bitPos >> 3] & mask[bitPos & 7];
}

void MemChunk::setBitBE(unsigned int bitPos, bool b)
{
    static constexpr unsigned char mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
    static constexpr unsigned char unmask[8] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};

    unsigned char& dst = (*this)[bitPos >> 3];
    dst = (dst & unmask[bitPos & 7]) | (b * mask[bitPos & 7]);
}


unsigned int MemChunk::getUint16BE(unsigned int start) const
{
    if (!Util::checkRange(start, 2, mSize))
        return 0;

    start += mStart;
    return ((*mData)[start    ] << 8)
         | ((*mData)[start + 1]);
}

unsigned int MemChunk::getUint24BE(unsigned int start) const
{
    if (!Util::checkRange(start, 3, mSize))
        return 0;

    start += mStart;
    return ((*mData)[start    ] << 16)
         | ((*mData)[start + 1] <<  8)
         | ((*mData)[start + 2]);
}

unsigned int MemChunk::getUint32BE(unsigned int start) const
{
    if (!Util::checkRange(start, 4, mSize))
        return 0;

    start += mStart;
    return ((*mData)[start    ] << 24)
         | ((*mData)[start + 1] << 16)
         | ((*mData)[start + 2] <<  8)
         | ((*mData)[start + 3]);
}

uint64_t MemChunk::getUint64BE(unsigned int start) const
{
    if (!Util::checkRange(start, 8, mSize))
        return 0;

    start += mStart;
    unsigned int high =
           ((*mData)[start    ] << 24)
         | ((*mData)[start + 1] << 16)
         | ((*mData)[start + 2] <<  8)
         | ((*mData)[start + 3]);
    unsigned int low =
           ((*mData)[start + 4] << 24)
         | ((*mData)[start + 5] << 16)
         | ((*mData)[start + 6] <<  8)
         | ((*mData)[start + 7]);

    uint64_t result = high;
    result <<= 32;
    result |= low;
    return result;
}


int MemChunk::getInt8(unsigned int start) const
{
    if (start >= mSize)
        return 0;
    return (char)(*mData)[mStart + start];
}

int MemChunk::getInt16BE(unsigned int start) const
{
    unsigned int tmp = getUint16BE(start);

    if (tmp & 0x8000)
    {
        int result = tmp ^ 0x8000;
        result -= 0x8000;
        return result;
    }

    return tmp;
}

int MemChunk::getInt32BE(unsigned int start) const
{
    unsigned int tmp = getUint32BE(start);

    if (tmp & 0x80000000)
    {
        int result = tmp ^ 0x80000000;
        result -= 0x80000000;
        return result;
    }

    return tmp;
}

int64_t MemChunk::getInt64BE(unsigned int start) const
{
    uint64_t tmp = getInt64BE(start);

    if (tmp & 0x8000000000000000ull)
    {
        int64_t result = tmp ^ 0x8000000000000000ull;
        result -= 0x8000000000000000ull;
        return result;
    }

    return tmp;
}


float MemChunk::getFloatBE(unsigned int start) const
{
    if (!Util::checkRange(start, 4, mSize))
        return 0;

    float result;
    unsigned char* ptr = reinterpret_cast<unsigned char*>(&result);
    for (unsigned int i = 0 ; i < 4 ; ++i)
        ptr[i] = (*mData)[start + 3 - i];

    return result;
}

double MemChunk::getDoubleBE(unsigned int start) const
{
    if (!Util::checkRange(start, 8, mSize))
        return 0;

    double result;
    unsigned char* ptr = reinterpret_cast<unsigned char*>(&result);
    for (unsigned int i = 0 ; i < 8 ; ++i)
        ptr[i] = (*mData)[start + 7 - i];

    return result;
}


unsigned int MemChunk::getUint16LE(unsigned int start) const
{
    if (!Util::checkRange(start, 2, mSize))
        return 0;

    start += mStart;
    return ((*mData)[start    ])
         | ((*mData)[start + 1] << 8);
}

unsigned int MemChunk::getUint32LE(unsigned int start) const
{
    if (!Util::checkRange(start, 4, mSize))
        return 0;

    start += mStart;
    return ((*mData)[start    ])
         | ((*mData)[start + 1] <<  8)
         | ((*mData)[start + 2] << 16)
         | ((*mData)[start + 3] << 24);
}

uint64_t MemChunk::getUint64LE(unsigned int start) const
{
    if (!Util::checkRange(start, 8, mSize))
        return 0;

    start += mStart;
    unsigned int low =
            ((*mData)[start    ])
          | ((*mData)[start + 1] <<  8)
          | ((*mData)[start + 2] << 16)
          | ((*mData)[start + 3] << 24);
    unsigned int high =
            ((*mData)[start + 4])
          | ((*mData)[start + 5] <<  8)
          | ((*mData)[start + 6] << 16)
          | ((*mData)[start + 7] << 24);

    uint64_t result = high;
    result <<= 32;
    result |= low;
    return result;
}

}
