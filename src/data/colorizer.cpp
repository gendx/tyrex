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

#include "colorizer.hpp"

namespace tyrex {
namespace data {

AbstractHighlighter::~AbstractHighlighter()
{
}

AbstractSeparater::~AbstractSeparater()
{
}


void Colorizer::addHighlight(unsigned int start, unsigned int size, QColor color)
{
    mHighlighter->addHighlight(start, size, color);
}

void Colorizer::addSeparation(unsigned int pos, unsigned int size)
{
    mSeparater->addSeparation(pos, size);
}

void Colorizer::colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const
{
    mHighlighter->colorize(painter, pos, width, height, lineSpacing, descent, countPlaces, countHoriz, countVert, leftmargin);
    mSeparater->colorize(painter, pos, width, height, lineSpacing, descent, countPlaces, countHoriz, countVert, leftmargin);
}


void Highlighter::addHighlight(unsigned int start, unsigned int size, QColor color)
{
    if (size)
        mHighlights[start] = Highlight(start, size, color);
}

void Separater::addSeparation(unsigned int pos, unsigned int size)
{
    mSeparations[pos] = Separation(pos, size);
}


void Highlighter::colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const
{
    if (mHighlights.isEmpty())
        return;

    for (unsigned int y = 0 ; y < countVert ; ++y)
    {
        for (unsigned int x = 0 ; x < countHoriz ; ++x)
        {
            QMap<unsigned int, Highlighter::Highlight>::const_iterator iter = mHighlights.upperBound(pos);
            --iter;

            QColor color = Qt::white;
            if (iter != mHighlights.end())
                if (pos >= iter.value().start && pos < iter.value().start + iter.value().size)
                    color = iter.value().color;

            if (color != Qt::white)
            {
                painter.fillRect((3 * x + 0.5 + leftmargin) * width, y * lineSpacing + descent, 3 * width, height, color);
                painter.fillRect((countPlaces + x - countHoriz - 1) * width, y * lineSpacing + descent, width, height, color);
            }

            ++pos;
        }
    }
}

void Separater::colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int/* height*/, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const
{
    if (mSeparations.isEmpty())
        return;

    for (unsigned int y = 0 ; y < countVert ; ++y)
    {
        for (unsigned int x = 0 ; x < countHoriz ; ++x)
        {
            QMap<unsigned int, Separater::Separation>::const_iterator iter = mSeparations.find(pos);
            if (iter != mSeparations.end())
            {
                painter.save();

                QPen pen(Qt::black, iter.value().size);
                painter.setPen(pen);

                painter.drawLine((0.5 + leftmargin) * width, (y + 1) * lineSpacing + descent,
                                 (3 * x + 0.5 + leftmargin) * width, (y + 1) * lineSpacing + descent);
                painter.drawLine((3 * x + 0.5 + leftmargin) * width, (y + 1) * lineSpacing + descent,
                                 (3 * x + 0.5 + leftmargin) * width, y * lineSpacing + descent);
                painter.drawLine((3 * x + 0.5 + leftmargin) * width, y * lineSpacing + descent,
                                 (3 * countHoriz + 0.5 + leftmargin) * width, y * lineSpacing + descent);

                if (iter.value().size)
                {
                    painter.drawLine((countPlaces - countHoriz - 1) * width, (y + 1) * lineSpacing + descent,
                                     (countPlaces + x - countHoriz - 1) * width, (y + 1) * lineSpacing + descent);
                    painter.drawLine((countPlaces + x - countHoriz - 1) * width, (y + 1) * lineSpacing + descent,
                                     (countPlaces + x - countHoriz - 1) * width, y * lineSpacing + descent);
                    painter.drawLine((countPlaces + x - countHoriz - 1) * width, y * lineSpacing + descent,
                                     (countPlaces - 1) * width, y * lineSpacing + descent);
                }

                painter.restore();
            }

            ++pos;
        }
    }
}


void ArrayHighlighter::addHighlight(unsigned int start, unsigned int size, QColor color)
{
    unsigned int end = start + size - 1;
    if (end >= mHighlights.size())
        mHighlights.resize(1 + (end << 2), 0);

    unsigned int val =
                    4 - ((color.blue() + 32) / 64)
            + 5  * (4 - (color.green() + 32) / 64)
            + 25 * (4 - (color.red() + 32) / 64);

    for (unsigned int i = start ; i <= end ; ++i)
        mHighlights[i] = val;
}

void ArraySeparater::addSeparation(unsigned int pos, unsigned int/* size*/)
{
    if (pos >= (mSeparations.size() << 3))
        mSeparations.resize(1 + (pos >> 3), 0);

    setAt1(mSeparations, pos, 1);
}


void ArrayHighlighter::colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const
{
    for (unsigned int y = 0 ; y < countVert ; ++y)
    {
        for (unsigned int x = 0 ; x < countHoriz ; ++x)
        {
            if (pos >= (mHighlights.size() << 2))
                return;

            unsigned int val = mHighlights[pos];

            QColor color(255, 255, 255, 64);
            switch (val % 5)
            {
            case 0:
                color.setBlue(0xFF); break;
            case 1:
                color.setBlue(0xC0); break;
            case 2:
                color.setBlue(0x80); break;
            case 3:
                color.setBlue(0x40); break;
            case 4:
                color.setBlue(0); break;
            }
            switch ((val / 5) % 5)
            {
            case 0:
                color.setGreen(0xFF); break;
            case 1:
                color.setGreen(0xC0); break;
            case 2:
                color.setGreen(0x80); break;
            case 3:
                color.setGreen(0x40); break;
            case 4:
                color.setGreen(0); break;
            }
            switch ((val / 25) % 5)
            {
            case 0:
                color.setRed(0xFF); break;
            case 1:
                color.setRed(0xC0); break;
            case 2:
                color.setRed(0x80); break;
            case 3:
                color.setRed(0x40); break;
            case 4:
                color.setRed(0); break;
            }

            if (val)
            {
                painter.fillRect((3 * x + 0.5 + leftmargin) * width, y * lineSpacing + descent, 3 * width, height, color);
                painter.fillRect((countPlaces + x - countHoriz - 1) * width, y * lineSpacing + descent, width, height, color);
            }

            ++pos;
        }
    }
}

void ArraySeparater::colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int/* height*/, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const
{
    for (unsigned int y = 0 ; y < countVert ; ++y)
    {
        for (unsigned int x = 0 ; x < countHoriz ; ++x)
        {
            if (pos >= (mSeparations.size() << 3))
                return;

            if (getAt1(mSeparations, pos))
            {
                painter.save();

                QPen pen(Qt::black, 1);
                painter.setPen(pen);

                painter.drawLine((0.5 + leftmargin) * width, (y + 1) * lineSpacing + descent,
                                 (3 * x + 0.5 + leftmargin) * width, (y + 1) * lineSpacing + descent);
                painter.drawLine((3 * x + 0.5 + leftmargin) * width, (y + 1) * lineSpacing + descent,
                                 (3 * x + 0.5 + leftmargin) * width, y * lineSpacing + descent);
                painter.drawLine((3 * x + 0.5 + leftmargin) * width, y * lineSpacing + descent,
                                 (3 * countHoriz + 0.5 + leftmargin) * width, y * lineSpacing + descent);

                if (mBothSides)
                {
                    painter.drawLine((countPlaces - countHoriz - 1) * width, (y + 1) * lineSpacing + descent,
                                     (countPlaces + x - countHoriz - 1) * width, (y + 1) * lineSpacing + descent);
                    painter.drawLine((countPlaces + x - countHoriz - 1) * width, (y + 1) * lineSpacing + descent,
                                     (countPlaces + x - countHoriz - 1) * width, y * lineSpacing + descent);
                    painter.drawLine((countPlaces + x - countHoriz - 1) * width, y * lineSpacing + descent,
                                     (countPlaces - 1) * width, y * lineSpacing + descent);
                }

                painter.restore();
            }

            ++pos;
        }
    }
}


unsigned int ArraySeparater::getAt1(const std::vector<unsigned char>& vect, unsigned int pos)
{
    return (vect[pos >> 3] >> (pos & 0x07)) & 0x01;
}

void ArraySeparater::setAt1(std::vector<unsigned char>& vect, unsigned int pos, unsigned int val)
{
    unsigned int offset = pos & 0x07;
    unsigned char& ref = vect[pos >> 3];
    ref ^= (ref & (0x01 << offset)) ^ (val << offset);
}

}
}
