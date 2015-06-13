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

#ifndef TYREX_TREEAREA_HPP
#define TYREX_TREEAREA_HPP

#include "area.hpp"
#include "misc/tree.tpl"

namespace tyrex {
namespace graphic {

class TreeArea : public TextArea
{
    Q_OBJECT

public:
    inline TreeArea(const Tree<void>& tree, ScrollView* parent);

private slots:
    void scroll(int value);

private:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

    unsigned int measureWidth(const Tree<void>& tree) const;
    int drawTree(QPainter& painter, int x, int& y, const Tree<void>& tree);

    Tree<void> mTree;
    unsigned int mLeafCount;
    int mPos;
};

inline TreeArea::TreeArea(const Tree<void>& tree, ScrollView* parent) :
    TextArea(parent), mTree(tree), mLeafCount(mTree.leafCount()), mPos(0) {}

}
}

#endif // TYREX_TREEAREA_HPP
