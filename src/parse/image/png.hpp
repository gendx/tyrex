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

#ifndef TYREX_PARSE_PNG_HPP
#define TYREX_PARSE_PNG_HPP

#include "parse/parser.tpl"
#include "data/image.hpp"

namespace tyrex {
namespace parse {

class Png : public DataParser<data::Image>
{
public:
    Png();

private:
    void doParse(const MemChunk& chunk, std::shared_ptr<data::Image>& data);
    void onError(const MemChunk& chunk, std::shared_ptr<data::Image>& data);

    void parseIHDR();
    void parseIDAT(const MemChunk& src);
    MemChunk decodeImage(const MemChunk& src, unsigned int& posSrc, unsigned int bitsPerPixel, unsigned int bytesPerPixel, unsigned int width, unsigned int height);

    MemChunk deinterlace(unsigned int bitsPerPixel, unsigned int bytesPerPixel, const MemChunk& img1, const MemChunk& img2, const MemChunk& img3, const MemChunk& img4, const MemChunk& img5, const MemChunk& img6, const MemChunk& img7);
    MemChunk deinterlaceBits(unsigned int bitsPerPixel, const MemChunk& img1, const MemChunk& img2, const MemChunk& img3, const MemChunk& img4, const MemChunk& img5, const MemChunk& img6, const MemChunk& img7);
    MemChunk deinterlaceBytes(unsigned int bytesPerPixel, const MemChunk& img1, const MemChunk& img2, const MemChunk& img3, const MemChunk& img4, const MemChunk& img5, const MemChunk& img6, const MemChunk& img7);

    struct Element
    {
        unsigned int mStart;
        MemChunk mHeader;
        MemChunk mData;
    };

    static unsigned char mMagic[8];

    data::Colorizer mSrcColorizer;
    data::Table mProperties;

    std::vector<Element> mChunks;
    std::shared_ptr<data::Pixmap> mPixmap;
    std::shared_ptr<data::Pixmap> mPalette;
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned char mDepth;
    unsigned char mColor;
    unsigned char mCompression;
    unsigned char mFilter;
    unsigned char mInterlace;
};

}
}

#endif // PNG_HPP
