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

#ifndef TYREX_COLORIZER_HPP
#define TYREX_COLORIZER_HPP

#include <QMap>
#include <QColor>
#include <QPainter>
#include "misc/shared.hpp"

namespace tyrex {
namespace data {

class AbstractHighlighter
{
public:
    virtual ~AbstractHighlighter();

    virtual void addHighlight(unsigned int start, unsigned int size, QColor color) = 0;

    virtual void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const = 0;
};


class Highlighter : public AbstractHighlighter
{
public:
    void addHighlight(unsigned int start, unsigned int size, QColor color);

    void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const;

private:
    struct Highlight
    {
        inline Highlight();
        inline Highlight(unsigned int _start, unsigned int _size, QColor _color);

        unsigned int start;
        unsigned int size;
        QColor color;
    };

    QMap<unsigned int, Highlight> mHighlights;
};

inline Highlighter::Highlight::Highlight() :
    start(0), size(0), color(Qt::white) {}
inline Highlighter::Highlight::Highlight(unsigned int _start, unsigned int _size, QColor _color) :
    start(_start), size(_size), color(_color) {}


class ArrayHighlighter : public AbstractHighlighter
{
public:
    void addHighlight(unsigned int start, unsigned int size, QColor color);

    void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const;

private:
    std::vector<unsigned char> mHighlights;
};


class AbstractSeparater
{
public:
    virtual ~AbstractSeparater();

    virtual void addSeparation(unsigned int pos, unsigned int size) = 0;

    virtual void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const = 0;
};


class Separater : public AbstractSeparater
{
public:
    void addSeparation(unsigned int pos, unsigned int size);

    void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const;

private:
    struct Separation
    {
        inline Separation();
        inline Separation(unsigned int _pos, unsigned int _size);

        unsigned int pos;
        unsigned int size;
    };

    QMap<unsigned int, Separation> mSeparations;
};

inline Separater::Separation::Separation() :
    pos(0), size(0) {}
inline Separater::Separation::Separation(unsigned int _pos, unsigned int _size) :
    pos(_pos), size(_size) {}


class ArraySeparater : public AbstractSeparater
{
public:
    inline ArraySeparater(bool bothSides = true);
    void addSeparation(unsigned int pos, unsigned int size);

    void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const;

private:
    static unsigned int getAt1(const std::vector<unsigned char>& vect, unsigned int pos);
    static void setAt1(std::vector<unsigned char>& vect, unsigned int pos, unsigned int val);

    std::vector<unsigned char> mSeparations;
    bool mBothSides;
};

inline ArraySeparater::ArraySeparater(bool bothSides) :
    mBothSides(bothSides) {}


class Colorizer
{
public:
    inline Colorizer();
    inline Colorizer(Shared<AbstractHighlighter> highlighter, Shared<AbstractSeparater> separater);

    void addHighlight(unsigned int start, unsigned int size, QColor color);
    void addSeparation(unsigned int pos, unsigned int size);

    void colorize(QPainter& painter, unsigned int pos, unsigned int width, unsigned int height, unsigned int lineSpacing, unsigned int descent, unsigned int countPlaces, unsigned int countHoriz, unsigned int countVert, unsigned int leftmargin) const;

private:
    Shared<AbstractHighlighter> mHighlighter;
    Shared<AbstractSeparater> mSeparater;
};

inline Colorizer::Colorizer() :
    mHighlighter(makeShared<AbstractHighlighter, Highlighter>()), mSeparater(makeShared<AbstractSeparater, Separater>()) {}
inline Colorizer::Colorizer(Shared<AbstractHighlighter> highlighter, Shared<AbstractSeparater> separater) :
    mHighlighter(highlighter), mSeparater(separater) {}

}
}

#endif // TYREX_COLORIZER_HPP
