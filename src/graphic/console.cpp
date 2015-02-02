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
    mList(new QListWidget)
{
    mLayout->setMargin(0);
    mLayout->addWidget(mList);
    mList->setFont(PlatformSpecific::font());
}


void Console::clear()
{
    mItems.clear();
    mList->clear();
}

void Console::printInfo(QString text)
{
    mItems.append(QPair<QString, QString>(":/icons/help.png", text));
    mList->addItem(new QListWidgetItem(QIcon(":/icons/help.png"), text));
}


void Console::append(const Console& other)
{
    for (auto& item : other.mItems)
    {
        mItems.append(item);
        mList->addItem(new QListWidgetItem(QIcon(item.first), item.second));
    }
}

}
}
