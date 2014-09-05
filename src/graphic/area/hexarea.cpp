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

#include "hexarea.hpp"

#include <QPainter>
#include <QTimerEvent>
#include <QMouseEvent>
#include <cstring>
#include <cmath>
#include "graphic/mainwindow.hpp"

namespace tyrex {
namespace graphic {

HexArea::~HexArea()
{
    if (mSearching)
        MainWindow::detachFindDialog();
}

unsigned int HexArea::setSearch(MemChunk search)
{
    mSearchLength = search.size();
    mFound.clear();

    if (mSearchLength)
    {
        for (unsigned int i = 0 ; i + search.size() <= mChunk.size() ; ++i)
            if (!mChunk.uncompare(search.data(), search.size(), i))
                mFound.append(i);
    }

    this->update();
    return mFound.size();
}

void HexArea::clearSearch()
{
    mSearchLength = 0;
    mFound.clear();
    this->update();
}

void HexArea::previousSearch()
{
    if (mFound.size())
    {
        QList<unsigned int>::const_iterator it = qLowerBound(mFound, mCursor);
        unsigned int pos;

        if (it != mFound.begin())
        {
            --it;
            pos = *it;
        }
        else
            pos = mFound.last();

        this->setSelection(pos, mSearchLength);
    }
}

void HexArea::nextSearch()
{
    if (mFound.size())
    {
        QList<unsigned int>::const_iterator it = qUpperBound(mFound, mCursor);
        unsigned int pos;

        if (it != mFound.end())
            pos = *it;
        else
            pos = mFound.first();

        this->setSelection(pos, mSearchLength);
    }
}


void HexArea::setCursor(unsigned int pos, bool checkScroll)
{
    unsigned int height = mMetrics.height();
    unsigned int lineSpacing = mMetrics.lineSpacing();
    unsigned int countVert = (this->height() + lineSpacing - height) / lineSpacing;

    if (checkScroll)
    {
        if (pos < mPos)
            this->setScroll(pos / this->charsPerLine());
        else if (pos >= mPos + countVert * this->charsPerLine())
            this->setScroll(pos / this->charsPerLine() - countVert + 1);
    }

    mCursor = pos;
    mCursorTimer.stop();
    this->setTimer();
}

void HexArea::setSelection(unsigned int pos, unsigned int length, bool cursor)
{
    mSelectStart = pos;
    mSelectLength = length;
    if (cursor)
        this->setCursor(pos);
}

void HexArea::setSelectAndCursor(unsigned int pos, unsigned int length, unsigned int cursor)
{
    mSelectStart = pos;
    mSelectLength = length;
    this->setCursor(cursor);
}

void HexArea::extendSelection(unsigned int pos, bool extend)
{
    if (extend)
    {
        unsigned int end = mSelectStart + mSelectLength - 1;
        if (mSelectLength == 0)
        {
            if (pos < mSelectStart)
                this->setSelectAndCursor(pos, mSelectStart - pos + 1, pos);
            else
                this->setSelectAndCursor(mSelectStart, pos - mSelectStart + 1, pos);
        }
        else if (mCursor == mSelectStart)
        {
            if (pos <= end)
                this->setSelectAndCursor(pos, end - pos + 1, pos);
            else
                this->setSelectAndCursor(end, pos - end + 1, pos);
        }
        else
        {
            if (pos >= mSelectStart)
                this->setSelectAndCursor(mSelectStart, pos - mSelectStart + 1, pos);
            else
                this->setSelectAndCursor(pos, mSelectStart - pos + 1, pos);
        }
    }
    else
        this->setSelection(pos);
}

void HexArea::clearSelection()
{
    mSelectStart = 0;
    mSelectLength = 0;
    this->update();
}

void HexArea::setTimer()
{
    mCursorVisible = true;
    mCursorTimer.start(500, this);
    this->update();
}

void HexArea::clearTimer()
{
    mCursorVisible = false;
    mCursorTimer.stop();
    mScrollTimer.stop();
    this->update();
}

void HexArea::selectionForMouse(const QPoint& mousePos, bool checkTimer)
{
    int dx, dy, pos;
    this->mouseToPos(mousePos, dx, dy, pos);

    if (checkTimer)
    {
        if (dy != 0 && !mScrollTimer.isActive())
            mScrollTimer.start(100, this);
        else if (dy == 0 && mScrollTimer.isActive())
            mScrollTimer.stop();
    }

    if (pos >= 0 && pos < mChunk.size())
    {
        if (pos < mPosClick)
            this->setSelection(pos, mPosClick - pos + 1, false);
        else
            this->setSelection(mPosClick, pos - mPosClick + 1, false);
        this->setCursor(pos, false);
    }
}


void HexArea::scroll(int value)
{
    mPos = this->charsPerLine() * value;
    if (mPosClick >= 0)
        this->selectionForMouse(mLastPos, false);
    this->update();
}

void HexArea::showEvent(QShowEvent*)
{
    MainWindow::attachFindDialog(this);
}

void HexArea::focusInEvent(QFocusEvent*)
{
    this->setTimer();
    MainWindow::attachFindDialog(this);
}

void HexArea::focusOutEvent(QFocusEvent*)
{
    this->clearTimer();
}

void HexArea::timerEvent(QTimerEvent* event)
{
    if (this->isVisible() && event->timerId() == mCursorTimer.timerId())
    {
        mCursorVisible = !mCursorVisible;
        this->update();
    }
    else if (event->timerId() == mScrollTimer.timerId())
    {
        int dx, dy, pos;
        this->mouseToPos(mLastPos, dx, dy, pos);

        if (dy != 0)
            this->setScroll(mPos / this->charsPerLine() + dy);
    }
}

void HexArea::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

    mPosClick = this->mouseToPos(event->pos());
    if (mPosClick >= 0)
        this->extendSelection(mPosClick, mShift);
}

void HexArea::mouseReleaseEvent(QMouseEvent*)
{
    mPosClick = -1;
    mScrollTimer.stop();
}

void HexArea::mouseMoveEvent(QMouseEvent* event)
{
    if (mPosClick >= 0)
    {
        mLastPos = event->pos();
        this->selectionForMouse(mLastPos);
    }
}

void HexArea::keyReleaseEvent(QKeyEvent* event)
{
    mShift = event->modifiers() & Qt::ShiftModifier;
}

void HexArea::keyPressEvent(QKeyEvent* event)
{
    mShift = event->modifiers() & Qt::ShiftModifier;

    if (event->matches(QKeySequence::SelectAll))
    {
        this->setSelection(0, mChunk.size());
    }
    else if (event->key() == Qt::Key_Home)
    {
        if (mChunk.size())
            this->extendSelection(0, mShift);
    }
    else if (event->key() == Qt::Key_End)
    {
        if (mChunk.size())
            this->extendSelection(mChunk.size() - 1, mShift);
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (mCursor > 0)
            this->extendSelection(mCursor - 1, mShift);
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (mCursor + 1 < mChunk.size())
            this->extendSelection(mCursor + 1, mShift);
    }
    else if (event->key() == Qt::Key_Down)
    {
        unsigned int pos = mCursor + this->charsPerLine();
        if (pos >= mChunk.size())
            pos = mChunk.size() - 1;
        if (mChunk.size())
            this->extendSelection(pos, mShift);
    }
    else if (event->key() == Qt::Key_Up)
    {
        int pos = mCursor - this->charsPerLine();
        if (pos < 0)
            pos = 0;
        if (mChunk.size())
            this->extendSelection(pos, mShift);
    }
    else if (event->key() == Qt::Key_PageDown)
    {
        unsigned int height = mMetrics.height();
        unsigned int lineSpacing = mMetrics.lineSpacing();
        unsigned int countVert = (this->height() + lineSpacing - height) / lineSpacing;

        int pos = mCursor + countVert * this->charsPerLine();
        if (pos >= mChunk.size())
            pos = mChunk.size() - 1;
        if (mChunk.size())
            this->extendSelection(pos, mShift);
    }
    else if (event->key() == Qt::Key_PageUp)
    {
        unsigned int height = mMetrics.height();
        unsigned int lineSpacing = mMetrics.lineSpacing();
        unsigned int countVert = (this->height() + lineSpacing - height) / lineSpacing;

        int pos = mCursor - countVert * this->charsPerLine();
        if (pos < 0)
            pos = 0;
        if (mChunk.size())
            this->extendSelection(pos, mShift);
    }
    else
        TextArea::keyPressEvent(event);
}

void HexArea::resizeEvent(QResizeEvent*/* event*/)
{
    unsigned int height = mMetrics.height();
    unsigned int lineSpacing = mMetrics.lineSpacing();

    unsigned int countHoriz = this->charsPerLine();
    unsigned int countVert = (this->height() + lineSpacing - height) / lineSpacing;

    if (countHoriz)
    {
        unsigned int linecount = (mChunk.size() + countHoriz - 1) / countHoriz;
        unsigned int pagestep = countVert;
        unsigned int value = mPos / countHoriz;

        this->changeScroll(linecount, pagestep, value);
    }

    this->update();
}

void HexArea::paintEvent(QPaintEvent*/* event*/)
{
    unsigned int width = mMetrics.width("0");
    unsigned int height = mMetrics.height();
    unsigned int lineSpacing = mMetrics.lineSpacing();
    unsigned int descent = mMetrics.descent();

    unsigned int countPlaces = this->width() / width;
    unsigned int countHoriz;
    if (countPlaces >= 27)
        countHoriz = (countPlaces - 11) / 4;
    else
        countHoriz = (countPlaces - 2) / 4;

    unsigned int countVert = (this->height() + lineSpacing - height) / lineSpacing;

    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), Qt::white);
    painter.setFont(mFont);
    painter.setPen(Qt::black);

    unsigned int leftmargin = 0;
    if (countPlaces >= 27)
        leftmargin = 9;
    mColorizer.colorize(painter, mPos, width, height, lineSpacing, descent, countPlaces, countHoriz, countVert, leftmargin);

    QFont boldFont = mFont;
    boldFont.setBold(true);

    unsigned int posSearch = 0;
    unsigned int pos = mPos;
    for (unsigned int y = 0 ; y < countVert ; ++y)
    {
        if (pos < mChunk.size() && countPlaces >= 27)
        {
            for (unsigned int i = 0 ; i < 8 ; ++i)
                this->drawHex(painter, (i + 0.5) * width, height + y * lineSpacing, (pos >> ((7 - i) << 2)) & 0x0F);

            painter.drawLine(leftmargin * width, (y + 1) * lineSpacing + descent,
                             leftmargin * width, y * lineSpacing + descent);
        }

        for (unsigned int x = 0 ; x < countHoriz ; ++x)
        {
            if (pos < mChunk.size())
            {
                while (posSearch < mFound.size() && pos >= mFound[posSearch] + mSearchLength)
                    ++posSearch;

                painter.save();

                if (posSearch < mFound.size() && pos >= mFound[posSearch] && pos < mFound[posSearch] + mSearchLength)
                {
                    painter.setFont(boldFont);
                    painter.fillRect((3 * x + 0.5 + leftmargin) * width, descent + y * lineSpacing, 3 * width, height, QColor(0, 0, 0, 0x80));
                    painter.fillRect((countPlaces + x - countHoriz - 1) * width, descent + y * lineSpacing, width, height, QColor(0, 0, 0, 0x80));
                    painter.setPen(Qt::yellow);
                }

                if (mCursorVisible && pos == mCursor)
                {
                    painter.setFont(boldFont);
                    painter.fillRect((3 * x + 0.5 + leftmargin) * width, descent + y * lineSpacing, 3 * width, height, QColor(0, 0, 0, 0x80));
                    painter.fillRect((countPlaces + x - countHoriz - 1) * width, descent + y * lineSpacing, width, height, QColor(0, 0, 0, 0x80));
                    painter.setPen(Qt::white);
                }
                else if (pos >= mSelectStart && pos < mSelectStart + mSelectLength)
                {
                    painter.fillRect((3 * x + 0.5 + leftmargin) * width, descent + y * lineSpacing, 3 * width, height, QColor(0, 0, 0, 0x40));
                    painter.fillRect((countPlaces + x - countHoriz - 1) * width, descent + y * lineSpacing, width, height, QColor(0, 0, 0, 0x40));
                }

                this->drawHexByte(painter, width, (3 * x + 1 + leftmargin) * width, height + y * lineSpacing, mChunk[pos]);
                this->drawASCII(painter, (countPlaces + x - countHoriz - 1) * width, height + y * lineSpacing, mChunk[pos]);

                painter.restore();
            }

            ++pos;
        }
    }
}


int HexArea::charsPerLine(int availableWidth) const
{
    unsigned int width = mMetrics.width("0");

    unsigned int countPlaces = availableWidth / width;
    if (countPlaces >= 27)
        return (countPlaces - 11) / 4;
    else
        return (countPlaces - 2) / 4;
}

int HexArea::mouseToPos(const QPoint& mousePos) const
{
    int dx, dy, result;
    return this->mouseToPos(mousePos, dx, dy, result);
}

int HexArea::mouseToPos(const QPoint& mousePos, int& dx, int& dy, int& result) const
{
    int width = mMetrics.width("0");
    int height = mMetrics.height();
    int lineSpacing = mMetrics.lineSpacing();
    int descent = mMetrics.descent();

    // descent + y * lineSpacing
    int y = std::floor((mousePos.y() - descent) / (double)lineSpacing);

    int countVert = (this->height() + lineSpacing - height) / lineSpacing;
    int countPlaces = this->width() / width;
    int countHoriz;
    if (countPlaces >= 27)
        countHoriz = (countPlaces - 11) / 4;
    else
        countHoriz = (countPlaces - 2) / 4;

    int leftmargin = 0;
    if (countHoriz >= 4)
        leftmargin = 9;

    // (countPlaces + x - countHoriz - 1) * width
    int x = mousePos.x() / width + countHoriz + 1 - countPlaces;
    // (3 * x + 0.5 + leftmargin) * width
    if (x < 0 && mousePos.x() >= int((0.5 + leftmargin) * width))
        x = (mousePos.x() / (double)width - 0.5 - leftmargin) / 3;

    dx = dy = 0;
    if (y >= 0 && y < countVert && x >= 0 && x < countHoriz)
    {
        result = mPos + x + y * countHoriz;
        if (result < mChunk.size())
            return result;
        else
        {
            result = mChunk.size() - 1;
            return -1;
        }
    }

    if (x < 0)
    {
        dx = -1;
        x = 0;
    }
    else if (x >= countHoriz)
    {
        dx = 1;
        x = countHoriz - 1;
    }

    if (y < 0)
    {
        if (mPos > 0)
            dy = y;
        else
            x = 0;
        y = 0;
    }
    else if (y >= countVert)
    {
        if (mPos + countVert * countHoriz <= mChunk.size())
            dy = y - countVert + 1;
        else
            x = countHoriz - 1;
        y = countVert - 1;
    }

    result = mPos + x + y * countHoriz;
    if (result >= mChunk.size())
        result = mChunk.size() - 1;

    return -1;
}

}
}
