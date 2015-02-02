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

#ifndef TYREX_HEXAREA_HPP
#define TYREX_HEXAREA_HPP

#include "area.hpp"
#include "data/bytesequence.hpp"
#include <QBasicTimer>
#include <QPoint>

namespace tyrex {
namespace graphic {

class HexArea : public TextArea
{
    Q_OBJECT

public:
    inline explicit HexArea(const MemChunk& chunk, const data::Colorizer& colorizer, ScrollView* parent);
    ~HexArea();

    inline MemChunk memChunk() const;
    inline MemChunk currentSelection() const;

    inline void setSearching(bool searching);
    unsigned int setSearch(MemChunk search);
    void clearSearch();
    void previousSearch();
    void nextSearch();

    void setCursor(unsigned int pos, bool checkScroll = true);
    void setSelection(unsigned int pos, unsigned int length = 0, bool cursor = true);
    void setSelectAndCursor(unsigned int pos, unsigned int length, unsigned int cursor);
    void extendSelection(unsigned int pos, bool extend = false);
    void clearSelection();

private slots:
    void scroll(int value);

private:
    void setTimer();
    void clearTimer();
    void selectionForMouse(const QPoint& mousePos, bool checkTimer = true);

    void showEvent(QShowEvent* event);
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void timerEvent(QTimerEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

    inline int charsPerLine() const;
    int charsPerLine(int availableWidth) const;
    int mouseToPos(const QPoint& mousePos) const;
    int mouseToPos(const QPoint& mousePos, int& dx, int& dy, int& result) const;

    MemChunk mChunk;
    data::Colorizer mColorizer;
    int mPos;

    bool mSearching;
    unsigned int mSearchLength;
    QList<unsigned int> mFound;
    unsigned int mCursor;
    unsigned int mSelectStart;
    unsigned int mSelectLength;
    bool mShift;

    QBasicTimer mCursorTimer;
    bool mCursorVisible;
    QBasicTimer mScrollTimer;
    int mPosClick;
    QPoint mLastPos;
};

inline HexArea::HexArea(const MemChunk& chunk, const data::Colorizer& colorizer, ScrollView* parent) :
    TextArea(parent), mChunk(chunk), mColorizer(colorizer), mPos(0), mSearching(false), mCursor(0), mSelectStart(0), mSelectLength(0), mShift(false), mCursorVisible(false), mPosClick(-1) {}

inline MemChunk HexArea::memChunk() const
    {return mChunk;}
inline MemChunk HexArea::currentSelection() const
    {return mChunk.subChunk(mSelectStart, mSelectLength);}

inline void HexArea::setSearching(bool searching)
    {mSearching = searching;}

inline int HexArea::charsPerLine() const
    {return this->charsPerLine(this->width());}

}
}

#endif // TYREX_HEXAREA_HPP
