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

#ifndef TYREX_DATA_PIXMAP_HPP
#define TYREX_DATA_PIXMAP_HPP

#include "misc/memchunk.hpp"
#include "color.hpp"

namespace tyrex {
namespace data {

class Pixmap
{
public:
    enum Type {
        grey1, grey2, grey4, grey8, grey16, greyalpha16, greyalpha32, rgb24, rgb48, argb32, argb64, rgba32, rgba64//, r5g6b5
    };

    Pixmap(Type type, unsigned int width, unsigned int height);
    Pixmap(Type type, unsigned int width, unsigned int height, MemChunk pixels);

    inline unsigned int width() const;
    inline unsigned int height() const;

    Pixmap convert(Type type) const;
    Pixmap unindex(const Pixmap& palette, bool& ok) const;

    unsigned int getARGB32(unsigned int x, unsigned int y) const;

private:
    static unsigned int bitdepth(Type type);

    ColorARGB get(unsigned int x, unsigned int y) const;
    void set(unsigned int x, unsigned int y, const ColorARGB& color);
    // return grey value (grey mode only)
    unsigned int greyAt(unsigned int x, unsigned int y) const;

    static inline unsigned int upscale8(unsigned int x);
    static inline unsigned int upscale6(unsigned int x);
    static inline unsigned int upscale5(unsigned int x);
    static inline unsigned int upscale4(unsigned int x);
    static inline unsigned int upscale2(unsigned int x);
    static inline unsigned int upscale1(unsigned int x);

    Type mType;
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mLinesize;
    MemChunk mPixels;
};

inline unsigned int Pixmap::width() const
    {return mWidth;}
inline unsigned int Pixmap::height() const
    {return mHeight;}

inline unsigned int Pixmap::upscale8(unsigned int x)
    {return (x << 8) ^ x;}
inline unsigned int Pixmap::upscale6(unsigned int x)
    {return (x << 10) ^ (x << 4) ^ (x >> 2);}
inline unsigned int Pixmap::upscale5(unsigned int x)
    {return (x << 11) ^ (x << 6) ^ (x << 1) ^ (x >> 4);}
inline unsigned int Pixmap::upscale4(unsigned int x)
    {return (x << 12) ^ (x << 8) ^ (x << 4) ^ x;}
inline unsigned int Pixmap::upscale2(unsigned int x)
    {return (x << 14) ^ (x << 12) ^ (x << 10) ^ (x << 8) ^ (x << 6) ^ (x << 4) ^ (x << 2) ^ x;}
inline unsigned int Pixmap::upscale1(unsigned int x)
    {return x ? 0xFFFF : 0;}

}
}

#endif // TYREX_DATA_PIXMAP_HPP
