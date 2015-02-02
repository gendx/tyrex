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

#include "movetofront.hpp"

namespace tyrex {
namespace parse {

MoveToFront::MoveToFront() :
    mArray(0x1000),
    mBase(0x10)
{
    int k = 0xF00;
    for (unsigned int i = 0 ; i < 0x10 ; ++i)
    {
        mBase[i] = k;
        for (unsigned int j = 0 ; j < 0x10 ; ++j)
        {
            mArray[k] = (i << 4) ^ j;
            ++k;
        }
    }
}


unsigned char MoveToFront::front() const
{
    return mArray[mBase[0]];
}

unsigned char MoveToFront::operator[](unsigned int pos)
{
    unsigned char result;

    // Low address => do the whole MTF.
    if (pos < 0x10)
    {
        unsigned int base = mBase[0];
        result = mArray[base + pos];

        for (; pos > 0 ; --pos)
            mArray[base + pos] = mArray[base + pos - 1];
        mArray[base] = result;
    }
    // High address => optimized MTF using holes in the array.
    else
    {
        unsigned int high = pos >> 4;
        unsigned int low = pos & 0xF;
        unsigned int k = mBase[high] + low;
        result = mArray[k];

        // MTF on the block.
        for (; low > 0 ; --low, --k)
            mArray[k] = mArray[k - 1];

        // Slide down each block with one copy per block !
        ++mBase[high];
        for (; high > 0 ; --high)
        {
            --mBase[high];
            mArray[mBase[high]] = mArray[mBase[high - 1] + 0xF];
        }

        --mBase[0];
        mArray[mBase[0]] = result;

        // First block on the floor => full rebuild on the ceiling.
        if (!mBase[0])
        {
            int k = 0xFFF;
            for (int i = 15 ; i >= 0 ; --i)
            {
                for (int j = 15 ; j >= 0 ; --j)
                {
                    mArray[k] = mArray[mBase[i] + j];;
                    --k;
                }
                mBase[i] = k + 1;
            }
        }
    }

    return result;
}

}
}
