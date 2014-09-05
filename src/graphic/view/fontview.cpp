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

#include "fontview.hpp"

namespace tyrex {
namespace graphic {

FontView::FontView(QList<Shared<data::Path> > path, QWidget* parent) :
    View(parent),
    mLayout(new QHBoxLayout(this)),
    mSplitter(new QSplitter),
    mSplitter2(new QSplitter(Qt::Vertical)),
    mPathWidget(new PathWidget),
    mPath(path)
{
    mLayout->setMargin(0);
    mLayout->addWidget(mSplitter);

    mSplitter->addWidget(mPathWidget);
    mSplitter->addWidget(mSplitter2);

    QList<QStringList> content;
    if (!path.isEmpty())
    {
        mPathWidget->setPath(path.first());
        content = FontView::pointsOfPath(path.first());
    }

    mTable = new TableView(QStringList() << "x" << "y" << "type", content);
    mSplitter2->addWidget(mTable);

    mListWidget = new ListWidget;
    mSplitter2->addWidget(mListWidget);

    for (unsigned int i = 0 ; i < path.size() ; ++i)
        mListWidget->addItem(QString("Glyph ") + QString::number(i));

    QObject::connect(mListWidget, SIGNAL(activated(QModelIndex)), this, SLOT(selected(QModelIndex)));
    QObject::connect(mListWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(selected(QModelIndex)));
}


void FontView::selected(QModelIndex index)
{
    int i = index.row();

    mPathWidget->setPath(mPath[i]);
    mTable->setContent(FontView::pointsOfPath(mPath[i]));
}

QList<QStringList> FontView::pointsOfPath(const Shared<data::Path>& path)
{
    QList<QStringList> content;
    for (const data::Path::PointInfo& pi : path->points())
    {
        QString type;
        switch (pi.flags)
        {
        case 0:
            type = "point";
            break;
        case 1:
            type = "move";
            break;
        case 2:
            type = "ctrl";
        }

        content.append(QStringList() << QString::number(pi.p.x) << QString::number(pi.p.y) << type);
    }

    return content;
}

}
}
