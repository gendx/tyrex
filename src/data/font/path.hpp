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

#ifndef TYREX_DATA_PATH_HPP
#define TYREX_DATA_PATH_HPP

#include <vector>

namespace tyrex {
namespace data {

class Path
{
public:
    struct Point
    {
        inline Point(double _x, double _y) :
            x(_x), y(_y) {}

        inline static Point mid(const Point& p1, const Point& p2)
            {return Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);}

        double x;
        double y;
    };

    void moveTo(const Point& point);
    void lineTo(const Point& point);
    void quadTo(const Point& control, const Point& point);
    void cubicTo(const Point& control1, const Point& control2, const Point& point);

    struct PointInfo
    {
        inline PointInfo(const Point& _p, unsigned char _flags) :
            p(_p), flags(_flags) {}

        Point p;
        unsigned char flags;
    };

    inline const std::vector<PointInfo>& points() const;

private:
    std::vector<PointInfo> mPoints;
};

inline const std::vector<Path::PointInfo>& Path::points() const
    {return mPoints;}

}
}

#endif // TYREX_DATA_PATH_HPP
