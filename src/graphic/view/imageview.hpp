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

#ifndef TYREX_IMAGEVIEW_HPP
#define TYREX_IMAGEVIEW_HPP

#include "view.hpp"
#include "data/image/pixmap.hpp"
#include <QImage>
#include <QHBoxLayout>
#include <QScrollArea>
#include <memory>

namespace tyrex {
namespace graphic {

class ImageWidget : public QWidget
{
public:
    ImageWidget(std::shared_ptr<data::Pixmap> pixmap, QWidget* parent = 0);

    void setZoom(int zoom);

private:
    void wheelEvent(QWheelEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

    std::shared_ptr<data::Pixmap> mPixmap;
    QImage mQImage;
    int mZoom;
};

class ImageView : public View
{
public:
    ImageView(std::shared_ptr<data::Pixmap> pixmap, QWidget* parent = 0);

private:
    void wheelEvent(QWheelEvent* event);

    QHBoxLayout* mLayout;
    QScrollArea* mScrollArea;
    ImageWidget* mImageWidget;
};

}
}

#endif // TYREX_IMAGEVIEW_HPP
