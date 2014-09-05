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

#ifndef TYREX_DATA_TABLE_HPP
#define TYREX_DATA_TABLE_HPP

#include "data.hpp"

namespace tyrex {
namespace data {

class Table : public Data
{
public:
    Table();
    Table(const QString& title, const QStringList& header);
    Table(const QString& title, const QStringList& header, const QList<QStringList>& content);

    void appendToTree(graphic::TreeNodeModel& tree) const;

    inline void setTitle(const QString& title);
    inline void setHeader(const QStringList& header);
    inline void setContent(const QList<QStringList>& content);

    void push(const QString& str1, const QString& str2);
    void push(const QString& str1, const QString& str2, const QString& str3);

private:
    QString mTitle;
    QStringList mHeader;
    QList<QStringList> mContent;
};


inline void Table::setTitle(const QString& title)
    {mTitle = title;}
inline void Table::setHeader(const QStringList& header)
    {mHeader = header;}
inline void Table::setContent(const QList<QStringList>& content)
    {mContent = content;}

}
}

#endif // TYREX_DATA_TABLE_HPP
