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

#include "lz.hpp"

namespace tyrex {
namespace parse {

Lz::Lz(unsigned int windowSize) :
    mWindowSize(windowSize),
    mStartHighlight(0)
{
}


void Lz::startHighlight()
{
    if (mDecompChunk.size())
        mDecompColorizer.addSeparation(mDecompChunk.size(), 2);
    mStartHighlight = mDecompChunk.size();
}

void Lz::endHighlight(const QColor& color)
{
    mDecompColorizer.addHighlight(mStartHighlight, mDecompChunk.size() - mStartHighlight, color);
}


void Lz::appendUncompressed(const MemChunk& chunk)
{
    if (mDecompChunk.size())
        mDecompColorizer.addSeparation(mDecompChunk.size(), 2);
    mDecompColorizer.addHighlight(mDecompChunk.size(), chunk.size(), QColor(255, 64, 64, 64));

    mDecompChunk.append(chunk);
}

void Lz::appendLiteral(bool addSeparation, unsigned char literal)
{
    if (addSeparation)
        mDecompColorizer.addSeparation(mDecompChunk.size(), 1);

    mDecompChunk.appendChar(literal);
}

void Lz::appendLz(bool addSeparation, unsigned int length, unsigned int distance, unsigned int pos)
{
    if (addSeparation)
        mDecompColorizer.addSeparation(mDecompChunk.size(), 1);

    if (mDecompChunk.size() < distance)
        Except::reportError(pos, "lz sequence", "distance is before start");
    if (distance > mWindowSize)
        Except::reportError(pos, "lz sequence", "distance is too big for window size");

    unsigned int k = mDecompChunk.size() - distance;
    for (unsigned int i = 0 ; i < length ; ++i)
        mDecompChunk.appendChar(mDecompChunk[k + i]);
}

}
}
