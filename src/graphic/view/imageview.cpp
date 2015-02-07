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

#include "imageview.hpp"

#include <QWheelEvent>
#include <QPainter>
#include <cmath>

namespace tyrex {
namespace graphic {

ImageView::ImageView(std::shared_ptr<data::Pixmap> pixmap, QWidget* parent) :
    View(parent),
    mLayout(new QHBoxLayout(this)),
    mScrollArea(new QScrollArea),
    mImageWidget(new ImageWidget(pixmap))
{
    mLayout->setContentsMargins(QMargins());
    mLayout->addWidget(mScrollArea);

    mScrollArea->setBackgroundRole(QPalette::Dark);
    mScrollArea->setAlignment(Qt::AlignCenter);
    mScrollArea->setWidget(mImageWidget);
}


void ImageView::wheelEvent(QWheelEvent* event)
{
    int offset = event->delta() / 120;
    mImageWidget->setZoom(offset);
    event->accept();
}


ImageWidget::ImageWidget(std::shared_ptr<data::Pixmap> pixmap, QWidget* parent) :
    QWidget(parent),
    mPixmap(pixmap),
    mQImage(mPixmap->width(), mPixmap->height(), QImage::Format_ARGB32),
    mZoom(0)
{
    for (unsigned int y = 0 ; y < mPixmap->height() ; ++y)
        for (unsigned int x = 0 ; x < mPixmap->width() ; ++x)
            mQImage.setPixel(x, y, mPixmap->getARGB32(x, y));

    this->resize(mPixmap->width(), mPixmap->height());
}


void ImageWidget::wheelEvent(QWheelEvent* event)
{
    int offset = event->delta() / 120;
    this->setZoom(offset);
    event->accept();
}

void ImageWidget::setZoom(int zoom)
{
    mZoom += zoom;
    double realFactor = std::pow(2, mZoom / 4.0);
    this->resize(mPixmap->width() * realFactor, mPixmap->height() * realFactor);
}


void ImageWidget::resizeEvent(QResizeEvent* event)
{
    this->update();
}

void ImageWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, this->width(), this->height(), Qt::white);

    double xratio = this->width() / (double)mQImage.width();
    double yratio = this->height() / (double)mQImage.height();
    double ratio = xratio < yratio ? xratio : yratio;

    unsigned int w = mQImage.width() * ratio;
    unsigned int h = mQImage.height() * ratio;
    unsigned int x = (this->width() - w) / 2;
    unsigned int y = (this->height() - h) / 2;

    painter.drawImage(QRect(x, y, w, h), mQImage);
}

}
}
