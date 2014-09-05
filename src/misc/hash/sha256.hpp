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

#ifndef SHA256_HPP
#define SHA256_HPP

#include <array>
#include <cstdint>
#include "misc/memchunk.hpp"

namespace tyrex {
namespace hash {

class Sha256
{
public:
    Sha256(const MemChunk& source);
    MemChunk get();

private:
    uint32_t loadUint32();
    unsigned char loadChar();

    void transfrom();

    static inline uint32_t rotate(uint32_t uint, unsigned int pos);
    static inline uint32_t shift(uint32_t uint, unsigned int pos);
    static inline uint32_t s0(uint32_t uint);
    static inline uint32_t s1(uint32_t uint);
    static inline uint32_t s2(uint32_t uint);
    static inline uint32_t s3(uint32_t uint);
    static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z);

    const MemChunk& mSource;
    std::array<uint32_t, 8> mResult;

    unsigned int mSize;
    uint64_t mSizeBits;
    unsigned int mCurrent;
    unsigned int mZeros;
    unsigned int mByteOfSize;

    static constexpr std::array<uint32_t, 8> h {{
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    }};
    static constexpr std::array<uint32_t, 64> k {{
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    }};

};

inline uint32_t Sha256::rotate(uint32_t uint, unsigned int r)
    {return (uint >> r) | (uint << (32 - r));}
inline uint32_t Sha256::shift(uint32_t uint, unsigned int s)
    {return uint >> s;}

inline uint32_t Sha256::s0(uint32_t uint)
    {return rotate(uint, 2) ^ rotate(uint, 13) ^ rotate(uint, 22);}
inline uint32_t Sha256::s1(uint32_t uint)
    {return rotate(uint, 6) ^ rotate(uint, 11) ^ rotate(uint, 25);}
inline uint32_t Sha256::s2(uint32_t uint)
    {return rotate(uint, 7) ^ rotate(uint, 18) ^ shift(uint, 3);}
inline uint32_t Sha256::s3(uint32_t uint)
    {return rotate(uint, 17) ^ rotate(uint, 19) ^ shift(uint, 10);}
inline uint32_t Sha256::maj(uint32_t x, uint32_t y, uint32_t z)
    {return (x & y) | (z & (x | y));}
inline uint32_t Sha256::ch(uint32_t x, uint32_t y, uint32_t z)
    {return z ^ (x & (y ^ z));}

}
}

#endif // SHA256_HPP
