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

#include "treemodel.hpp"

namespace tyrex {
namespace graphic {

QStandardItem* TreeNodeModel::makeTreeModel(QStandardItem* rootItem, QMap<QStandardItem*, std::shared_ptr<View> >& itemToView, const std::shared_ptr<View>& firstView, bool& firstViewFound) const
{
    QStandardItem* firstItem = 0;

    for (auto& leaf : mLeaves)
    {
        QStandardItem* item = new QStandardItem(leaf.mTitle);
        rootItem->appendRow(item);
        itemToView[item] = leaf.mContent;

        if (leaf.mContent == firstView)
        {
            firstViewFound = true;
            firstItem = item;
        }
        else if (!firstItem)
            firstItem = item;
    }

    for (auto& subtree : mSubtrees)
    {
        QStandardItem* item = new QStandardItem(subtree->mTitle);
        rootItem->appendRow(item);
        itemToView[item] = std::shared_ptr<View>();

        bool found = false;
        item = subtree->makeTreeModel(item, itemToView, firstView, found);

        if (found)
        {
            firstViewFound = true;
            firstItem = item;
        }
        else if (!firstItem)
            firstItem = item;
    }

    return firstItem;
}


TreeModel::TreeModel(std::shared_ptr<TreeNodeModel> root, QStandardItemModel& model, const std::shared_ptr<View>& firstView) :
    mRoot(root)
{
    bool found;
    mFirstItem = model.indexFromItem(mRoot->makeTreeModel(model.invisibleRootItem(), mItemToView, firstView, found));
}

std::shared_ptr<View> TreeModel::viewForItem(QStandardItem* item) const
{
    return mItemToView[item];
}

}
}
