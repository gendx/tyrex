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

#ifndef TYREX_TABLEVIEW_HPP
#define TYREX_TABLEVIEW_HPP

#include "view.hpp"

#include <QHBoxLayout>
#include <QTreeWidget>

namespace tyrex {
namespace graphic {

class TableView : public View
{
    Q_OBJECT

public:
    TableView(const QStringList& header, const QList<QStringList>& content, QWidget* parent = 0);

    void setContent(const QList<QStringList>& content);

signals:
    void selected(int);

private slots:
    void selected(QModelIndex index);

private:
    QHBoxLayout* mLayout;
    QTreeWidget* mTreeWidget;
};

}
}

#endif // TYREX_TABLEVIEW_HPP
