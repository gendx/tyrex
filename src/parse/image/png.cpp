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

#include "png.hpp"

#include "parse/compress/deflate/zlib.hpp"
#include "misc/hash/hash.hpp"

namespace tyrex {
namespace parse {

unsigned char Png::mMagic[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

Png::Png() :
    mProperties("Properties", QStringList() << "Property" << "Value")
{
}


void Png::onError(const MemChunk& chunk, std::shared_ptr<data::Image>& data)
{
    data = std::make_shared<data::Image>(chunk, mSrcColorizer, std::shared_ptr<data::Pixmap>(), mProperties);
}

void Png::doParse(const MemChunk& chunk, std::shared_ptr<data::Image>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 8)
        Except::reportError(size, "png magic", "unexpected end of data");
    // check magic
    if (chunk.uncompare(Png::mMagic, 8))
        Except::reportError(0, "png magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 8, QColor(255, 128, 0, 64));
    unsigned int processed = 8;

    while (processed < size)
    {
        if (!Util::checkRange(processed, 4, size))
            Except::reportError(size, "png chunk size", "unexpected end of data");

        mSrcColorizer.addSeparation(processed, 2);

        Element element;
        element.mStart = processed;
        unsigned int length = chunk.getUint32BE(processed);
        processed += 4;

        if (!Util::checkRanges(processed, {length, 8}, size))
            Except::reportError(size, "png chunk", "unexpected end of data");

        MemChunk crcChunk = chunk.subChunk(processed, length + 4);

        element.mHeader = chunk.subChunk(processed, 4);
        mSrcColorizer.addHighlight(processed, 4, QColor(255, 0, 0, 64));
        processed += 4;

        element.mData = chunk.subChunk(processed, length);
        mSrcColorizer.addHighlight(processed, length, QColor(128, 128, 255, 64));
        processed += length;

        unsigned int crc = chunk.getUint32BE(processed);
        mSrcColorizer.addHighlight(processed, 4, QColor(0, 255, 0, 64));

        if (crc != Hasher::getCRC32(crcChunk))
            Except::reportError(processed, "png chunk", "invalid crc");
        processed += 4;

        mChunks.push_back(element);
    }

    // TODO: check chunks titles/order
    // parse data
    MemChunk idat;
    mWidth = 0;
    mHeight = 0;
    mDepth = 0;
    mColor = 0;
    mCompression = 0;
    mFilter = 0;
    mInterlace = 0;

    for (unsigned int i = 0 ; i < mChunks.size() ; ++i)
    {
        // IHDR
        if (i == 0)
            this->parseIHDR();

        // PLTE
        if (mChunks[i].mHeader == "PLTE")
        {
            if (mDepth == 0)
                Except::reportError(mChunks[i].mStart, "png chunk", "PLTE chunk must be after IHDR");

            unsigned int size = mChunks[i].mData.size();
            if (size % 3 || size == 0 || size > 3 * (1u << mDepth))
                Except::reportError(mChunks[i].mStart, "png PLTE chunk", "invalid PLTE chunk size");

            mPalette = std::make_shared<data::Pixmap>(data::Pixmap::rgb24, size / 3, 1, mChunks[i].mData);
        }

        // IDAT
        if (mChunks[i].mHeader == "IDAT")
            idat.append(mChunks[i].mData);

        // IEND
        if (i + 1 == mChunks.size() && mChunks[i].mHeader != "IEND")
            Except::reportError(mChunks[i].mStart, "png chunk", "last chunk must be IEND");
    }

    Zlib zlib;
    std::shared_ptr<data::Compress> zlibData;

    if (!zlib.parse(idat, zlibData))
        Except::reportError(0, "png idat", "error decompressing idat");
    MemChunk decompChunk = zlibData->decomp().chunk();

    this->parseIDAT(decompChunk);

    data = std::make_shared<data::Image>(chunk, mSrcColorizer, mPixmap, mProperties);
}


void Png::parseIHDR()
{
    if (mChunks[0].mHeader != "IHDR")
        Except::reportError(mChunks[0].mStart, "png chunk", "first chunk must be IHDR");

    MemChunk data = mChunks[0].mData;
    if (data.size() != 13)
        Except::reportError(mChunks[0].mStart, "png IHDR chunk", "IHDR chunk size must be 13");

    mWidth = data.getUint32BE(0);
    mHeight = data.getUint32BE(4);
    mDepth = data[8];
    mColor = data[9];
    mCompression = data[10];
    mFilter = data[11];
    mInterlace = data[12];

    if (mWidth == 0 || mHeight == 0)
        Except::reportError(mChunks[0].mStart, "png size", "size must not be empty");

    switch (mColor)
    {
    case 0:
        if (mDepth != 1 && mDepth != 2 && mDepth != 4 && mDepth != 8 && mDepth != 16)
            Except::reportError(mChunks[0].mStart, "png bit depth", "invalid bit depth");
        break;
    case 3:
        if (mDepth != 1 && mDepth != 2 && mDepth != 4 && mDepth != 8)
            Except::reportError(mChunks[0].mStart, "png bit depth", "invalid bit depth");
        break;
    case 2:
    case 4:
    case 6:
        if (mDepth != 8 && mDepth != 16)
            Except::reportError(mChunks[0].mStart, "png bit depth", "invalid bit depth");
        break;
    default:
        Except::reportError(mChunks[0].mStart, "png colour type", "invalid colour type");
    }

    if (mCompression != 0)
        Except::reportError(mChunks[0].mStart, "png compression method", "invalid compression method");

    if (mFilter != 0)
        Except::reportError(mChunks[0].mStart, "png filter method", "invalid filter method");

    if (mInterlace != 0 && mInterlace != 1)
        Except::reportError(mChunks[0].mStart, "png interlace method", "invalid interlace method");

    // TODO: update properties
    mProperties.push("Width", QString::number(mWidth));
    mProperties.push("Height", QString::number(mHeight));
    mProperties.push("Bit depth", QString::number(mDepth));

    if (mInterlace == 0)
        mProperties.push("Interlace method", "none");
    else if (mInterlace == 1)
        mProperties.push("Interlace method", "Adam7");

    switch (mColor)
    {
    case 0:
        mProperties.push("Colour type", "greyscale");
        break;
    case 2:
        mProperties.push("Colour type", "truecolour");
        break;
    case 3:
        mProperties.push("Colour type", "indexed");
        break;
    case 4:
        mProperties.push("Colour type", "greyscale with alpha");
        break;
    case 6:
        mProperties.push("Colour type", "truecolour with alpha");
    }
}

void Png::parseIDAT(const MemChunk& src)
{
    data::Pixmap::Type type;
    data::Pixmap::Type indexedtype;
    unsigned int bitsPerPixel;

    switch (mColor)
    {
    case 0: // greyscale
        bitsPerPixel = mDepth;
        switch (mDepth)
        {
        case 1:
            type = data::Pixmap::grey1;
            break;
        case 2:
            type = data::Pixmap::grey2;
            break;
        case 4:
            type = data::Pixmap::grey4;
            break;
        case 8:
            type = data::Pixmap::grey8;
            break;
        case 16:
            type = data::Pixmap::grey16;
        }
        break;
    case 2: // truecolour
        bitsPerPixel = mDepth * 3;
        if (mDepth == 8)
            type = data::Pixmap::rgb24;
        else if (mDepth == 16)
            type = data::Pixmap::rgb48;
        break;
    case 3: // indexed
        bitsPerPixel = mDepth;
        type = data::Pixmap::rgb24;
        switch (mDepth)
        {
        case 1:
            indexedtype = data::Pixmap::grey1;
            break;
        case 2:
            indexedtype = data::Pixmap::grey2;
            break;
        case 4:
            indexedtype = data::Pixmap::grey4;
            break;
        case 8:
            indexedtype = data::Pixmap::grey8;
        }
        break;
    case 4: // greyscale with alpha
        bitsPerPixel = mDepth << 1;
        if (mDepth == 8)
            type = data::Pixmap::greyalpha16;
        else if (mDepth == 16)
            type = data::Pixmap::greyalpha32;
        break;
    case 6: // truecolour with alpha
        bitsPerPixel = mDepth << 2;
        if (mDepth == 8)
            type = data::Pixmap::rgba32;
        else if (mDepth == 16)
            type = data::Pixmap::rgba64;
    }

    // for filtering prediction
    unsigned int bytesPerPixel = bitsPerPixel >> 3;
    if (!bytesPerPixel)
        ++bytesPerPixel;
    mProperties.push("Bytes per pixel", QString::number(bytesPerPixel));

    MemChunk unfiltered;
    unsigned int srcPos = 0;

    if (mInterlace == 0)
        unfiltered = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel, mWidth, mHeight);
    else if (mInterlace == 1)
    {
        MemChunk img1 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                          (mWidth + 7) >> 3, (mHeight + 7) >> 3);
        MemChunk img2 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                          (mWidth + 3) >> 3, (mHeight + 7) >> 3);
        MemChunk img3 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                          (mWidth + 3) >> 2, (mHeight + 3) >> 3);
        MemChunk img4 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                          (mWidth + 1) >> 2, (mHeight + 3) >> 2);
        MemChunk img5 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                          (mWidth + 1) >> 1, (mHeight + 1) >> 2);
        MemChunk img6 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                           mWidth      >> 1, (mHeight + 1) >> 1);
        MemChunk img7 = this->decodeImage(src, srcPos, bitsPerPixel, bytesPerPixel,
                                           mWidth,            mHeight      >> 1);
        unfiltered = this->deinterlace(bitsPerPixel, bytesPerPixel, img1, img2, img3, img4, img5, img6, img7);
    }

    switch (mColor)
    {
    case 0: // greyscale
    case 2: // truecolour
    case 4: // greyscale with alpha
    case 6: // truecolour with alpha
        mPixmap = std::make_shared<data::Pixmap>(type, mWidth, mHeight, unfiltered);
        break;
    case 3: // indexed
        bool ok;
        mPixmap = std::make_shared<data::Pixmap>(data::Pixmap(indexedtype, mWidth, mHeight, unfiltered).unindex(*mPalette, ok));
        if (!ok)
            Except::reportError(mChunks[0].mStart, "png IDAT chunk", "invalid index in PLTE");
    }
}


MemChunk Png::decodeImage(const MemChunk& src, unsigned int& posSrc, unsigned int bitsPerPixel, unsigned int bytesPerPixel, unsigned int width, unsigned int height)
{
    unsigned int xpredict = bytesPerPixel;
    unsigned int ypredict = width * bytesPerPixel;

    unsigned int linesize = (bitsPerPixel * width + 7) >> 3;
    MemChunk dst(linesize * height);

    for (unsigned int y = 0 ; y < height ; ++y)
    {
        unsigned int filter = src[posSrc];

        ++posSrc;
        unsigned int posDst = y * linesize;

        switch (filter)
        {
        case 0:
            for (unsigned int byte = 0 ; byte < linesize ; ++byte)
                dst[posDst++] = src[posSrc++];
            break;
        case 1:
            for (unsigned int x = 0 ; x < width ; ++x)
            {
                for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                {
                    dst[posDst] = src[posSrc++] + (x > 0 ? dst[posDst - xpredict] : 0);
                    ++posDst;
                }
            }
            break;
        case 2:
            for (unsigned int x = 0 ; x < width ; ++x)
            {
                for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                {
                    dst[posDst] = src[posSrc++] + (y > 0 ? dst[posDst - ypredict] : 0);
                    ++posDst;
                }
            }
            break;
        case 3:
            for (unsigned int x = 0 ; x < width ; ++x)
            {
                for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                {
                    unsigned int a = x > 0 ? dst[posDst - xpredict] : 0;
                    unsigned int b = y > 0 ? dst[posDst - ypredict] : 0;
                    dst[posDst++] = src[posSrc++] + (a + b) / 2;
                }
            }
            break;
        case 4:
            for (unsigned int x = 0 ; x < width ; ++x)
            {
                for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                {
                    int a = x > 0 ? dst[posDst - xpredict] : 0;
                    int b = y > 0 ? dst[posDst - ypredict] : 0;
                    int c = x > 0 && y > 0 ? dst[posDst - ypredict - xpredict] : 0;

                    int p = a + b - c;
                    int pa = abs(p - a);
                    int pb = abs(p - b);
                    int pc = abs(p - c);

                    int r = c;
                    if (pa <= pb && pa <= pc)
                        r = a;
                    else if (pb <= pc)
                        r = b;

                    dst[posDst++] = src[posSrc++] + r;
                }
            }
            break;
        default:
            Except::reportError(mChunks[0].mStart, "png IDAT chunk", "invalid filter method");
        }
    }

    return dst;
}


MemChunk Png::deinterlace(unsigned int bitsPerPixel, unsigned int bytesPerPixel, const MemChunk& img1, const MemChunk& img2, const MemChunk& img3, const MemChunk& img4, const MemChunk& img5, const MemChunk& img6, const MemChunk& img7)
{
    if (bitsPerPixel & 7)
        return this->deinterlaceBits(bitsPerPixel, img1, img2, img3, img4, img5, img6, img7);
    else
        return this->deinterlaceBytes(bytesPerPixel, img1, img2, img3, img4, img5, img6, img7);
}

MemChunk Png::deinterlaceBytes(unsigned int bytesPerPixel, const MemChunk& img1, const MemChunk& img2, const MemChunk& img3, const MemChunk& img4, const MemChunk& img5, const MemChunk& img6, const MemChunk& img7)
{
    unsigned int linesize = bytesPerPixel * mWidth;
    MemChunk result(linesize * mHeight);

    // pass 1
    unsigned int pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 8)
        for (unsigned int x = 0 ; x < mWidth ; x += 8)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img1[pos++];

    // pass 2
    pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 8)
        for (unsigned int x = 4 ; x < mWidth ; x += 8)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img2[pos++];

    // pass 3
    pos = 0;
    for (unsigned int y = 4 ; y < mHeight ; y += 8)
        for (unsigned int x = 0 ; x < mWidth ; x += 4)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img3[pos++];

    // pass 4
    pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 4)
        for (unsigned int x = 2 ; x < mWidth ; x += 4)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img4[pos++];

    // pass 5
    pos = 0;
    for (unsigned int y = 2 ; y < mHeight ; y += 4)
        for (unsigned int x = 0 ; x < mWidth ; x += 2)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img5[pos++];

    // pass 6
    pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 2)
        for (unsigned int x = 1 ; x < mWidth ; x += 2)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img6[pos++];

    // pass 7
    pos = 0;
    for (unsigned int y = 1 ; y < mHeight ; y += 2)
        for (unsigned int x = 0 ; x < mWidth ; ++x)
            for (unsigned int byte = 0 ; byte < bytesPerPixel ; ++byte)
                result[(y * mWidth + x) * bytesPerPixel + byte] = img7[pos++];

    return result;
}

MemChunk Png::deinterlaceBits(unsigned int bitsPerPixel, const MemChunk& img1, const MemChunk& img2, const MemChunk& img3, const MemChunk& img4, const MemChunk& img5, const MemChunk& img6, const MemChunk& img7)
{
    unsigned int linesize = (bitsPerPixel * mWidth + 7) >> 3;
    MemChunk result(linesize * mHeight);
    linesize <<= 3;

    // pass 1
    unsigned int pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 8)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 0 ; x < mWidth ; x += 8)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img1.getBitBE(pos++));
    }

    // pass 2
    pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 8)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 4 ; x < mWidth ; x += 8)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img2.getBitBE(pos++));
    }

    // pass 3
    pos = 0;
    for (unsigned int y = 4 ; y < mHeight ; y += 8)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 0 ; x < mWidth ; x += 4)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img3.getBitBE(pos++));
    }

    // pass 4
    pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 4)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 2 ; x < mWidth ; x += 4)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img4.getBitBE(pos++));
    }

    // pass 5
    pos = 0;
    for (unsigned int y = 2 ; y < mHeight ; y += 4)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 0 ; x < mWidth ; x += 2)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img5.getBitBE(pos++));
    }

    // pass 6
    pos = 0;
    for (unsigned int y = 0 ; y < mHeight ; y += 2)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 1 ; x < mWidth ; x += 2)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img6.getBitBE(pos++));
    }

    // pass 7
    pos = 0;
    for (unsigned int y = 1 ; y < mHeight ; y += 2)
    {
        pos = ((pos + 7) >> 3) << 3;
        for (unsigned int x = 0 ; x < mWidth ; ++x)
            for (unsigned int bit = 0 ; bit < bitsPerPixel ; ++bit)
                result.setBitBE(y * linesize + x * bitsPerPixel + bit, img7.getBitBE(pos++));
    }

    return result;
}

}
}
