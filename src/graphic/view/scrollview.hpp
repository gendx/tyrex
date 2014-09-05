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

#ifndef TYREX_SCROLLVIEW_HPP
#define TYREX_SCROLLVIEW_HPP

#include <QHBoxLayout>
#include <QScrollBar>

#include "view.hpp"

namespace tyrex {
namespace graphic {

class Area;

// A view with a scrollbar and an inner area.
class ScrollView : public View
{
public:
    ScrollView(Area* area, QWidget* parent);

    void changeScroll(int linecount, int pagestep, int value);
    void setScroll(int value);
    inline int maxScroll() const;

protected:
    void wheelEvent(QWheelEvent* event);

    Area* mArea;

private:
    QGridLayout* mLayout;
    QScrollBar* mScrollBar;

    bool mScrolling;
};

inline int ScrollView::maxScroll() const
    {return mScrollBar->maximum();}

}
}

#endif // TYREX_SCROLLVIEW_HPP
