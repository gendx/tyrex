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

#include "compress.hpp"

#include "graphic/view/hexview.hpp"

namespace tyrex {
namespace data {

Compress::Compress(const MemChunk& srcChunk, const MemChunk& decompChunk) :
    mSource(srcChunk),
    mDecomp(decompChunk)
{
}

Compress::Compress(const MemChunk& srcChunk, const MemChunk& decompChunk, const Colorizer& srcColorizer, const Colorizer& decompColorizer) :
    mSource(srcChunk, srcColorizer),
    mDecomp(decompChunk, decompColorizer)
{
}


void Compress::appendToTree(graphic::TreeNodeModel& tree) const
{
    tree.appendLeaf("Source", mSource.view());
    mFirstView = mDecomp.view();
    tree.appendLeaf("Unpacked", mFirstView);
}

}
}
