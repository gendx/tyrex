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

#include "lzmalendecoder.hpp"

namespace tyrex {
namespace parse {

LzmaLenDecoder::LzmaLenDecoder() :
    mChoice(0x400),
    mChoice2(0x400)
{
}


unsigned int LzmaLenDecoder::decode(LzmaStream& stream, unsigned int posState)
{
    if (!stream.decodeBit(mChoice))
        return mLowCoder[posState].parse(stream);
    if (!stream.decodeBit(mChoice2))
        return 8 + mMidCoder[posState].parse(stream);
    return 16 + mHighCoder.parse(stream);
}

}
}
