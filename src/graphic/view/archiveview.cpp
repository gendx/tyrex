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

#include "archiveview.hpp"

namespace tyrex {
namespace graphic {

ArchiveView::ArchiveView(const data::FileInfoFilter& fileInfoFilter, const Tree<data::File>& files, QWidget* parent) :
    View(parent),
    mFileInfoFilter(fileInfoFilter),
    mFiles(files),
    mLayout(new QHBoxLayout(this)),
    mTreeView(new QTreeView),
    mHeaderView(mTreeView->header()),
    mModelFiles(new QStandardItemModel(mTreeView))
{
    mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mLayout->setContentsMargins(QMargins());
    mLayout->addWidget(mTreeView);

    this->makeTree();
    mTreeView->setModel(mModelFiles);

    for (unsigned int i = 0 ; i < mFileInfoFilter.mInfos.size() ; ++i)
        mHeaderView->setSectionHidden(i, !mFileInfoFilter.mInfos[i].second);
    mHeaderView->setResizeMode(QHeaderView::ResizeToContents);
}


void ArchiveView::makeTree()
{
    QStringList titles;
    for (auto& info : mFileInfoFilter.mInfos)
        titles.append(data::FileInfo::infoToString(info.first));
    mModelFiles->setHorizontalHeaderLabels(titles);

    QStandardItem* root = mModelFiles->invisibleRootItem();

    for (auto& subtree : mFiles.mSubtrees)
        this->appendToTree(root, *subtree);
    for (auto& leaf : mFiles.mLeaves)
        this->appendToTree(root, leaf);
}

void ArchiveView::appendToTree(QStandardItem* root, const Tree<data::File>& folder)
{
    QStandardItem* item = new QStandardItem(folder.mTitle);

    for (auto& subtree : folder.mSubtrees)
        this->appendToTree(item, *subtree);
    for (auto& leaf : folder.mLeaves)
        this->appendToTree(item, leaf);

    root->appendRow(item);
}

void ArchiveView::appendToTree(QStandardItem* root, const TreeLeaf<data::File>& file)
{
    QList<QStandardItem*> item;
    const data::FileInfo& info = file.mContent.mInfo;

    item.append(new QStandardItem(file.mTitle));
    for (unsigned int i = 1 ; i < mFileInfoFilter.mInfos.size() ; ++i)
        item.append(new QStandardItem(info[mFileInfoFilter.mInfos[i].first]));

    root->appendRow(item);
}

}
}
