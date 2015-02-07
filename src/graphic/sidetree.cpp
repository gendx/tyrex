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

#include "sidetree.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

namespace tyrex {
namespace graphic {

SideTree::SideTree(std::shared_ptr<TreeNodeModel> model, const std::shared_ptr<View>& firstView) :
    mLayout(new QVBoxLayout(this)),
    mSplitter(new QSplitter),
    mTreeView(new QTreeView),
    mStackedWidget(new QStackedWidget),
    mItemModel(new QStandardItemModel(mTreeView)),
    mModel(model, *mItemModel, firstView)
{
    mTreeView->setModel(mItemModel);
    mTreeView->setHeaderHidden(true);
    mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mStackedWidget->setMinimumWidth(50);

    mSplitter->addWidget(mTreeView);
    mSplitter->addWidget(mStackedWidget);
    mSplitter->setSizes(QList<int>() << 1500 << 8500);

    mLayout->setMargin(0);
    mLayout->addWidget(mSplitter);

    mTreeView->selectionModel()->select(mModel.firstItem(), QItemSelectionModel::Select);
    this->selected(mModel.firstItem());

    qApp->installEventFilter(this);

    QObject::connect(mTreeView, SIGNAL(activated(QModelIndex)), this, SLOT(selected(QModelIndex)));
    QObject::connect(mTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selected(QModelIndex)));
}


std::shared_ptr<View> SideTree::currentView()
{
    return mCurrentView;
}

bool SideTree::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->globalPos();
        QRect rect = this->geometry();
        rect.moveTopLeft(this->mapToGlobal(QPoint(0, 0)));

        if (rect.contains(pos))
            emit onFocus(this);
    }

    return false;
}

void SideTree::selected(const QModelIndex& index)
{
    QStandardItem* item = mItemModel->itemFromIndex(index);
    std::shared_ptr<View> view = mModel.viewForItem(item);

    if (view)
    {
        QWidget* old = mStackedWidget->widget(0);
        if (old != view.get())
        {
            mStackedWidget->removeWidget(old);
            mStackedWidget->addWidget(view.get());

            mCurrentView = view;
            emit viewChanged();
        }
    }
}

}
}
