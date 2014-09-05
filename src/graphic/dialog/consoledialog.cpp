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

#include "consoledialog.hpp"

#include "graphic/mainwindow.hpp"

namespace tyrex {
namespace graphic {

ConsoleDialog::ConsoleDialog(QString title, QWidget* parent) :
    QDialog(parent),
    mLayout(new QVBoxLayout(this)),
    mConsole(new Console),
    mButtonCopy(new QPushButton("Copy into console")),
    mButtonClose(new QPushButton("Close"))
{
    this->setWindowTitle(title);

    mLayout->addWidget(mConsole);
    mLayout->addWidget(mButtonCopy);
    mLayout->addWidget(mButtonClose);
    mButtonClose->setFocus();

    QObject::connect(mButtonCopy, SIGNAL(clicked()), this, SLOT(copy()));
    QObject::connect(mButtonClose, SIGNAL(clicked()), this, SLOT(close()));
}


void ConsoleDialog::copy()
{
    MainWindow::console()->append(*mConsole);
}

}
}
