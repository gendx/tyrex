#   Tyrex - the versatile file decoder.
#   Copyright (C) 2014  G. Endignoux
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see http://www.gnu.org/licenses/gpl-3.0.txt


TEMPLATE = app
DESTDIR = ../bin

INCLUDEPATH += .

HEADERS += \
    data/archive.hpp \
    data/bytesequence.hpp \
    data/colorizer.hpp \
    data/compress.hpp \
    data/data.hpp \
    data/elf.hpp \
    data/file.hpp \
    data/fileinfo.hpp \
    data/font/font.hpp \
    data/font/path.hpp \
    data/image.hpp \
    data/image/color.hpp \
    data/image/pixmap.hpp \
    data/javaclass.hpp \
    data/table.hpp \
    external/elf.h \
    graphic/actionset.hpp \
    graphic/area/area.hpp \
    graphic/area/hexarea.hpp \
    graphic/console.hpp \
    graphic/dialog/consoledialog.hpp \
    graphic/dialog/hexfinddialog.hpp \
    graphic/dialog/inputdialog.hpp \
    graphic/dialog/typeselector.hpp \
    graphic/document.hpp \
    graphic/mainwindow.hpp \
    graphic/sidetree.hpp \
    graphic/treemodel.hpp \
    graphic/util/listwidget.hpp \
    graphic/util/treewidget.hpp \
    graphic/view/archiveview.hpp \
    graphic/view/fontview.hpp \
    graphic/view/hexview.hpp \
    graphic/view/imageview.hpp \
    graphic/view/pathview.hpp \
    graphic/view/scrollview.hpp \
    graphic/view/tableview.hpp \
    graphic/view/view.hpp \
    misc/chunk.hpp \
    misc/chunk.tpl \
    misc/hash/hash.hpp \
    misc/hash/sha256.hpp \
    misc/memchunk.hpp \
    misc/shared.hpp \
    misc/tree.hpp \
    misc/tree.tpl \
    misc/util.hpp \
    parse/archive/tar.hpp \
    parse/archive/tarfile.hpp \
    parse/archive/zip.hpp \
    parse/archive/zipfile.hpp \
    parse/compress/bitstream.hpp \
    parse/compress/bzip2.hpp \
    parse/compress/deflate/deflate.hpp \
    parse/compress/deflate/deflatestream.hpp \
    parse/compress/deflate/gzip.hpp \
    parse/compress/deflate/zlib.hpp \
    parse/compress/forwardstream.hpp \
    parse/compress/huffmantree.hpp \
    parse/compress/lz.hpp \
    parse/compress/lzma/lzma.hpp \
    parse/compress/lzma/lzmabittree.hpp \
    parse/compress/lzma/lzmadecoder.hpp \
    parse/compress/lzma/lzmalendecoder.hpp \
    parse/compress/lzma/lzmastream.hpp \
    parse/compress/lzma/lzma2.hpp \
    parse/compress/lzw.hpp \
    parse/compress/movetofront.hpp \
    parse/compress/parsecompress.hpp \
    parse/font/truetype.hpp \
    parse/image/png.hpp \
    parse/parsedocument.hpp \
    parse/parseexception.hpp \
    parse/parser.hpp \
    parse/parser.tpl \
    parse/program/elf32.hpp \
    parse/program/elf32header.hpp \
    parse/program/parsejavaclass.hpp \
    platform-specific/platform-specific.hpp \

SOURCES += \
    data/archive.cpp \
    data/bytesequence.cpp \
    data/colorizer.cpp \
    data/compress.cpp \
    data/data.cpp \
    data/elf.cpp \
    data/file.cpp \
    data/fileinfo.cpp \
    data/font/font.cpp \
    data/font/path.cpp \
    data/image.cpp \
    data/image/pixmap.cpp \
    data/javaclass.cpp \
    data/table.cpp \
    graphic/area/area.cpp \
    graphic/area/hexarea.cpp \
    graphic/console.cpp \
    graphic/dialog/consoledialog.cpp \
    graphic/dialog/hexfinddialog.cpp \
    graphic/dialog/inputdialog.cpp \
    graphic/dialog/typeselector.cpp \
    graphic/document.cpp \
    graphic/mainwindow.cpp \
    graphic/sidetree.cpp \
    graphic/treemodel.cpp \
    graphic/util/listwidget.cpp \
    graphic/util/treewidget.cpp \
    graphic/view/archiveview.cpp \
    graphic/view/fontview.cpp \
    graphic/view/hexview.cpp \
    graphic/view/imageview.cpp \
    graphic/view/pathview.cpp \
    graphic/view/scrollview.cpp \
    graphic/view/tableview.cpp \
    graphic/view/view.cpp \
    misc/hash/hash.cpp \
    misc/hash/sha256.cpp \
    misc/memchunk.cpp \
    misc/util.cpp \
    main.cpp \
    parse/archive/tar.cpp \
    parse/archive/tarfile.cpp \
    parse/archive/zip.cpp \
    parse/archive/zipfile.cpp \
    parse/compress/bzip2.cpp \
    parse/compress/deflate/deflate.cpp \
    parse/compress/deflate/deflatestream.cpp \
    parse/compress/deflate/gzip.cpp \
    parse/compress/deflate/zlib.cpp \
    parse/compress/forwardstream.cpp \
    parse/compress/huffmantree.cpp \
    parse/compress/lz.cpp \
    parse/compress/lzma/lzma.cpp \
    parse/compress/lzma/lzmabittree.cpp \
    parse/compress/lzma/lzmadecoder.cpp \
    parse/compress/lzma/lzmalendecoder.cpp \
    parse/compress/lzma/lzmastream.cpp \
    parse/compress/lzma/lzma2.cpp \
    parse/compress/lzw.cpp \
    parse/compress/movetofront.cpp \
    parse/compress/parsecompress.cpp \
    parse/font/truetype.cpp \
    parse/image/png.cpp \
    parse/parsedocument.cpp \
    parse/parseexception.cpp \
    parse/program/elf32.cpp \
    parse/program/elf32header.cpp \
    parse/program/parsejavaclass.cpp \

unix:{
SOURCES += platform-specific/linux/platform-specific.cpp
}

win32:{
SOURCES += platform-specific/windows/platform-specific.cpp
}

RESOURCES += \
    ressources.qrc

QMAKE_CXXFLAGS += --std=c++11
