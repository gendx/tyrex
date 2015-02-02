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

#include "inputdialog.hpp"

namespace tyrex {
namespace graphic {

bool InputDialog::getUint(unsigned int& value, QString prompt, QWidget* parent)
{
    InputDialog dialog(prompt, Validator::_uint, parent);

    if (dialog.exec() == QDialog::Accepted)
    {
        value = dialog.mLineEdit->text().toUInt();
        return true;
    }
    else
        return false;
}


InputDialog::InputDialog(QString prompt, Validator::Type type, QWidget* parent) :
    QDialog(parent),
    mLayout(new QGridLayout(this)),
    mLabelPrompt(new QLabel(prompt)),
    mLineEdit(new QLineEdit),
    mValidButton(new QPushButton("Accept")),
    mCancelButton(new QPushButton("Cancel")),
    mValidator(new Validator(type, mLineEdit))
{
    mLineEdit->setValidator(mValidator);

    mLayout->addWidget(mLabelPrompt, 0, 0, 1, 2);
    mLayout->addWidget(mLineEdit, 1, 0, 1, 2);
    mLayout->addWidget(mValidButton, 2, 0);
    mLayout->addWidget(mCancelButton, 2, 1);

    QObject::connect(mValidButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(mCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


InputDialog::Validator::Validator(Type type, QObject* parent) :
    QValidator(parent),
    mType(type)
{
}


QValidator::State InputDialog::Validator::validate(QString& input, int& pos) const
{
    switch (mType)
    {
    case _uint:
        if (!isNumber(input))
            return QValidator::Invalid;
        if (input.size() > 10)
            return QValidator::Invalid;

        if (numberCompare(input, QString("4294967296")))
            return QValidator::Acceptable;
        return QValidator::Intermediate;
        break;
    }

    return QValidator::Invalid;
}

bool InputDialog::Validator::isNumber(QString input) const
{
    for (auto it : input)
    {
        unsigned int code = it.unicode();
        if (code < QChar('0').unicode() || code > QChar('9').unicode())
            return false;
    }

    return true;
}

bool InputDialog::Validator::numberCompare(QString lhs, QString rhs)
{
    while (lhs.size() && lhs[0] == '0')
        lhs = lhs.right(lhs.size() - 1);
    while (rhs.size() && rhs[0] == '0')
        rhs = rhs.right(rhs.size() - 1);

    if (lhs.size() != rhs.size())
        return lhs.size() < rhs.size();

    for (unsigned int i = 0 ; i < lhs.size() ; ++i)
    {
        if (lhs[i].unicode() < rhs[i].unicode())
            return true;
        else if (lhs[i].unicode() > rhs[i].unicode())
            return false;
    }

    return false;
}

}
}
