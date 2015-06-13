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

#include "console.hpp"

#include "platform-specific/platform-specific.hpp"

namespace tyrex {
namespace graphic {

Console::Console(QWidget* parent) :
    QWidget(parent),
    mLayout(new QHBoxLayout(this)),
    mEdit(new QTextEdit)
{
    mLayout->setMargin(0);
    mLayout->addWidget(mEdit);
    mEdit->setFont(PlatformSpecific::font());
    mEdit->setReadOnly(true);
}


void Console::clear()
{
    mEdit->clear();
}

void Console::append(QString text, QColor color)
{
    mEdit->setTextColor(color);
    mEdit->append(text);
}

void Console::append(const Console& other)
{
    mEdit->append(other.mEdit->toHtml());
}

}
}
