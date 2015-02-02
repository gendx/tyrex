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

#ifndef TYREX_PARSE_LZ_HPP
#define TYREX_PARSE_LZ_HPP

#include "parsecompress.hpp"

namespace tyrex {
namespace parse {

class Lz : public Compress
{
    friend class LzmaDecoder;

public:
    Lz(unsigned int windowSize = 0xFFFFFFFF);

protected:
    void startHighlight();
    void endHighlight(const QColor& color);

    void appendUncompressed(const MemChunk& chunk);
    void appendLiteral(bool addSeparation, unsigned char literal);
    void appendLz(bool addSeparation, unsigned int length, unsigned int distance, unsigned int pos);

    unsigned int mWindowSize;
    unsigned int mStartHighlight;
};

}
}

#endif // TYREX_PARSE_LZ_HPP
