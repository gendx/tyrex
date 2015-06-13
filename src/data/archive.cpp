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

#include "archive.hpp"

#include "graphic/view/archiveview.hpp"
#include "graphic/view/hexview.hpp"

namespace tyrex {
namespace data {

Archive::Archive(const MemChunk& srcChunk, const Colorizer& srcColorizer, const FileInfoFilter& fileInfoFilter, const std::vector<File>& files) :
    mSource(srcChunk, srcColorizer),
    mFileInfoFilter(fileInfoFilter),
    mFiles(files),
    mTreeFiles("Files")
{
    this->makeTreeFiles();
}


void Archive::doAppendToTree(graphic::TreeNodeModel& tree) const
{
    tree.appendLeaf("Source", mSource.view());
    mFirstView = this->view();
    tree.appendLeaf("Archive", mFirstView);

    this->doAppendToTree(tree, mTreeFiles);
}

std::shared_ptr<graphic::View> Archive::view() const
{
    return std::make_shared<graphic::ArchiveView>(mFileInfoFilter, mTreeFiles);
}


void Archive::doAppendToTree(graphic::TreeNodeModel& tree, const TreeLeaf<File>& file) const
{
    std::shared_ptr<graphic::View> view = std::make_shared<graphic::HexView>(file.mContent.mChunk);
    tree.appendLeaf(file.mTitle, view);
}

void Archive::doAppendToTree(graphic::TreeNodeModel& tree, const Tree<File>& folder) const
{
    std::shared_ptr<graphic::TreeNodeModel> node = std::make_shared<graphic::TreeNodeModel>(folder.mTitle);

    for (auto& subtree : folder.mSubtrees)
        this->doAppendToTree(*node, *subtree);
    for (auto& leaf : folder.mLeaves)
        this->doAppendToTree(*node, leaf);

    tree.appendTree(node);
}


void Archive::makeTreeFiles()
{
    QMap<QString, Tree<File>*> folders;

    for (auto& file : mFiles)
    {
        QString name = file.mInfo[FileInfo::fileName];
        Tree<File>* folder = &mTreeFiles;

        for (int pos = -1 ; ; )
        {
            int nextpos = name.indexOf("/", pos + 1);

            if (nextpos >= 0)
            {
                QString key = name.left(nextpos + 1);

                if (folders.contains(key))
                    folder = folders[key];
                else
                {
                    std::shared_ptr<Tree<File> > tmp = std::make_shared<Tree<File> >(name.mid(pos + 1, nextpos - pos - 1));
                    folder->appendTree(tmp);
                    folder = tmp.get();
                    folders[key] = folder;
                }

                pos = nextpos;
            }
            else
            {
                folder->appendLeaf(name.mid(pos + 1), file);
                break;
            }
        }
    }
}

}
}
