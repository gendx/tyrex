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

#ifndef TYREX_DATA_BYTESEQUENCE_HPP
#define TYREX_DATA_BYTESEQUENCE_HPP

#include "data.hpp"
#include "colorizer.hpp"
#include "misc/memchunk.hpp"

namespace tyrex {
namespace data {

class ByteSequence : public Data
{
public:
    ByteSequence(const MemChunk& chunk);
    ByteSequence(const MemChunk& chunk, const Colorizer& colorizer);

    inline const MemChunk& chunk() const;
    inline const Colorizer& colorizer() const;

    void write(std::ostream& ofs) const;

    void appendToTree(graphic::TreeNodeModel& tree) const;
    std::shared_ptr<graphic::View> view() const;

    void addHighlight(unsigned int begin, unsigned int end, QColor color);
    void addSeparation(unsigned int pos, unsigned int size);

private:
    MemChunk mChunk;
    Colorizer mColorizer;
};

inline const MemChunk& ByteSequence::chunk() const
    {return mChunk;}
inline const Colorizer& ByteSequence::colorizer() const
    {return mColorizer;}

}
}

#endif // TYREX_DATA_BYTESEQUENCE_HPP
