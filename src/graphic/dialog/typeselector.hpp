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

#ifndef TYREX_TYPESELECTOR_HPP
#define TYREX_TYPESELECTOR_HPP

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include "graphic/util/listwidget.hpp"
#include "graphic/util/treewidget.hpp"

namespace tyrex {
namespace graphic {

class TypeSelector : public QDialog
{
    Q_OBJECT

public:
    static QString select(QWidget* parent, QStringList tips, bool& cancelled);

private slots:
    void validate();
    void cancel();
    void treeActivated();
    void listActivated();

private:
    TypeSelector(QWidget* parent, QStringList tips);

    QString getType();
    void makeTips(QStringList tips);
    void makeTree();

    static const std::vector<QString> mTypes;

    bool mTreeSelected;

    QGridLayout* mLayout;
    TreeWidget* mTreeTypes;
    ListWidget* mListTips;
    QPushButton* mValidButton;
    QPushButton* mCancelButton;
    QMap<QTreeWidgetItem*, unsigned int> mItemToType;
};

}
}

#endif // TYREX_TYPESELECTOR_HPP
