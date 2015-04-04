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

#include "zipfile.hpp"

#include "parse/compress/deflate/deflate.hpp"
#include "misc/util.hpp"

#include <QDateTime>

namespace tyrex {
namespace parse {

ZipFile::ZipFile(data::Colorizer& srcColorizer, const MemChunk& centralDir, unsigned int centralDirStart, unsigned int centralDirSize) :
    mSrcColorizer(srcColorizer),
    mCentralDir(centralDir),
    mCentralDirStart(centralDirStart),
    mCentralDirSize(centralDirSize),
    mProcessed(0)
{
}


void ZipFile::doParse(const MemChunk& chunk, data::File& file)
{
    unsigned int size = chunk.size();

    if (mCentralDirSize < 46)
        Except::reportError(mCentralDirStart + mCentralDirSize, "zip central directory", "unexpected end of central directory");
    if (mCentralDir.getUint32LE(0) != 0x02014B50)
        Except::reportError(mCentralDirStart, "zip central directory header", "invalid signature");

    mSrcColorizer.addSeparation(mCentralDirStart, 2);
    mSrcColorizer.addHighlight(mCentralDirStart, 46, QColor(128, 0, 255, 64));
    mSrcColorizer.addSeparation(mCentralDirStart + 4, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 6, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 8, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 10, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 12, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 14, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 16, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 20, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 24, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 28, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 30, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 32, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 34, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 36, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 38, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 42, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 46, 1);

    unsigned int versionMadeBy = mCentralDir.getUint16LE(4);
    unsigned int versionNeeded = mCentralDir.getUint16LE(6);
    unsigned int bitFlag = mCentralDir.getUint16LE(8);
    unsigned int compressionMethod = mCentralDir.getUint16LE(10);
    unsigned int modTime = mCentralDir.getUint16LE(12);
    unsigned int modDate = mCentralDir.getUint16LE(14);
    unsigned int crc32 = mCentralDir.getUint32LE(16);
    unsigned int compressedSize = mCentralDir.getUint32LE(20);
    unsigned int uncompressedSize = mCentralDir.getUint32LE(24);
    unsigned int fileNameLength = mCentralDir.getUint16LE(28);
    unsigned int extraFieldLength = mCentralDir.getUint16LE(30);
    unsigned int fileCommentLength = mCentralDir.getUint16LE(32);
    unsigned int diskNumber = mCentralDir.getUint16LE(34);
    unsigned int internalAttributes = mCentralDir.getUint16LE(36);
    unsigned int externalAttributes = mCentralDir.getUint32LE(38);
    unsigned int localHeaderOffset = mCentralDir.getUint32LE(42);

    unsigned int extraLength = fileNameLength + extraFieldLength + fileCommentLength;

    if (!Util::checkRanges(46, {fileNameLength, extraFieldLength, fileCommentLength}, mCentralDirSize))
        Except::reportError(mCentralDirStart + mCentralDirSize, "zip central directory", "unexpected end of central directory");

    mSrcColorizer.addHighlight(mCentralDirStart + 46, extraLength, QColor(128, 255, 0, 64));
    mSrcColorizer.addSeparation(mCentralDirStart + 46 + fileNameLength, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 46 + fileNameLength + extraFieldLength, 1);
    mSrcColorizer.addSeparation(mCentralDirStart + 46 + extraLength, 2);

    mProcessed = 46 + extraLength;

    // check local header
    if (!Util::checkRange(localHeaderOffset, 30, size))
        Except::reportError(size, "zip local file header", "offset is out of range");
    if (chunk.getUint32LE(localHeaderOffset) != 0x04034B50)
        Except::reportError(localHeaderOffset, "zip local file header", "invalid signature");

    mSrcColorizer.addSeparation(localHeaderOffset, 2);
    mSrcColorizer.addHighlight(localHeaderOffset, 30, QColor(128, 0, 255, 64));
    mSrcColorizer.addSeparation(localHeaderOffset + 4, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 6, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 8, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 10, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 12, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 14, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 18, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 22, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 26, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 28, 1);
    mSrcColorizer.addSeparation(localHeaderOffset + 30, 1);

    if (chunk.getUint16LE(localHeaderOffset + 4) != versionNeeded)
        Except::reportError(localHeaderOffset + 4, "zip local file header", "version needed does not match central directory");
    if (chunk.getUint16LE(localHeaderOffset + 6) != bitFlag)
        Except::reportError(localHeaderOffset + 6, "zip local file header", "flag does not match central directory");
    if (chunk.getUint16LE(localHeaderOffset + 8) != compressionMethod)
        Except::reportError(localHeaderOffset + 8, "zip local file header", "compression method does not match central directory");
    if (chunk.getUint16LE(localHeaderOffset + 10) != modTime)
        Except::reportError(localHeaderOffset + 10, "zip local file header", "modification time does not match central directory");
    if (chunk.getUint16LE(localHeaderOffset + 12) != modDate)
        Except::reportError(localHeaderOffset + 12, "zip local file header", "modification date does not match central directory");

    if (bitFlag & 0x08)
    {
        for (unsigned int i = 14 ; i < 26 ; ++i)
            if (chunk[localHeaderOffset + i] != 0)
                Except::reportError(localHeaderOffset + i, "zip local file header", "value must be zero with such flags");
    }
    else
    {
        if (chunk.getUint32LE(localHeaderOffset + 14) != crc32)
            Except::reportError(localHeaderOffset + 14, "zip local file header", "crc32 does not match central directory");
        if (chunk.getUint32LE(localHeaderOffset + 18) != compressedSize)
            Except::reportError(localHeaderOffset + 18, "zip local file header", "compressed size does not match central directory");
        if (chunk.getUint32LE(localHeaderOffset + 22) != uncompressedSize)
            Except::reportError(localHeaderOffset + 22, "zip local file header", "uncompressed size does not match central directory");
    }

    if (chunk.getUint16LE(localHeaderOffset + 26) != fileNameLength)
        Except::reportError(localHeaderOffset + 26, "zip local file header", "file name length does not match central directory");
    unsigned int localExtraFieldLength = chunk.getUint16LE(localHeaderOffset + 28);

    localHeaderOffset += 30;

    extraLength = fileNameLength + localExtraFieldLength;
    if (!Util::checkRanges(localHeaderOffset, {fileNameLength, localExtraFieldLength}, size))
        Except::reportError(size, "zip local file header", "unexpected end of central directory");

    mSrcColorizer.addHighlight(localHeaderOffset, fileNameLength, QColor(255, 0, 0, 64));
    mSrcColorizer.addSeparation(localHeaderOffset + fileNameLength, 1);
    mSrcColorizer.addHighlight(localHeaderOffset + fileNameLength, extraFieldLength + fileCommentLength, QColor(128, 255, 0, 64));
    mSrcColorizer.addSeparation(localHeaderOffset + extraLength, 2);
    mSrcColorizer.addHighlight(localHeaderOffset + extraLength, compressedSize, QColor(128, 128, 255, 64));


    data::FileInfo fileInfo;
    fileInfo.mInfos[data::FileInfo::fileName] = Util::chunkToUtf8(chunk.subChunk(localHeaderOffset, fileNameLength));
    QDateTime dateTime = QDateTime(QDate(1980 + (modDate >> 9), (modDate >> 5) & 0xF, modDate & 0x1F), QTime(modTime >> 11, (modTime >> 5) & 0x3F, (modTime & 0x1F) << 1));
    fileInfo.mInfos[data::FileInfo::modDateTime] = dateTime.toString("dd MMM yyyy  hh:mm:ss");
    fileInfo.mInfos[data::FileInfo::size] = QString::number(uncompressedSize);
    fileInfo.mInfos[data::FileInfo::compressedSize] = QString::number(compressedSize);
    fileInfo.mInfos[data::FileInfo::crc32] = QString::fromStdString(Util::hexToString(crc32, 8));

    fileInfo.mInfos[data::FileInfo::fileComment] = Util::chunkToUtf8(chunk.subChunk(localHeaderOffset + fileNameLength + extraFieldLength, fileCommentLength));
    fileInfo.mInfos[data::FileInfo::diskNumber] = QString::number(diskNumber);
    fileInfo.mInfos[data::FileInfo::versionMadeBy] = QString::number(versionMadeBy);
    fileInfo.mInfos[data::FileInfo::versionNeeded] = QString::number(versionNeeded);
    fileInfo.mInfos[data::FileInfo::internalAttributes] = QString::number(internalAttributes);
    fileInfo.mInfos[data::FileInfo::externalAttributes] = QString::number(externalAttributes);

    localHeaderOffset += extraLength;
    if (!Util::checkRange(localHeaderOffset, compressedSize, size))
        Except::reportError(size, "zip file", "unexpected end of data");
    MemChunk compressedChunk = chunk.subChunk(localHeaderOffset, compressedSize);

    switch (compressionMethod)
    {
    case 0:
        fileInfo.mInfos[data::FileInfo::compressionMethod] = "Uncompressed";
        file.mChunk = compressedChunk;
        break;
    case 8:
        fileInfo.mInfos[data::FileInfo::compressionMethod] = "Deflate";
        {
            Deflate deflate(0x8000);
            std::shared_ptr<data::Compress> deflateData;

            if (!deflate.parse(compressedChunk, deflateData))
                Except::reportError(size, "zip file", "error parsing deflate stream");
            file.mChunk = deflateData->decomp().chunk();
        }
        break;
    default:
        fileInfo.mInfos[data::FileInfo::compressionMethod] = "Unsupported";
        file.mChunk = compressedChunk;
        Except::reportWarning(size, "zip file", "unsupported compression method");
    }

    file.mInfo = fileInfo;
}

}
}
