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

#include "font.hpp"
#include "graphic/view/fontview.hpp"

namespace tyrex {
namespace data {

Font::Font(const MemChunk& srcChunk, const Colorizer& srcColorizer, const QList<std::shared_ptr<Path> >& glyphs, const Table& properties) :
    mSource(srcChunk, srcColorizer),
    mGlyphs(glyphs),
    mProperties(properties)
{
}


void Font::appendToTree(graphic::TreeNodeModel& tree) const
{
    tree.appendLeaf("Source", mSource.view());
    mProperties.appendToTree(tree);
    tree.appendLeaf("Glyphs", std::make_shared<graphic::FontView>(mGlyphs));
}

}
}
