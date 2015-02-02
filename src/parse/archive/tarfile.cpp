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

#include "tarfile.hpp"

#include "misc/util.hpp"

#include <QDateTime>

namespace tyrex {
namespace parse {

TarFile::TarFile(data::Colorizer& srcColorizer, unsigned int processed) :
    mSrcColorizer(srcColorizer),
    mProcessed(processed)
{
}


void TarFile::doParse(const MemChunk& chunk, data::File& file)
{
    unsigned int size = chunk.size();

    if (size < 0x200)
        Except::reportError(mProcessed + size, "tar file header", "unexpected end of data");

    mSrcColorizer.addHighlight(mProcessed, 0x200, QColor(128, 0, 255, 64));
    mSrcColorizer.addSeparation(mProcessed, 2);
    mSrcColorizer.addSeparation(mProcessed + 100, 1);
    mSrcColorizer.addSeparation(mProcessed + 108, 1);
    mSrcColorizer.addSeparation(mProcessed + 116, 1);
    mSrcColorizer.addSeparation(mProcessed + 124, 1);
    mSrcColorizer.addSeparation(mProcessed + 136, 1);
    mSrcColorizer.addSeparation(mProcessed + 148, 1);
    mSrcColorizer.addSeparation(mProcessed + 156, 1);
    mSrcColorizer.addSeparation(mProcessed + 157, 1);
    mSrcColorizer.addSeparation(mProcessed + 257, 1);
    mSrcColorizer.addSeparation(mProcessed + 0x200, 2);

    data::FileInfo fileInfo;

    unsigned int fileNameLength = 0;
    for (unsigned int i = 0 ; i < 100 ; ++i)
    {
        if (!chunk[i])
        {
            fileNameLength = i;
            break;
        }
    }

    if (!fileNameLength)
        Except::reportError(mProcessed, "tar file header", "invalid file name");
    fileInfo.mInfos[data::FileInfo::fileName] = Util::chunkToUtf8(chunk.subChunk(0, fileNameLength));

    bool ok;
    unsigned int mode = Util::numFromOctal(chunk.subChunk(100, 8), ok);
    if (!ok)
        Except::reportError(mProcessed + 100, "tar file header", "invalid file mode");
    fileInfo.mInfos[data::FileInfo::fileMode] = QString::number(mode, 8);

    unsigned int uid = Util::numFromOctal(chunk.subChunk(108, 8), ok);
    if (!ok)
        Except::reportError(mProcessed + 108, "tar file header", "invalid user id");
    fileInfo.mInfos[data::FileInfo::fileMode] = QString::number(uid);

    unsigned int gid = Util::numFromOctal(chunk.subChunk(116, 8), ok);
    if (!ok)
        Except::reportError(mProcessed + 116, "tar file header", "invalid group id");
    fileInfo.mInfos[data::FileInfo::fileMode] = QString::number(gid);

    unsigned int fileSize = Util::numFromOctal(chunk.subChunk(124, 12), ok);
    if (!ok)
        Except::reportError(mProcessed + 124, "tar file header", "invalid file size");
    fileInfo.mInfos[data::FileInfo::size] = QString::number(fileSize);

    unsigned int mtime = Util::numFromOctal(chunk.subChunk(136, 12), ok);
    if (!ok)
        Except::reportError(mProcessed + 136, "tar file header", "invalid modification time");
    QDateTime dateTime = QDateTime(QDate(1970, 1, 1), QTime(0, 0)).addSecs(mtime);
    fileInfo.mInfos[data::FileInfo::modDateTime] = dateTime.toString("dd MMM yyyy  hh:mm:ss");

    unsigned int checksum = Util::numFromOctal(chunk.subChunk(148, 8), ok);
    if (!ok)
        Except::reportError(mProcessed + 148, "tar file header", "invalid checksum");

    unsigned int tmp = 0;
    for (unsigned int i = 0 ; i < 0x200 ; ++i)
    {
        if (i >= 148 && i < 156)
            tmp += 0x20;
        else
            tmp += chunk[i];
    }
    if (tmp != checksum)
        Except::reportError(mProcessed + 148, "tar file header", "invalid checksum");


    if (size < fileSize + 0x200)
        Except::reportError(size, "tar file", "unexpected end of data");

    file.mChunk = chunk.subChunk(0x200, fileSize);
    file.mInfo = fileInfo;


    mSrcColorizer.addHighlight(mProcessed + 0x200, fileSize, QColor(128, 128, 255, 64));
    mSrcColorizer.addSeparation(mProcessed + 0x200 + fileSize, 2);

    fileSize = (fileSize + 0x1FF) & 0xFFFFFE00;
    mProcessed += 0x200 + fileSize;
}

}
}
