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

#ifndef CONSOLEDIALOG_HPP
#define CONSOLEDIALOG_HPP

#include <QDialog>
#include <QPushButton>
#include "graphic/console.hpp"

namespace tyrex {
namespace graphic {

class ConsoleDialog : public QDialog
{
    Q_OBJECT

public:
    ConsoleDialog(QString title, QWidget* parent = 0);

    inline void clear();
    inline void append(QString text);

private slots:
    void copy();

private:
    QVBoxLayout* mLayout;
    Console* mConsole;
    QPushButton* mButtonCopy;
    QPushButton* mButtonClose;
};

inline void ConsoleDialog::clear()
    {mConsole->clear();}
inline void ConsoleDialog::append(QString text)
    {mConsole->info(text);}

}
}

#endif // CONSOLEDIALOG_HPP
