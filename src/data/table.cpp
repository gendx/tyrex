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

#include "table.hpp"

#include "graphic/view/tableview.hpp"

namespace tyrex {
namespace data {

Table::Table()
{
}

Table::Table(const QString& title, const QStringList& header) :
    mTitle(title),
    mHeader(header)
{
}

Table::Table(const QString& title, const QStringList& header, const QList<QStringList>& content) :
    mTitle(title),
    mHeader(header),
    mContent(content)
{
}


void Table::appendToTree(graphic::TreeNodeModel& tree) const
{
    tree.appendLeaf(mTitle, makeShared<graphic::View, graphic::TableView>(mHeader, mContent));
}

void Table::push(const QString& str1, const QString& str2)
{
    mContent.append(QStringList() << str1 << str2);
}

void Table::push(const QString& str1, const QString& str2, const QString& str3)
{
    mContent.append(QStringList() << str1 << str2 << str3);
}

}
}
