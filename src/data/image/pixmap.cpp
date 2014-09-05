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

#include "pixmap.hpp"

namespace tyrex {
namespace data {

Pixmap::Pixmap(Type type, unsigned int width, unsigned int height) :
    mType(type),
    mWidth(width),
    mHeight(height),
    mLinesize((width * Pixmap::bitdepth(mType) + 7) >> 3),
    mPixels(mLinesize * mHeight)
{
}

Pixmap::Pixmap(Type type, unsigned int width, unsigned int height, MemChunk pixels) :
    mType(type),
    mWidth(width),
    mHeight(height),
        mLinesize((width * Pixmap::bitdepth(mType) + 7) >> 3),
    mPixels(pixels)
{
    if (mLinesize * mHeight != mPixels.size())
        mPixels = MemChunk(mLinesize * mHeight);
}


Pixmap Pixmap::convert(Type type) const
{
    Pixmap result(type, mWidth, mHeight);

    for (unsigned int x = 0 ; x < mWidth ; ++x)
        for (unsigned int y = 0 ; y < mHeight ; ++y)
            result.set(x, y, this->get(x, y));
    return result;
}

Pixmap Pixmap::unindex(const Pixmap& palette, bool& ok) const
{
    Pixmap result(palette.mType, mWidth, mHeight);
    ok = true;

    for (unsigned int x = 0 ; x < mWidth ; ++x)
    {
        for (unsigned int y = 0 ; y < mHeight ; ++y)
        {
            unsigned int index = this->greyAt(x, y);
            if (index < palette.mWidth * palette.mHeight)
                result.set(x, y, palette.get(index, 0));
            else
                ok = false;
        }
    }

    return result;
}

unsigned int Pixmap::bitdepth(Type type)
{
    switch (type)
    {
    case grey1:
        return 1;
    case grey2:
        return 2;
    case grey4:
        return 4;
    case grey8:
        return 8;
    case grey16:
        return 16;
    case greyalpha16:
        return 16;
    case greyalpha32:
        return 32;
    case rgb24:
        return 24;
    case rgb48:
        return 48;
    case argb32:
        return 32;
    case argb64:
        return 64;
    case rgba32:
        return 32;
    case rgba64:
        return 64;/*
    case r5g6b5:
        return 16;*/
    default:
        return 0;
    }
}

// return grey value (grey mode only)
unsigned int Pixmap::greyAt(unsigned int x, unsigned int y) const
{
    unsigned int yoff = y * mLinesize;

    switch (mType)
    {
    case grey16:
        {
        unsigned int pos = yoff + (x << 1);
        return ((unsigned int)mPixels[pos] << 8) ^ mPixels[pos + 1];
        }
    case grey8:
        return mPixels[yoff + x];
    case grey4:
        {
        unsigned int pos = yoff + (x >> 1);
        if (x % 2)
            return mPixels[pos] & 0xF;
        else
            return mPixels[pos] >> 4;
        }
    case grey2:
        {
        unsigned int pos = yoff + (x >> 2);
        switch (x % 4)
        {
        case 0:
            return mPixels[pos] >> 6;
        case 1:
            return (mPixels[pos] >> 4) & 0x3;
        case 2:
            return (mPixels[pos] >> 2) & 0x3;
        case 3:
            return mPixels[pos] & 0x3;
        }
        }
    case grey1:
        {
        unsigned int pos = yoff + (x >> 3);
        switch (x % 8)
        {
        case 0:
            return (mPixels[pos] & 0x80) >> 7;
        case 1:
            return (mPixels[pos] & 0x40) >> 6;
        case 2:
            return (mPixels[pos] & 0x20) >> 5;
        case 3:
            return (mPixels[pos] & 0x10) >> 4;
        case 4:
            return (mPixels[pos] & 0x8) >> 3;
        case 5:
            return (mPixels[pos] & 0x4) >> 2;
        case 6:
            return (mPixels[pos] & 0x2) >> 1;
        case 7:
            return mPixels[pos] & 0x1;
        }
        }
    default:
        return 0;
    }
}

unsigned int Pixmap::getARGB32(unsigned int x, unsigned int y) const
{
    unsigned int yoff = y * mLinesize;

    switch (mType)
    {
    case greyalpha16:
        {
        unsigned int pos = yoff + (x << 1);
        return (mPixels[pos + 1] << 24) ^ (mPixels[pos] << 16) ^ (mPixels[pos] << 8) ^ mPixels[pos];
        }
    case greyalpha32:
        {
        unsigned int pos = yoff + (x << 2);
        return (mPixels[pos + 2] << 24) ^ (mPixels[pos] << 16) ^ (mPixels[pos] << 8) ^ mPixels[pos];
        }
    case rgb24:
        {
        unsigned int pos = yoff + 3 * x;
        return 0xFF000000 ^ (mPixels[pos] << 16) ^ (mPixels[pos + 1] << 8) ^ mPixels[pos + 2];
        }
    case rgb48:
        {
        unsigned int pos = yoff + 6 * x;
        return 0xFF000000 ^ (mPixels[pos] << 16) ^ (mPixels[pos + 2] << 8) ^ mPixels[pos + 4];
        }
    case argb32:
        {
        unsigned int pos = yoff + (x << 2);
        return (mPixels[pos] << 24) ^ (mPixels[pos + 1] << 16) ^ (mPixels[pos + 2] << 8) ^ mPixels[pos + 3];
        }
    case argb64:
        {
        unsigned int pos = yoff + (x << 3);
        return (mPixels[pos] << 24) ^ (mPixels[pos + 2] << 16) ^ (mPixels[pos + 4] << 8) ^ mPixels[pos + 6];
        }
    case rgba32:
        {
        unsigned int pos = yoff + (x << 2);
        return (mPixels[pos + 3] << 24) ^ (mPixels[pos] << 16) ^ (mPixels[pos + 1] << 8) ^ mPixels[pos + 2];
        }
    case rgba64:
        {
        unsigned int pos = yoff + (x << 3);
        return (mPixels[pos + 6] << 24) ^ (mPixels[pos] << 16) ^ (mPixels[pos + 2] << 8) ^ mPixels[pos + 4];
        }
    case grey16:
        {
        unsigned int pos = yoff + (x << 1);
        return 0xFF000000 ^ (mPixels[pos] << 16) ^ (mPixels[pos] << 8) ^ mPixels[pos];
        }
    case grey8:
        {
        unsigned int pos = yoff + x;
        return 0xFF000000 ^ (mPixels[pos] << 16) ^ (mPixels[pos] << 8) ^ mPixels[pos];
        }
    case grey4:
        {
        unsigned int pos = yoff + (x >> 1);
        if (x % 2)
            return 0xFF000000 ^ ((mPixels[pos] & 0xF) * 0x111111);
        else
            return 0xFF000000 ^ ((mPixels[pos] >> 4) * 0x111111);
        }
    case grey2:
        {
        unsigned int pos = yoff + (x >> 2);
        switch (x % 4)
        {
        case 0:
            return 0xFF000000 ^ ((mPixels[pos] >> 6) * 0x555555);
        case 1:
            return 0xFF000000 ^ (((mPixels[pos] >> 4) & 0x3) * 0x555555);
        case 2:
            return 0xFF000000 ^ (((mPixels[pos] >> 2) & 0x3) * 0x555555);
        case 3:
            return 0xFF000000 ^ ((mPixels[pos] & 0x3) * 0x555555);
        }
        }
    case grey1:
        {
        unsigned int pos = yoff + (x >> 3);
        switch (x % 8)
        {
        case 0:
            return (mPixels[pos] & 0x80) ? 0xFFFFFFFF : 0xFF000000;
        case 1:
            return (mPixels[pos] & 0x40) ? 0xFFFFFFFF : 0xFF000000;
        case 2:
            return (mPixels[pos] & 0x20) ? 0xFFFFFFFF : 0xFF000000;
        case 3:
            return (mPixels[pos] & 0x10) ? 0xFFFFFFFF : 0xFF000000;
        case 4:
            return (mPixels[pos] & 0x8) ? 0xFFFFFFFF : 0xFF000000;
        case 5:
            return (mPixels[pos] & 0x4) ? 0xFFFFFFFF : 0xFF000000;
        case 6:
            return (mPixels[pos] & 0x2) ? 0xFFFFFFFF : 0xFF000000;
        case 7:
            return (mPixels[pos] & 0x1) ? 0xFFFFFFFF : 0xFF000000;
        }
        }
    default:
        ;
    }

    return 0;
}

ColorARGB Pixmap::get(unsigned int x, unsigned int y) const
{
    ColorARGB result;
    unsigned int yoff = y * mLinesize;

    switch (mType)
    {
    case greyalpha16:
        {
        unsigned int pos = yoff + (x << 1);
        result.red = result.green = result.blue = upscale8(mPixels[pos]);
        result.alpha = upscale8(mPixels[pos + 1]);
        }
        break;
    case greyalpha32:
        {
        unsigned int pos = yoff + (x << 2);
        result.red = result.green = result.blue = ((unsigned int)mPixels[pos] << 8) ^ mPixels[pos + 1];
        result.alpha = ((unsigned int)mPixels[pos + 2] << 8) ^ mPixels[pos + 3];
        }
        break;
    case rgb24:
        {
        unsigned int pos = yoff + 3 * x;
        result.alpha = 0xFFFF;
        result.red = upscale8(mPixels[pos]);
        result.green = upscale8(mPixels[pos + 1]);
        result.blue = upscale8(mPixels[pos + 2]);
        }
        break;
    case rgb48:
        {
        unsigned int pos = yoff + 6 * x;
        result.alpha = 0xFFFF;
        result.red = ((unsigned int)mPixels[pos] << 8) ^ mPixels[pos + 1];
        result.green = ((unsigned int)mPixels[pos + 2] << 8) ^ mPixels[pos + 3];
        result.blue = ((unsigned int)mPixels[pos + 4] << 8) ^ mPixels[pos + 5];
        }
        break;
    case argb32:
        {
        unsigned int pos = yoff + (x << 2);
        result.alpha = upscale8(mPixels[pos]);
        result.red = upscale8(mPixels[pos + 1]);
        result.green = upscale8(mPixels[pos + 2]);
        result.blue = upscale8(mPixels[pos + 3]);
        }
        break;
    case argb64:
        {
        unsigned int pos = yoff + (x << 3);
        result.alpha = ((unsigned int)mPixels[pos] << 8) ^ mPixels[pos + 1];
        result.red = ((unsigned int)mPixels[pos + 2] << 8) ^ mPixels[pos + 3];
        result.green = ((unsigned int)mPixels[pos + 4] << 8) ^ mPixels[pos + 5];
        result.blue = ((unsigned int)mPixels[pos + 6] << 8) ^ mPixels[pos + 7];
        }
        break;
    case rgba32:
        {
        unsigned int pos = yoff + (x << 2);
        result.red = upscale8(mPixels[pos]);
        result.green = upscale8(mPixels[pos + 1]);
        result.blue = upscale8(mPixels[pos + 2]);
        result.alpha = upscale8(mPixels[pos + 3]);
        }
        break;
    case rgba64:
        {
        unsigned int pos = yoff + (x << 3);
        result.red = ((unsigned int)mPixels[pos] << 8) ^ mPixels[pos + 1];
        result.green = ((unsigned int)mPixels[pos + 2] << 8) ^ mPixels[pos + 3];
        result.blue = ((unsigned int)mPixels[pos + 4] << 8) ^ mPixels[pos + 5];
        result.alpha = ((unsigned int)mPixels[pos + 6] << 8) ^ mPixels[pos + 7];
        }
        break;/*
    case r5g6b5:
        {
        unsigned int pos = yoff + (x << 1);
        result.alpha = 0xFFFF;
        result.red = upscale5(mPixels[pos] >> 3);
        result.green = ((mPixels[pos] & 0x7) << 3) ^ (mPixels[pos + 1] >> 5);
        result.blue = mPixels[pos + 1] & 0x1F;
        }
        break;*/
    case grey16:
        {
        unsigned int pos = yoff + (x << 1);
        result.alpha = 0xFFFF;
        result.red = result.green = result.blue = ((unsigned int)mPixels[pos] << 8) ^ mPixels[pos + 1];
        }
        break;
    case grey8:
        result.alpha = 0xFFFF;
        result.red = result.green = result.blue = upscale8(mPixels[yoff + x]);
        break;
    case grey4:
        {
        unsigned int pos = yoff + (x >> 1);
        result.alpha = 0xFFFF;
        if (x % 2)
            result.red = result.green = result.blue = upscale4(mPixels[pos] & 0xF);
        else
            result.red = result.green = result.blue = upscale4(mPixels[pos] >> 4);
        }
        break;
    case grey2:
        {
        unsigned int pos = yoff + (x >> 2);
        result.alpha = 0xFFFF;
        switch (x % 4)
        {
        case 0:
            result.red = result.green = result.blue = upscale2(mPixels[pos] >> 6);
            break;
        case 1:
            result.red = result.green = result.blue = upscale2((mPixels[pos] >> 4) & 0x3);
            break;
        case 2:
            result.red = result.green = result.blue = upscale2((mPixels[pos] >> 2) & 0x3);
            break;
        case 3:
            result.red = result.green = result.blue = upscale2(mPixels[pos] & 0x3);
        }
        }
        break;
    case grey1:
        {
        unsigned int pos = yoff + (x >> 3);
        result.alpha = 0xFFFF;
        switch (x % 8)
        {
        case 0:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x80);
            break;
        case 1:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x40);
            break;
        case 2:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x20);
            break;
        case 3:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x10);
            break;
        case 4:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x8);
            break;
        case 5:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x4);
            break;
        case 6:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x2);
            break;
        case 7:
            result.red = result.green = result.blue = upscale1(mPixels[pos] & 0x1);
        }
        }
        break;
    default:
        ;
    }

    return result;
}

void Pixmap::set(unsigned int x, unsigned int y, const ColorARGB& color)
{
    unsigned int yoff = y * mLinesize;

    switch (mType)
    {
    case greyalpha16:
        {
        unsigned int pos = yoff + (x << 1);
        mPixels[pos] = color.grey() >> 8;
        mPixels[pos + 1] = color.alpha >> 8;
        }
        break;
    case greyalpha32:
        {
        unsigned int pos = yoff + (x << 2);
        {
            unsigned int val = color.grey();
            mPixels[pos] = val >> 8;
            mPixels[pos + 1] = val;
        }
        mPixels[pos + 2] = color.alpha >> 8;
        mPixels[pos + 3] = color.alpha;
        }
        break;
    case rgb24:
        {
        unsigned int pos = yoff + 3 * x;
        mPixels[pos] = color.red >> 8;
        mPixels[pos + 1] = color.green >> 8;
        mPixels[pos + 2] = color.blue >> 8;
        }
        break;
    case rgb48:
        {
        unsigned int pos = yoff + 6 * x;
        mPixels[pos] = color.red >> 8;
        mPixels[pos + 1] = color.red;
        mPixels[pos + 2] = color.green >> 8;
        mPixels[pos + 3] = color.green;
        mPixels[pos + 4] = color.blue >> 8;
        mPixels[pos + 5] = color.blue;
        }
        break;
    case argb32:
        {
        unsigned int pos = yoff + (x << 2);
        mPixels[pos] = color.alpha >> 8;
        mPixels[pos + 1] = color.red >> 8;
        mPixels[pos + 2] = color.green >> 8;
        mPixels[pos + 3] = color.blue >> 8;
        }
        break;
    case argb64:
        {
        unsigned int pos = yoff + (x << 3);
        mPixels[pos] = color.alpha >> 8;
        mPixels[pos + 1] = color.alpha;
        mPixels[pos + 2] = color.red >> 8;
        mPixels[pos + 3] = color.red;
        mPixels[pos + 4] = color.green >> 8;
        mPixels[pos + 5] = color.green;
        mPixels[pos + 6] = color.blue >> 8;
        mPixels[pos + 7] = color.blue;
        }
        break;
    case rgba32:
        {
        unsigned int pos = yoff + (x << 2);
        mPixels[pos] = color.red >> 8;
        mPixels[pos + 1] = color.green >> 8;
        mPixels[pos + 2] = color.blue >> 8;
        mPixels[pos + 3] = color.alpha >> 8;
        }
        break;
    case rgba64:
        {
        unsigned int pos = yoff + (x << 3);
        mPixels[pos] = color.red >> 8;
        mPixels[pos + 1] = color.red;
        mPixels[pos + 2] = color.green >> 8;
        mPixels[pos + 3] = color.green;
        mPixels[pos + 4] = color.blue >> 8;
        mPixels[pos + 5] = color.blue;
        mPixels[pos + 6] = color.alpha >> 8;
        mPixels[pos + 7] = color.alpha;
        }
        break;/*
    case r5g6b5:
        {
        unsigned int pos = yoff + (x << 1);
        mPixels[pos] = ((color.red >> 11) << 3) ^ (color.green >> 13);
        mPixels[pos + 1] = ((color.green >> 10) << 5) ^ (color.blue >> 11);
        }
        break;*/
    case grey16:
        {
            unsigned int pos = yoff + (x << 1);
            unsigned int val = color.grey();
            mPixels[pos] = val >> 8;
            mPixels[pos + 1] = val;
        }
        break;
    case grey8:
        mPixels[yoff + x] = color.grey() >> 8;
        break;
    case grey4:
        {
        unsigned int pos = yoff + (x >> 1);
        if (x % 2)
            mPixels[pos] = (mPixels[pos] & 0xF0) ^ (color.grey() >> 12);
        else
            mPixels[pos] = (mPixels[pos] & 0xF) ^ ((color.grey() >> 12) << 4);
        }
        break;
    case grey2:
        {
        unsigned int pos = yoff + (x >> 2);
        switch (x % 4)
        {
        case 0:
            mPixels[pos] = (mPixels[pos] & 0x3F) ^ ((color.grey() >> 14) << 6);
            break;
        case 1:
            mPixels[pos] = (mPixels[pos] & 0xCF) ^ ((color.grey() >> 14) << 4);
            break;
        case 2:
            mPixels[pos] = (mPixels[pos] & 0xF3) ^ ((color.grey() >> 14) << 2);
            break;
        case 3:
            mPixels[pos] = (mPixels[pos] & 0xFC) ^ (color.grey() >> 14);
        }
        }
        break;
    case grey1:
        {
        unsigned int pos = yoff + (x >> 3);
        switch (x % 8)
        {
        case 0:
            mPixels[pos] = (mPixels[pos] & 0x7F) ^ ((color.grey() >> 15) << 7);
            break;
        case 1:
            mPixels[pos] = (mPixels[pos] & 0xBF) ^ ((color.grey() >> 15) << 6);
            break;
        case 2:
            mPixels[pos] = (mPixels[pos] & 0xDF) ^ ((color.grey() >> 15) << 5);
            break;
        case 3:
            mPixels[pos] = (mPixels[pos] & 0xEF) ^ ((color.grey() >> 15) << 4);
            break;
        case 4:
            mPixels[pos] = (mPixels[pos] & 0xF7) ^ ((color.grey() >> 15) << 3);
            break;
        case 5:
            mPixels[pos] = (mPixels[pos] & 0xFB) ^ ((color.grey() >> 15) << 2);
            break;
        case 6:
            mPixels[pos] = (mPixels[pos] & 0xFD) ^ ((color.grey() >> 15) << 1);
            break;
        case 7:
            mPixels[pos] = (mPixels[pos] & 0xFE) ^ (color.grey() >> 15);
        }
        }
        break;
    default:
        ;
    }
}

}
}
