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

#include "area.hpp"

#include <QPainter>
#include "graphic/view/scrollview.hpp"
#include "platform-specific/platform-specific.hpp"

namespace tyrex {
namespace graphic {

Area::Area(ScrollView* parent) :
    mScrollView(parent)
{
    this->setFocusPolicy(Qt::ClickFocus);
}


void Area::changeScroll(int linecount, int pagestep, int value)
{
    mScrollView->changeScroll(linecount, pagestep, value);
}

void Area::setScroll(int value)
{
    if (value < 0)
        value = 0;
    else if (value > mScrollView->maxScroll())
        value = mScrollView->maxScroll();
    mScrollView->setScroll(value);
    this->scroll(value);
}


TextArea::TextArea(ScrollView* parent) :
    Area(parent),
    mFont(PlatformSpecific::font()),
    mMetrics(mFont)
{
}


void TextArea::drawHex(QPainter& painter, unsigned int x, unsigned int y, unsigned int value)
{
    QChar c('?');
    if (value < 10)
        c = '0' + value;
    else if (value < 16)
        c = 'A' + value - 10;

    painter.drawText(x, y, c);
}

void TextArea::drawHexByte(QPainter& painter, unsigned int width, unsigned int x, unsigned int y, unsigned int value)
{
    this->drawHex(painter, x, y, value >> 4);
    this->drawHex(painter, x + width, y, value & 0xF);
}

void TextArea::drawASCII(QPainter& painter, unsigned int x, unsigned int y, unsigned char value)
{
    QChar c('.');
    if (value >= 0x20 && value < 0x7F)
        c = value;

    painter.drawText(x, y, c);
}

void TextArea::drawUnicode(QPainter& painter, unsigned int x, unsigned int y, unsigned int value)
{
    painter.drawText(x, y, QChar(value));
}

}
}
