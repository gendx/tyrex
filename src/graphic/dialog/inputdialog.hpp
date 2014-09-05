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

#ifndef TYREX_INPUTDIALOG_HPP
#define TYREX_INPUTDIALOG_HPP

#include <QDialog>
#include <QValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace tyrex {
namespace graphic {

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    static bool getUint(unsigned int& value, QString prompt, QWidget* parent);

private:
    class Validator : public QValidator
    {
    public:
        enum Type {
            _uint
        };

        Validator(Type type, QObject* parent);

        QValidator::State validate(QString& input, int& pos) const;

    private:
        bool isNumber(QString input) const;
        static bool numberCompare(QString lhs, QString rhs);

        Type mType;
    };

    InputDialog(QString prompt, Validator::Type type, QWidget* parent);

    QGridLayout* mLayout;
    QLabel* mLabelPrompt;
    QLineEdit* mLineEdit;
    QPushButton* mValidButton;
    QPushButton* mCancelButton;
    Validator* mValidator;
};

}
}

#endif // TYREX_INPUTDIALOG_HPP
