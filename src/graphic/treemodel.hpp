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

#ifndef TYREX_TREEMODEL_HPP
#define TYREX_TREEMODEL_HPP

#include "misc/tree.tpl"
#include "view/view.hpp"
#include <QString>
#include <QStandardItemModel>

namespace tyrex {
namespace graphic {

// A leaf of the tree is a view associated with a title.
typedef TreeLeaf<std::shared_ptr<View> > TreeLeafModel;

// A node of the tree is labelled with a title.
class TreeNodeModel : public TreeNode<std::shared_ptr<View>, TreeNodeModel>
{
public:
    inline TreeNodeModel(QString title) :
        TreeNode<std::shared_ptr<View>, TreeNodeModel>(title) {}

    QStandardItem* makeTreeModel(QStandardItem* rootItem, QMap<QStandardItem*, std::shared_ptr<View> >& itemToView, const std::shared_ptr<View>& firstView, bool& firstViewFound) const;
};

// A tree of views.
class TreeModel
{
public:
    TreeModel(std::shared_ptr<TreeNodeModel> root, QStandardItemModel& model, const std::shared_ptr<View>& firstView = std::shared_ptr<View>());

    inline QModelIndex firstItem() const;
    std::shared_ptr<View> viewForItem(QStandardItem* item) const;

private:
    std::shared_ptr<TreeNodeModel> mRoot;
    QModelIndex mFirstItem;
    QMap<QStandardItem*, std::shared_ptr<View> > mItemToView;
};

inline QModelIndex TreeModel::firstItem() const
    {return mFirstItem;}

}
}

#endif // TYREX_TREEMODEL_HPP
