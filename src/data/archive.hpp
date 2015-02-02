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

#ifndef TYREX_DATA_ARCHIVE_HPP
#define TYREX_DATA_ARCHIVE_HPP

#include "bytesequence.hpp"
#include "file.hpp"
#include "misc/tree.tpl"

namespace tyrex {
namespace data {

class Archive : public Data
{
public:
    Archive(const MemChunk& srcChunk, const Colorizer& srcColorizer, const FileInfoFilter& fileInfoFilter, const std::vector<File>& files);

    void appendToTree(graphic::TreeNodeModel& tree) const;
    Shared<graphic::View> view() const;

private:
    void makeTreeFiles();

    void appendToTree(graphic::TreeNodeModel& tree, const TreeLeaf<File>& file) const;
    void appendToTree(graphic::TreeNodeModel& tree, const Tree<File>& folder) const;

    ByteSequence mSource;
    FileInfoFilter mFileInfoFilter;
    std::vector<File> mFiles;
    Tree<File> mTreeFiles;
};

}
}

#endif // TYREX_DATA_ARCHIVE_HPP
