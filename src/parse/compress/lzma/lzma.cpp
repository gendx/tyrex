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

#include "lzma.hpp"

#include "lzmadecoder.hpp"

namespace tyrex {
namespace parse {

Lzma::Lzma() :
    mEnd(0)
{
}


void Lzma::onError(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Lzma::doParse(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    unsigned int size = chunk.size();

    // check header size
    if (size < 13)
        Except::reportError(size, "lzma, header", "unexpected end of data");

    // props
    unsigned int pb = chunk[0];
    if (pb >= 225)
        Except::reportError(size, "lzma, props", "invalid props");

    unsigned int lc = pb % 9;
    pb /= 9;
    unsigned int lp = pb % 5;
    pb /= 5;

    // dict size
    unsigned int dictSizeProvided = chunk.getUint32LE(1);
    unsigned int dictSize = dictSizeProvided;
    if (dictSize < 0x1000)
        dictSize = 0x1000;

    // unpack size
    uint64_t unpackSize = chunk.getUint64LE(5);
    bool markerIsMandatory = true;
    for (unsigned int i = 5 ; i < 13 ; ++i)
    {
        if (chunk[i] != 0xFF)
        {
            markerIsMandatory = false;
            break;
        }
    }

    mSrcColorizer.addHighlight(0, 13, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(1, 1);
    mSrcColorizer.addSeparation(5, 1);
    mSrcColorizer.addSeparation(13, 2);

    LzmaDecoder decoder(lc, lp, pb);
    decoder.sync(0, !markerIsMandatory, unpackSize, markerIsMandatory);

    this->startHighlight();
    decoder.parse(chunk.subChunk(13), this);
    this->endHighlight(QColor(128, 128, 255, 64));

    mEnd = 13 + decoder.pos();
    mSrcColorizer.addHighlight(13, mEnd - 13, QColor(128, 128, 255, 64));
    mSrcColorizer.addSeparation(mEnd, 2);

    if (size != mEnd)
        Except::reportError(mEnd, "lzma", "expected end of data");

    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

}
}
