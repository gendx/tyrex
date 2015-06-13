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

#include "treearea.hpp"

#include <QPainter>

namespace tyrex {
namespace graphic {

void TreeArea::scroll(int value)
{
    mPos = value;
    this->update();
}

void TreeArea::resizeEvent(QResizeEvent*)
{
    unsigned int height = mMetrics.height();
    unsigned int lineSpacing = mMetrics.lineSpacing();

    unsigned int pagestep = (this->height() + lineSpacing - height) / lineSpacing;
    emit changeScroll(mLeafCount, pagestep, mPos);

    this->update();
}

void TreeArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), Qt::white);
    painter.setFont(mFont);
    painter.setPen(Qt::black);

    int y = -mPos;
    this->drawTree(painter, 1, y, mTree);
}

int TreeArea::drawTree(QPainter& painter, int x, int& y, const Tree<void>& tree)
{
    unsigned int width = mMetrics.width("0");
    unsigned int height = mMetrics.height();
    unsigned int lineSpacing = mMetrics.lineSpacing();

    if (tree.mSubtrees.empty())
    {
        painter.drawText((x + 1) * width, height + y * lineSpacing, tree.mTitle);
        int result = y * lineSpacing;
        ++y;
        return result;
    }
    else
    {
        unsigned int yoffset = 2 * height / 3;

        std::vector<int> positions;
        for (auto& subtree : tree.mSubtrees)
        {
            int ypos = this->drawTree(painter, x + 2, y, *subtree);
            painter.drawLine(x * width, yoffset + ypos, (x + 2) * width, yoffset + ypos);

            positions.push_back(ypos);
        }

        unsigned int middle = positions.size() / 2;
        unsigned int yorigin = 0;
        if (positions.size() % 2)
            yorigin = positions[middle];
        else
            yorigin = (positions[middle] + positions[middle - 1]) / 2;

        for (unsigned int i = 0 ; i < positions.size() ; ++i)
            painter.drawLine(x * width, yoffset + yorigin, x * width, yoffset + positions[i]);

        return yorigin;
    }
}

}
}
