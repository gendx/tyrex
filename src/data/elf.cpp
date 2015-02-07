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

#include "elf.hpp"
#include "graphic/view/tableview.hpp"

namespace tyrex {
namespace data {

Elf::Elf(const MemChunk& srcChunk, const Colorizer& srcColorizer, const QList<QStringList>& segmentsHeader, const QList<QStringList>& sectionsHeader, const Table& properties) :
    mSource(srcChunk, srcColorizer),
    mSegmentsHeader("Segments", QStringList() << "Mode" << "Type", segmentsHeader),
    mSectionsHeader("Sections", QStringList() << "Mode" << "Type", sectionsHeader),
    mProperties(properties)
{
}


void Elf::appendToTree(graphic::TreeNodeModel& tree) const
{
    tree.appendLeaf("Source", mSource.view());
    mProperties.appendToTree(tree);

    std::shared_ptr<graphic::TreeNodeModel> node = std::make_shared<graphic::TreeNodeModel>("Elf");
    mSegmentsHeader.appendToTree(*node);
    mSectionsHeader.appendToTree(*node);
    tree.appendTree(node);
}

}
}
