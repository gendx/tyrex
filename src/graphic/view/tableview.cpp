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

#include "tableview.hpp"

#include <QHeaderView>

namespace tyrex {
namespace graphic {

TableView::TableView(const QStringList& header, const QList<QStringList>& content, QWidget* parent) :
    View(parent),
    mLayout(new QHBoxLayout(this)),
    mTreeWidget(new QTreeWidget)
{
    mTreeWidget->setRootIsDecorated(false);

    mLayout->setContentsMargins(QMargins());
    mLayout->addWidget(mTreeWidget);

    mTreeWidget->setHeaderLabels(header);
    for (auto& item : content)
        mTreeWidget->addTopLevelItem(new QTreeWidgetItem(item));

    mTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
}


void TableView::setContent(const QList<QStringList>& content)
{
    mTreeWidget->clear();
    for (auto& item : content)
        mTreeWidget->addTopLevelItem(new QTreeWidgetItem(item));
}

}
}
