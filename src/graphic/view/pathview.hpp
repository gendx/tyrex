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

#ifndef TYREX_PATHVIEW_HPP
#define TYREX_PATHVIEW_HPP

#include "view.hpp"
#include "data/font/path.hpp"
#include "tableview.hpp"
#include <QSplitter>
#include <memory>

namespace tyrex {
namespace graphic {

class PathWidget : public QWidget
{
    Q_OBJECT

public:
    PathWidget();
    PathWidget(std::shared_ptr<data::Path> path, QWidget* parent = 0);

    void setPath(std::shared_ptr<data::Path> path);

public slots:
    void selectPoint(int);

private:
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

    void drawAxes(QPainter& painter) const;
    void drawPartialAxes(QPainter& painter, const QColor& color, QPoint min, QPoint max, double space, double ratio, bool skipBig = true) const;

    QPointF transform(data::Path::Point p) const;
    QPointF transform(QPointF p) const;
    QPointF invert(QPointF p) const;

    std::shared_ptr<data::Path> mPath;
    data::Path::Point mTranslate;
    int mZoom;
    bool mMousePress;
    QPointF mMousePos;

    int mCurrentPoint;
};

class PathView : public View
{
public:
    PathView(std::shared_ptr<data::Path> path, QWidget* parent = 0);

private:
    QHBoxLayout* mLayout;
    QSplitter* mSplitter;
    PathWidget* mPathWidget;
    TableView* mTable;
};

}
}

#endif // TYREX_PATHVIEW_HPP
