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

#ifndef TYREX_AREA_HPP
#define TYREX_AREA_HPP

#include <QWidget>

namespace tyrex {
namespace graphic {

class ScrollView;

// An area inside a ScrollView.
class Area : public QWidget
{
    Q_OBJECT

public:
    explicit Area(ScrollView* parent);

protected slots:
    virtual void scroll(int value) = 0;

protected:
    void changeScroll(int linecount, int pagestep, int value);
    void setScroll(int value);

private:
    ScrollView* mScrollView;
};


class TextArea : public Area
{
public:
    explicit TextArea(ScrollView* parent);

protected:
    void drawHex(QPainter& painter, unsigned int x, unsigned int y, unsigned int value);
    void drawHexByte(QPainter& painter, unsigned int width, unsigned int x, unsigned int y, unsigned int value);
    void drawASCII(QPainter& painter, unsigned int x, unsigned int y, unsigned char value);
    void drawUnicode(QPainter& painter, unsigned int x, unsigned int y, unsigned int value);

    QFont mFont;
    QFontMetrics mMetrics;
};

}
}

#endif // TYREX_AREA_HPP
