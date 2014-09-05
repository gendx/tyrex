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

#ifndef TYREX_FONTVIEW_HPP
#define TYREX_FONTVIEW_HPP

#include "pathview.hpp"
#include "graphic/util/listwidget.hpp"

#include <QHBoxLayout>
#include <QSplitter>

namespace tyrex {
namespace graphic {

class FontView : public View
{
    Q_OBJECT

public:
    FontView(QList<Shared<data::Path> > path, QWidget* parent = 0);

private slots:
    void selected(QModelIndex index);

private:
    static QList<QStringList> pointsOfPath(const Shared<data::Path>& path);

    QHBoxLayout* mLayout;
    QSplitter* mSplitter;
    QSplitter* mSplitter2;
    ListWidget* mListWidget;
    PathWidget* mPathWidget;
    TableView* mTable;

    QList<Shared<data::Path> > mPath;
};

}
}

#endif // TYREX_FONTVIEW_HPP
