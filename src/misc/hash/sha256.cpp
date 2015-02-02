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

#include "sha256.hpp"

namespace tyrex {
namespace hash {

// Constants
constexpr std::array<uint32_t, 8> Sha256::h;
constexpr std::array<uint32_t, 64> Sha256::k;


Sha256::Sha256(const MemChunk& source) :
    mSource(source),
    mResult(Sha256::h),
    mSize(source.size()),
    mSizeBits(0),
    mCurrent(0),
    mZeros(0),
    mByteOfSize(0)
{
}

MemChunk Sha256::get()
{
    while (mByteOfSize != 8)
        this->transfrom();

    MemChunk result;
    for (unsigned int i = 0 ; i < 8 ; ++i)
    {
        result.appendChar(mResult[i] >> 24);
        result.appendChar(mResult[i] >> 16);
        result.appendChar(mResult[i] >> 8);
        result.appendChar(mResult[i]);
    }

    return result;
}


uint32_t Sha256::loadUint32()
{
    return (this->loadChar() << 24) |
           (this->loadChar() << 16) |
           (this->loadChar() <<  8) |
            this->loadChar();
}

unsigned char Sha256::loadChar()
{
    // Append source data
    if (mCurrent < mSize)
        return mSource[mCurrent++];
    // Append bits '10000000'
    else if (mSize == mCurrent)
    {
        mSizeBits = mSize;
        mSizeBits <<= 3;

        mZeros = 64 - ((mSize + 9) & 0x3F);
        mZeros &= 0x3F;

        ++mCurrent;
        return 0x80;
    }
    // Fill with zeros
    else if (mZeros)
    {
        --mZeros;
        return 0;
    }
    // Append source length
    return mSizeBits >> ((8 - ++mByteOfSize) << 3);
}


// SHA-256 algorithm
void Sha256::transfrom()
{
    uint32_t w[64];
    // Init w from data
    for (unsigned int t = 0 ; t < 16 ; ++t)
        w[t] = this->loadUint32();
    for (unsigned int t = 16 ; t < 64 ; ++t)
        w[t] = s3(w[t - 2]) + w[t - 7] + s2(w[t - 15]) + w[t - 16];

    std::array<uint32_t, 8> a = mResult;
    // Block computation
    for (unsigned int t = 0 ; t < 64 ; ++t)
    {
        uint32_t t1 = a[7] + s1(a[4]) + ch(a[4], a[5], a[6]) + k[t] + w[t];
        uint32_t t2 = s0(a[0]) + maj(a[0], a[1], a[2]);

        // Rotation
        a[7] = a[6];
        a[6] = a[5];
        a[5] = a[4];
        a[4] = a[3] + t1;
        a[3] = a[2];
        a[2] = a[1];
        a[1] = a[0];
        a[0] = t1 + t2;
    }

    // Update result
    for (unsigned int i = 0 ; i < 8 ; ++i)
        mResult[i] += a[i];
}

}
}
