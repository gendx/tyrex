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

#include "hexfinddialog.hpp"

#include <QIntValidator>
#include <QLabel>
#include <QEvent>
#include "graphic/area/hexarea.hpp"
#include "misc/memchunk.hpp"
#include "platform-specific/platform-specific.hpp"

namespace tyrex {
namespace graphic {

HexFindDialog::HexFindDialog(QWidget* parent) :
    QDialog(parent),
    mArea(0),
    mLayout(new QVBoxLayout(this)),
    mLineAscii(new QLineEdit),
    mLineHex(new QLineEdit),
    mButtonPrevious(new QPushButton(QIcon(":/icons/left.png"), "&Previous")),
    mButtonNext(new QPushButton(QIcon(":/icons/right.png"), "&Next")),
    mStatusBar(new QStatusBar)
{
    this->setWindowTitle("Find...");

    mLineAscii->setFont(PlatformSpecific::font());
    mLineHex->setFont(PlatformSpecific::font());

    mLineHex->setValidator(new QRegExpValidator(QRegExp("^([0-9A-Fa-f]|\\s)*$"), this));

    mLayout->setContentsMargins(2, 2, 2, 2);
    mLayout->setSpacing(2);

    QHBoxLayout* rowLayout = new QHBoxLayout;
    rowLayout->addWidget(new QLabel("Find :"));
    rowLayout->addWidget(mLineAscii);
    mLayout->addLayout(rowLayout);

    rowLayout = new QHBoxLayout;
    rowLayout->addWidget(new QLabel("Hex :"));
    rowLayout->addWidget(mLineHex);
    mLayout->addLayout(rowLayout);

    rowLayout = new QHBoxLayout;
    rowLayout->addWidget(mButtonPrevious);
    rowLayout->addWidget(mButtonNext);
    mLayout->addLayout(rowLayout);

    mLayout->addWidget(mStatusBar);

    QObject::connect(mLineAscii, SIGNAL(textEdited(QString)), this, SLOT(asciiChanged(QString)));
    QObject::connect(mLineHex, SIGNAL(textEdited(QString)), this, SLOT(hexChanged(QString)));
    QObject::connect(mButtonPrevious, SIGNAL(clicked()), this, SLOT(previous()));
    QObject::connect(mButtonNext, SIGNAL(clicked()), this, SLOT(next()));
}


void HexFindDialog::setArea(HexArea* area)
{
    if (mArea != area)
    {
        if (mArea)
        {
            mArea->clearSearch();
            mArea->setSearching(false);
        }

        mArea = area;
        mArea->setSearching(true);
    }

    MemChunk search = area->currentSelection();
    if (search.size())
        this->searchChanged(search);
    else
        this->hexChanged(mLineHex->text());

    this->show();
    this->activateWindow();
}

void HexFindDialog::attachArea(HexArea* area)
{
    if (this->isVisible() && mArea != area)
    {
        mArea->clearSearch();
        mArea->setSearching(false);

        mArea = area;
        mArea->setSearching(true);
        this->hexChanged(mLineHex->text());
    }
}

void HexFindDialog::detach()
{
    if (mArea)
    {
        mArea->clearSearch();
        mArea->setSearching(false);
    }

    mArea = NULL;
    this->hide();
}


void HexFindDialog::hideEvent(QHideEvent*/* event*/)
{
    if (mArea)
        mArea->clearSearch();
}

void HexFindDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        if (this->isActiveWindow())
            this->setWindowOpacity(1.0);
        else
            this->setWindowOpacity(0.8);
    }
    QDialog::changeEvent(event);
}


void HexFindDialog::previous()
{
    if (mArea)
        mArea->previousSearch();
}

void HexFindDialog::next()
{
    if (mArea)
        mArea->nextSearch();
}


void HexFindDialog::setSearch(const MemChunk& chunk)
{
    unsigned int count = mArea->setSearch(chunk);

    if (count == 0)
        mStatusBar->showMessage("No matches found.");
    else if (count == 1)
        mStatusBar->showMessage("1 match found.");
    else
        mStatusBar->showMessage(QString("%1 matches found.").arg(count));
}


void HexFindDialog::searchChanged(const MemChunk& search)
{
    if (!mArea)
        return;

    QString ascii;
    QString hex;

    for (unsigned int i = 0 ; i < search.size() ; ++i)
    {
        if (i)
            hex.append(' ');

        unsigned int value = search[i];
        unsigned int v1 = (value >> 4) & 0xF;
        unsigned int v2 = value & 0xF;

        if (v1 < 10)
            hex.append('0' + v1);
        else
            hex.append('A' + v1 - 10);

        if (v2 < 10)
            hex.append('0' + v2);
        else
            hex.append('A' + v2 - 10);

        if (value < 0x20 || value >= 0x7F)
            ascii.append('.');
        else
            ascii.append(value);
    }

    mLineAscii->setText(ascii);
    mLineHex->setText(hex);

    this->setSearch(search);
}

void HexFindDialog::asciiChanged(QString ascii)
{
    if (!mArea)
        return;

    MemChunk search;
    QString hex;

    for (unsigned int i = 0 ; i < ascii.size() ; ++i)
    {
        if (i)
            hex.append(' ');

        unsigned int value = ascii[i].unicode();
        unsigned int v1 = (value >> 4) & 0xF;
        unsigned int v2 = value & 0xF;

        if (v1 < 10)
            hex.append('0' + v1);
        else
            hex.append('A' + v1 - 10);

        if (v2 < 10)
            hex.append('0' + v2);
        else
            hex.append('A' + v2 - 10);

        if (value < 0x20 || value >= 0x7F)
        {
            ascii[i] = '.';
            search.appendChar('.');
        }
        else
            search.appendChar(value);
    }

    mLineHex->setText(hex);
    unsigned int cursor = mLineAscii->cursorPosition();
    mLineAscii->setText(ascii);
    mLineAscii->setCursorPosition(cursor);

    this->setSearch(search);
}

void HexFindDialog::hexChanged(QString hex)
{
    if (!mArea)
        return;

    MemChunk search;
    QString ascii;
    QString newhex;

    unsigned int cursor = mLineHex->cursorPosition();
    unsigned int newCursor = 0;
    unsigned int j = 0;
    unsigned int value = 0;

    for (unsigned int i = 0 ; i < hex.size() ; ++i)
    {
        if (cursor == i)
            newCursor = newhex.size();

        if (hex[i] != ' ')
        {
            if (j && j % 2 == 0)
                newhex.append(' ');

            value <<= 4;
            if (hex[i] >= 'a' && hex[i] <= 'f')
            {
                value += hex[i].unicode() - 'a' + 10;
                newhex.append(hex[i].unicode() + 'A' - 'a');
            }
            else
            {
                if (hex[i] >= 'A' && hex[i] <= 'F')
                    value += hex[i].unicode() - 'A' + 10;
                else
                    value += hex[i].unicode() - '0';
                newhex.append(hex[i]);
            }
            ++j;

            if (j % 2 == 0)
            {
                QChar c('.');
                if (value >= 0x20 && value < 0x7F)
                    c = value;
                ascii.append(c);
                search.appendChar(value);
                value = 0;
            }
        }
    }

    if (cursor == hex.size())
        newCursor = newhex.size();

    mLineHex->setText(newhex);
    mLineHex->setCursorPosition(newCursor);
    mLineAscii->setText(ascii);

    this->setSearch(search);
}

}
}
