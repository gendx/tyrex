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

#ifndef TYREX_TREE_HPP
#define TYREX_TREE_HPP

#include <QString>
#include <memory>

namespace tyrex {

template <typename LeafT>
class TreeLeaf
{
public:
    inline TreeLeaf(QString title, const LeafT& content) :
        mTitle(title), mContent(content) {}

    QString mTitle;
    LeafT mContent;
};

template <typename LeafT, typename NodeT>
class TreeNode
{
public:
    inline TreeNode(QString title) :
        mTitle(title) {}

    void appendLeaf(QString title, const LeafT& content);
    void appendTree(std::shared_ptr<NodeT> tree);

    QString mTitle;
    QList<TreeLeaf<LeafT> > mLeaves;
    QList<std::shared_ptr<NodeT> > mSubtrees;
};

template <typename LeafT>
class Tree : public TreeNode<LeafT, Tree<LeafT> >
{
public:
    inline Tree(QString title) :
        TreeNode<LeafT, Tree<LeafT> >(title) {}
};

}

#endif // TYREX_TREE_HPP
