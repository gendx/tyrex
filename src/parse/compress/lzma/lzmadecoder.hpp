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

#ifndef TYREX_PARSE_LZMADECODER_HPP
#define TYREX_PARSE_LZMADECODER_HPP

#include "lzmabittree.hpp"
#include "lzmalendecoder.hpp"
#include "parse/compress/lz.hpp"

namespace tyrex {
namespace parse {

class LzmaDecoder
{
public:
    LzmaDecoder(unsigned int lc, unsigned int lp, unsigned int pb);

    void sync(unsigned int virtualDictStart, bool unpackSizeDefined, unsigned int unpackSize, bool markerIsMandatory);
    void parse(const MemChunk& chunk, Lz* lzParser);

    inline unsigned int lc() const;
    inline unsigned int lp() const;
    inline unsigned int pb() const;

    inline unsigned int pos() const;

private:
    unsigned char decodeLiteral(LzmaStream& stream);
    unsigned int decodeDistance(LzmaStream& stream, unsigned int length);

    unsigned int mPos;
    Lz* mLzParser;

    unsigned int mVirtualDictStart;
    bool mUnpackSizeDefined;
    unsigned int mUnpackSize;
    bool mMarkerIsMandatory;

    unsigned int mLc;
    unsigned int mLp;
    unsigned int mPb;

    unsigned int mRep0;
    unsigned int mRep1;
    unsigned int mRep2;
    unsigned int mRep3;
    unsigned int mState;

    std::vector<std::vector<unsigned short> > mLiteralProbs;
    LzmaStaticBitTree<6> mPosSlotDecoder[4];
    LzmaStaticBitTree<4> mAlignDecoder;
    std::vector<unsigned short> mPosDecoders;

    LzmaLenDecoder mLenDecoder;
    LzmaLenDecoder mRepLenDecoder;

    std::vector<unsigned short> mIsMatch;
    std::vector<unsigned short> mIsRep;
    std::vector<unsigned short> mIsRepG0;
    std::vector<unsigned short> mIsRepG1;
    std::vector<unsigned short> mIsRepG2;
    std::vector<unsigned short> mIsRep0Long;
};

inline unsigned int LzmaDecoder::lc() const
    {return mLc;}
inline unsigned int LzmaDecoder::lp() const
    {return mLp;}
inline unsigned int LzmaDecoder::pb() const
    {return mPb;}

inline unsigned int LzmaDecoder::pos() const
    {return mPos;}

}
}

#endif // TYREX_PARSE_LZMADECODER_HPP
