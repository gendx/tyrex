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

#ifndef TYREX_MEMCHUNK_HPP
#define TYREX_MEMCHUNK_HPP

#include <fstream>
#include <cstdint>
#include "chunk.tpl"

namespace tyrex {

// A Chunk of unsigned char.
// Added methods to compare to a std::string, to deal with std::fstreams and to extract numbers anywhere in the chunk (big- or little-endian encoding).
class MemChunk : public Chunk<unsigned char>
{
public:
    using Chunk<unsigned char>::append;
    using Chunk<unsigned char>::operator==;
    using Chunk<unsigned char>::operator!=;

    inline MemChunk();
    inline MemChunk(unsigned int size);
    inline MemChunk(const Chunk<unsigned char>& other);
    MemChunk(const std::string& str);

    bool operator==(const std::string& str) const;
    inline bool operator!=(const std::string& str) const;
    bool uncompare(const std::string& str, unsigned int start = 0) const;
    bool uncompare(const std::vector<unsigned char>& str, unsigned int start = 0) const;
    bool uncompare(const unsigned char* data, unsigned int size, unsigned int start = 0) const;

    inline void appendChar(unsigned char value);
    void append(const std::string& str);
    bool append(std::ifstream& file);
    void append(const char* data, unsigned int size);
    void write(std::ostream& file) const;

    bool getBitBE(unsigned int bitPos) const;
    void setBitBE(unsigned int bitPos, bool b);

    unsigned int getUint16BE(unsigned int start) const;
    unsigned int getUint24BE(unsigned int start) const;
    unsigned int getUint32BE(unsigned int start) const;
    uint64_t getUint64BE(unsigned int start) const;

    int getInt8(unsigned int start) const;
    int getInt16BE(unsigned int start) const;
    int getInt32BE(unsigned int start) const;
    int64_t getInt64BE(unsigned int start) const;

    float getFloatBE(unsigned int start) const;
    double getDoubleBE(unsigned int start) const;

    unsigned int getUint16LE(unsigned int start) const;
    unsigned int getUint32LE(unsigned int start) const;
    uint64_t getUint64LE(unsigned int start) const;
};

inline MemChunk::MemChunk() :
    Chunk<unsigned char>() {}
inline MemChunk::MemChunk(unsigned int size) :
    Chunk<unsigned char>(size) {}
inline MemChunk::MemChunk(const Chunk<unsigned char>& other) :
    Chunk<unsigned char>(other) {}

inline bool MemChunk::operator!=(const std::string& str) const
    {return !(*this == str);}

inline void MemChunk::appendChar(unsigned char value)
    {append(value);}

}

#endif // TYREX_MEMCHUNK_HPP
