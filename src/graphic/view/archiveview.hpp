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

#ifndef TYREX_ARCHIVEVIEW_HPP
#define TYREX_ARCHIVEVIEW_HPP

#include "data/file.hpp"
#include "misc/tree.hpp"
#include "view.hpp"

#include <QHBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>

namespace tyrex {
namespace graphic {

class ArchiveView : public View
{
public:
    ArchiveView(const data::FileInfoFilter& fileInfoFilter, const Tree<data::File>& files, QWidget* parent = 0);

private:
    void makeTree();
    void appendToTree(QStandardItem* root, const Tree<data::File>& folder);
    void appendToTree(QStandardItem* root, const TreeLeaf<data::File>& file);

    const data::FileInfoFilter& mFileInfoFilter;
    const Tree<data::File>& mFiles;

    QHBoxLayout* mLayout;
    QTreeView* mTreeView;
    QHeaderView* mHeaderView;
    QStandardItemModel* mModelFiles;
};

}
}

#endif // TYREX_ARCHIVEVIEW_HPP
