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

#include "pathview.hpp"

#include <QWheelEvent>
#include <QPainter>
#include <cmath>

namespace tyrex {
namespace graphic {

PathView::PathView(std::shared_ptr<data::Path> path, QWidget* parent) :
    View(parent),
    mLayout(new QHBoxLayout(this)),
    mSplitter(new QSplitter),
    mPathWidget(new PathWidget(path))
{
    mLayout->setMargin(0);
    mLayout->addWidget(mSplitter);

    mSplitter->addWidget(mPathWidget);

    QList<QStringList> content;
    for (const data::Path::PointInfo& pi : path->points())
    {
        QString type;
        switch (pi.flags)
        {
        case 0:
            type = "point";
            break;
        case 1:
            type = "move";
            break;
        case 2:
            type = "ctrl";
        }

        content.append(QStringList() << QString::number(pi.p.x) << QString::number(pi.p.y) << type);
    }

    mTable = new TableView(QStringList() << "x" << "y" << "type", content);
    mSplitter->addWidget(mTable);
}


PathWidget::PathWidget() :
    mPath(std::make_shared<data::Path>()),
    mTranslate(0, 0),
    mZoom(0),
    mMousePress(false),
    mCurrentPoint(-1)
{
}

PathWidget::PathWidget(std::shared_ptr<data::Path> path, QWidget* parent) :
    QWidget(parent),
    mPath(path),
    mTranslate(0, 0),
    mZoom(0),
    mMousePress(false),
    mCurrentPoint(-1)
{
}


void PathWidget::setPath(std::shared_ptr<data::Path> path)
{
    mPath = path;
    this->update();
}

void PathWidget::selectPoint(int i)
{
    mCurrentPoint = i;
    this->update();
}


QPointF PathWidget::transform(data::Path::Point p) const
{
    double scaleFactor = std::pow(2, mZoom / 4.0);
    return QPointF((p.x + mTranslate.x) * scaleFactor + this->width() / 2, (-p.y + mTranslate.y) * scaleFactor + this->height() / 2);
}

QPointF PathWidget::transform(QPointF p) const
{
    double scaleFactor = std::pow(2, mZoom / 4.0);
    return QPointF((p.x() + mTranslate.x) * scaleFactor + this->width() / 2, (-p.y() + mTranslate.y) * scaleFactor + this->height() / 2);
}

QPointF PathWidget::invert(QPointF p) const
{
    double scaleFactor = std::pow(2, mZoom / 4.0);
    return QPointF((p.x() - this->width() / 2) / scaleFactor - mTranslate.x, (p.y() - this->height() / 2) / scaleFactor + mTranslate.y);
}


void PathWidget::wheelEvent(QWheelEvent* event)
{
    mZoom += event->delta() / 120;
    event->accept();
    this->update();
}

void PathWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mMousePress)
    {
        QPointF diff = event->pos() - mMousePos;

        double scaleFactor = std::pow(2, mZoom / 4.0);
        mTranslate.x += diff.x() / scaleFactor;
        mTranslate.y += diff.y() / scaleFactor;

        update();
    }

    mMousePos = event->pos();
    event->accept();
}

void PathWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    mMousePress = true;
    mMousePos = event->pos();
    this->setCursor(Qt::ClosedHandCursor);
    event->accept();
}

void PathWidget::mouseReleaseEvent(QMouseEvent* event)
{
    mMousePress = false;
    this->setCursor(Qt::OpenHandCursor);
    event->accept();
}

void PathWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), Qt::white);
    this->drawAxes(painter);

    QPainterPath painterPath;
    QList<QPointF> ctrl;
    for (const data::Path::PointInfo& pi : mPath->points())
    {
        QPointF p = this->transform(pi.p);

        switch (pi.flags)
        {
        case 0:
            if (ctrl.size() == 0)
                painterPath.lineTo(p);
            else if (ctrl.size() == 1)
                painterPath.quadTo(ctrl[0], p);
            else if (ctrl.size() == 2)
                painterPath.cubicTo(ctrl[0], ctrl[1], p);
            ctrl.clear();
            break;
        case 1:
            painterPath.moveTo(p);
            break;
            // Control point.
        case 2:
            ctrl.append(p);
        }
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(painterPath, QBrush(QColor(0, 0, 0, 64)));
    painter.strokePath(painterPath, QPen(Qt::black, 2));

    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(Qt::black, 5);

    const std::vector<data::Path::PointInfo>& points = mPath->points();
    for (int i = 0 ; i < points.size() ; ++i)
    {
        const data::Path::PointInfo& pi = points[i];
        QPointF p = this->transform(pi.p);

        switch (pi.flags)
        {
        case 0:
            pen.setColor(Qt::blue);
            break;
        case 1:
            pen.setColor(Qt::green);
            break;
            // Control point.
        case 2:
            pen.setColor(Qt::red);
        }

        if (i == mCurrentPoint)
            pen.setWidth(10);
        else
            pen.setWidth(5);

        painter.setPen(pen);
        painter.drawPoint(p);
    }
}


void PathWidget::drawAxes(QPainter& painter) const
{
    QPointF leftTop = this->invert(QPointF(0, 0));
    QPointF rightBottom = this->invert(QPointF(this->width(), this->height()));

    double log10Zoom = std::log10(2) * mZoom / 4.0;
    int logZoom = std::floor(1.7 - log10Zoom);
    // remain >= 0  (when log10Zoom = n + 0.7)
    // remain <  1  (when log10Zoom = n + 0.7 + epsilon)
    double remain = (1.7 - log10Zoom) - logZoom;

    // 0 < grey <= 0x80
    // 0x80 < black <= 0x100
    int _grey = 0x80 * (1 - remain);
    int _black = 0x80 + _grey;
    if (_black > 0xFF)
        _black = 0xFF;
    QColor grey(0x00, 0x00, 0x00, _grey);
    QColor black(0x00, 0x00, 0x00, _black);

    double space = std::pow(10, logZoom);
    QPoint min(std::floor(leftTop.x() / space), std::floor(leftTop.y() / space));
    QPoint max(std::ceil(rightBottom.x() / space), std::ceil(rightBottom.y() / space));

    this->drawPartialAxes(painter, grey, min, max, space, 1);
    this->drawPartialAxes(painter, black, min, max, space, 10.0);
    this->drawPartialAxes(painter, Qt::black, min, max, space, 100.0, false);
}

void PathWidget::drawPartialAxes(QPainter& painter, const QColor& color, QPoint min, QPoint max, double space, double ratio, bool skipBig) const
{
    painter.setPen(color);
    for (int i = std::floor(min.x() / ratio) ; i <= std::ceil(max.x() / ratio) ; ++i)
    {
        if (skipBig && i % 10 == 0)
            continue;
        painter.drawLine(QLineF(this->transform(QPointF(ratio * i * space, min.y() * space)),
                                this->transform(QPointF(ratio * i * space, max.y() * space))));
    }
    for (int i = std::floor(min.y() / ratio) ; i <= std::ceil(max.y() / ratio) ; ++i)
    {
        if (skipBig && i % 10 == 0)
            continue;
        painter.drawLine(QLineF(this->transform(QPointF(min.x() * space, ratio * i * space)),
                                this->transform(QPointF(max.x() * space, ratio * i * space))));
    }
}

}
}
