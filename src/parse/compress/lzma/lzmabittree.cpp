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

#include "lzmabittree.hpp"

namespace tyrex {
namespace parse {

LzmaBitTree::LzmaBitTree(unsigned int numBits) :
    mNumBits(numBits),
    mProbs(1 << numBits, 0x400)
{
}


unsigned int LzmaBitTree::parse(LzmaStream& stream)
{
    unsigned int tmp = 1;
    for (unsigned int i = 0 ; i < mNumBits ; ++i)
        tmp = (tmp << 1) ^ stream.decodeBit(mProbs[tmp]);
    return tmp - (1 << mNumBits);
}

unsigned int LzmaBitTree::parseReverse(LzmaStream& stream)
{
    return stream.parseReverseBitTree(mNumBits, mProbs);
}

}
}
