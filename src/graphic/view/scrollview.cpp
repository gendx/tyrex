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

#include "scrollview.hpp"

#include <QWheelEvent>
#include <QApplication>
#include <QStyle>
#include "graphic/area/area.hpp"
#include "graphic/mainwindow.hpp"

namespace tyrex {
namespace graphic {

ScrollView::ScrollView(Area* area, QWidget* parent) :
    View(parent),
    mArea(area),
    mLayout(new QGridLayout(this)),
    mScrollBar(new QScrollBar(Qt::Vertical))
{
    mLayout->setContentsMargins(QMargins());
    mLayout->setSpacing(0);
    mLayout->addWidget(mArea, 0, 0);
    mLayout->addWidget(mScrollBar, 0, 1);

    QObject::connect(mScrollBar, SIGNAL(valueChanged(int)), mArea, SLOT(scroll(int)));
}


void ScrollView::setScroll(int value)
{
    mScrollBar->setValue(value);
}

void ScrollView::changeScroll(int linecount, int pagestep, int value)
{
    mScrollBar->setMinimum(0);
    if (linecount > pagestep)
    {
        mScrolling = true;
        mScrollBar->setMaximum(linecount - pagestep);
    }
    else
    {
        mScrolling = false;
        mScrollBar->setMaximum(0);
    }
    mScrollBar->setPageStep(pagestep);
    mScrollBar->setValue(value);
}

void ScrollView::wheelEvent(QWheelEvent* event)
{
    if (mScrolling)
    {
        int offset = -event->delta() / 40;
        if ((mScrollBar->value() != mScrollBar->maximum() || offset < 0) && (mScrollBar->value() != 0 || offset > 0))
        {
            mScrollBar->setValue(mScrollBar->value() + offset);
            event->accept();
            return;
        }
    }

    event->ignore();
}

}
}
