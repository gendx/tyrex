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

#ifndef TYREX_SIDETREE_HPP
#define TYREX_SIDETREE_HPP

#include "treemodel.hpp"
#include <QTreeView>
#include <QSplitter>
#include <QBoxLayout>
#include <QStackedWidget>

namespace tyrex {
namespace graphic {

class SideTree : public QWidget
{
    Q_OBJECT

public:
    SideTree(Shared<TreeNodeModel> model, const Shared<View>& firstView);

    Shared<View> currentView();

signals:
    void viewChanged();
    void onFocus(SideTree*);

private slots:
    void selected(const QModelIndex& index);

private:
    bool eventFilter(QObject* obj, QEvent* event);

    QVBoxLayout* mLayout;
    QSplitter* mSplitter;
    QTreeView* mTreeView;
    QStackedWidget* mStackedWidget;
    QStandardItemModel* mItemModel;
    TreeModel mModel;
    Shared<View> mCurrentView;
};

}
}

#endif // TYREX_SIDETREE_HPP
