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

#include "document.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include "parse/parsedocument.hpp"
#include "mainwindow.hpp"

namespace tyrex {
namespace graphic {

Document::Document(const MemChunk& chunk, bool& success, QWidget* parent) :
    QWidget(parent),
    mUntitled(true),
    mData(parse::Document::parse(chunk, parent)),
    mLayout(new QVBoxLayout(this))
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    success = (bool)mData;

    if (success)
    {
        mSplitter = new QSplitter(Qt::Vertical);
        mLayout->setMargin(0);
        mLayout->addWidget(mSplitter);

        this->split();
    }
}


Shared<View> Document::currentView()
{
    return mCurrentSideTree->currentView();
}

bool Document::hasMultipleViews()
{
    return mSideTrees.size() > 1;
}

void Document::focusChanged(SideTree* sideTree)
{
    if (this == MainWindow::getCurrentDocument())
        this->setFocus(sideTree);
}

void Document::setFocus(SideTree* sideTree)
{
    mCurrentSideTree = sideTree;
    emit viewChanged();
}

void Document::split()
{
    Shared<TreeNodeModel> treeModel = mData->treeModel("");
    SideTree* sideTree = new SideTree(treeModel, mData->firstView());
    mSplitter->addWidget(sideTree);

    mSideTrees.append(sideTree);
    this->setFocus(sideTree);

    QObject::connect(sideTree, SIGNAL(viewChanged()), this, SIGNAL(viewChanged()));
    QObject::connect(sideTree, SIGNAL(onFocus(SideTree*)), this, SLOT(focusChanged(SideTree*)));
}

void Document::unsplit()
{
    if (mSideTrees.size() > 1)
    {
        mCurrentSideTree->deleteLater();

        int index = mSideTrees.indexOf(mCurrentSideTree);
        mSideTrees.removeAt(index);

        this->setFocus(mSideTrees[0]);
    }
}


void Document::setPath(const QString& path)
{
    mPath = QFileInfo(path).canonicalFilePath();
    mUntitled = false;
    this->setWindowModified(false);
    this->setWindowTitle(this->userFriendlyPath());
}

void Document::setUntitledPath(const QString& title)
{
    mUntitled = true;
    this->setWindowModified(false);
    this->setWindowTitle(QString("[") + title + "]");
}

}
}
