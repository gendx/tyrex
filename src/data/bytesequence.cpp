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

#include "bytesequence.hpp"

#include "graphic/view/hexview.hpp"

namespace tyrex {
namespace data {

ByteSequence::ByteSequence(const MemChunk& chunk) :
    mChunk(chunk)
{
}

ByteSequence::ByteSequence(const MemChunk& chunk, const Colorizer& colorizer) :
    mChunk(chunk),
    mColorizer(colorizer)
{
}


void ByteSequence::write(std::ostream& ofs) const
{
    mChunk.write(ofs);
}


void ByteSequence::appendToTree(graphic::TreeNodeModel& tree) const
{
    tree.appendLeaf("Source", this->view());
}

Shared<graphic::View> ByteSequence::view() const
{
    return makeShared<graphic::View, graphic::HexView>(mChunk, mColorizer);
}


void ByteSequence::addHighlight(unsigned int begin, unsigned int end, QColor color)
{
    mColorizer.addHighlight(begin, end - begin, color);
}

void ByteSequence::addSeparation(unsigned int pos, unsigned int size)
{
    mColorizer.addSeparation(pos, size);
}

}
}
