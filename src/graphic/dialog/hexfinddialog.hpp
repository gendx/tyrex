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

#ifndef TYREX_HEXFINDDIALOG_HPP
#define TYREX_HEXFINDDIALOG_HPP

#include "misc/memchunk.hpp"
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>

namespace tyrex {
namespace graphic {

class HexArea;

class HexFindDialog : public QDialog
{
    Q_OBJECT

public:
    HexFindDialog(QWidget* parent);

    void setArea(HexArea* area);
    void attachArea(HexArea* area);
    void detach();

private slots:
    void searchChanged(const MemChunk& search);
    void asciiChanged(QString str);
    void hexChanged(QString str);
    void previous();
    void next();

private:
    void hideEvent(QHideEvent* event);
    void changeEvent(QEvent* event);

    void setSearch(const MemChunk& chunk);

    HexArea* mArea;

    QVBoxLayout* mLayout;
    QLineEdit* mLineAscii;
    QLineEdit* mLineHex;
    QPushButton* mButtonPrevious;
    QPushButton* mButtonNext;
    QStatusBar* mStatusBar;
};

}
}

#endif // TYREX_HEXFINDDIALOG_HPP
