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

#ifndef TYREX_DATA_FILEINFO_HPP
#define TYREX_DATA_FILEINFO_HPP

#include <map>
#include <vector>
#include <QString>

namespace tyrex {
namespace data {

class FileInfo
{
public:
    enum Info {
        // zip
        versionMadeBy, versionNeeded, compressionMethod, modDateTime, crc32, compressedSize, size, fileName, fileComment, diskNumber, internalAttributes, externalAttributes,
        // tar
        fileMode, userId, groupId, userName, groupName, devMajorMinor, linkName
    };

    QString operator[](Info key) const;

    static QString infoToString(Info info);

    std::map<Info, QString> mInfos;
};

class FileInfoFilter
{
public:
    static const FileInfoFilter mZipFilter;
    static const FileInfoFilter mTarFilter;

    std::vector<std::pair<FileInfo::Info, bool> > mInfos;
};

}
}

#endif // TYREX_DATA_FILEINFO_HPP
