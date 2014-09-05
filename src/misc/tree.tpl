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

#ifndef TYREX_TREE_TPL
#define TYREX_TREE_TPL

#include "tree.hpp"

namespace tyrex {

template <typename LeafT, typename NodeT>
void TreeNode<LeafT, NodeT>::appendLeaf(QString title, const LeafT& content)
{
    mLeaves.append(TreeLeaf<LeafT>(title, content));
}

template <typename LeafT, typename NodeT>
void TreeNode<LeafT, NodeT>::appendTree(Shared<NodeT> tree)
{
    mSubtrees.append(tree);
}

}

#endif // TYREX_TREE_TPL
