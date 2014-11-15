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

#include "typeselector.hpp"

namespace tyrex {
namespace graphic {

const std::vector<QString> TypeSelector::mTypes {
    "binary",
    "archive/tar",
    "archive/zip",
    "compress/bzip2",
    "compress/deflate",
    "compress/gzip",
    "compress/lzma",
    "compress/lzma2",
    "compress/zlib",
    "font/truetype",
    "image/png",
    "program/elf32",
    "program/java"
};


QString TypeSelector::select(QWidget* parent, QStringList tips, bool& cancelled)
{
    TypeSelector selector(parent, tips);
    selector.exec();
    if (selector.result() == QDialog::Rejected)
    {
        cancelled = true;
        return QString();
    }

    cancelled = false;
    return selector.getType();
}


TypeSelector::TypeSelector(QWidget* parent, QStringList tips) :
    QDialog(parent),
    mTreeSelected(true),
    mLayout(new QGridLayout(this)),
    mTreeTypes(new TreeWidget),
    mListTips(new ListWidget),
    mValidButton(new QPushButton("Accept")),
    mCancelButton(new QPushButton("Cancel"))
{
    mTreeTypes->setHeaderHidden(true);
    mListTips->setSelectionMode(QAbstractItemView::SingleSelection);

    mLayout->addWidget(mTreeTypes, 0, 0);
    mLayout->addWidget(mListTips, 0, 1);
    mLayout->addWidget(mValidButton, 1, 0);
    mLayout->addWidget(mCancelButton, 1, 1);

    this->makeTree();
    this->makeTips(tips);

    QObject::connect(mValidButton, SIGNAL(clicked()), this, SLOT(validate()));
    QObject::connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    QObject::connect(mTreeTypes, SIGNAL(focusIn()), this, SLOT(treeActivated()));
    QObject::connect(mListTips, SIGNAL(focusIn()), this, SLOT(listActivated()));
}


void TypeSelector::validate()
{
    // TODO: check

    this->accept();
}

void TypeSelector::cancel()
{
    this->reject();
}

void TypeSelector::treeActivated()
{
    mTreeSelected = true;
}

void TypeSelector::listActivated()
{
    mTreeSelected = false;
}


QString TypeSelector::getType()
{
    if (mTreeSelected)
    {
        QList<QTreeWidgetItem*> items = mTreeTypes->selectedItems();
        if (items.size() != 1)
            return QString();

        QTreeWidgetItem* item = items[0];
        if (mItemToType.contains(item))
        {
            unsigned int i = mItemToType[item];
            if (i < mTypes.size())
                return mTypes[i];
        }
    }
    else
    {
        QList<QListWidgetItem*> items = mListTips->selectedItems();
        if (items.size() != 1)
            return QString();

        return items[0]->text();
    }

    return QString();
}


void TypeSelector::makeTips(QStringList tips)
{
    for (unsigned int i = 0 ; i < tips.size() ; ++i)
    {
        QListWidgetItem* item = new QListWidgetItem(tips[i]);
        mListTips->addItem(item);

        if (!i)
        {
            mListTips->setCurrentItem(item);
            mTreeSelected = false;
        }
    }
}

void TypeSelector::makeTree()
{
    QMap<QString, QTreeWidgetItem*> nameToItem;

    for (unsigned int i = 0 ; i < mTypes.size() ; ++i)
    {
        QString str = mTypes[i];
        QTreeWidgetItem* item = 0;

        for (int pos = -1 ; ; )
        {
            int nextpos = str.indexOf("/", pos + 1);

            if (nextpos >= 0)
            {
                QString key = str.left(nextpos + 1);

                if (nameToItem.contains(key))
                    item = nameToItem[key];
                else
                {
                    if (pos < 0)
                        item = new QTreeWidgetItem(mTreeTypes, QStringList(str.mid(pos + 1, nextpos - pos - 1)));
                    else
                        item = new QTreeWidgetItem(item, QStringList(str.mid(pos + 1, nextpos - pos - 1)));

                    nameToItem[key] = item;
                }

                pos = nextpos;
            }
            else
            {
                if (pos < 0)
                    item = new QTreeWidgetItem(mTreeTypes, QStringList(str.mid(pos + 1)));
                else
                    item = new QTreeWidgetItem(item, QStringList(str.mid(pos + 1)));

                nameToItem[str] = item;
                mItemToType[item] = i;

                break;
            }
        }
    }
}

}
}
