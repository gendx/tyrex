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

#include "fileinfo.hpp"

namespace tyrex {
namespace data {

QString FileInfo::operator[](Info key) const
{
    std::map<Info, QString>::const_iterator found = mInfos.find(key);
    if (found != mInfos.end())
        return found->second;
    return QString();
}

QString FileInfo::infoToString(Info info)
{
    switch (info)
    {
    case versionMadeBy:
        return "versionMadeBy";
    case versionNeeded:
        return "versionNeeded";
    case compressionMethod:
        return "method";
    case modDateTime:
        return "modified";
    case crc32:
        return "CRC32";
    case compressedSize:
        return "compressed";
    case size:
        return "size";
    case fileName:
        return "file";
    case fileComment:
        return "comment";
    case diskNumber:
        return "disk number";
    case internalAttributes:
        return "internalAttributes";
    case externalAttributes:
        return "externalAttributes";
    case fileMode:
        return "mode";
    case userId:
        return "uid";
    case groupId:
        return "gid";
    case userName:
        return "user";
    case groupName:
        return "group";
    case devMajorMinor:
        return "device";
    case linkName:
        return "link";
    }

    return QString();
}


const FileInfoFilter FileInfoFilter::mZipFilter {{
    {FileInfo::fileName, true},
    {FileInfo::modDateTime, true},
    {FileInfo::size, true},
    {FileInfo::compressedSize, true},
    {FileInfo::compressionMethod, true},
    {FileInfo::crc32, true},
    {FileInfo::fileComment, false},
    {FileInfo::diskNumber, false},
    {FileInfo::versionMadeBy, false},
    {FileInfo::versionNeeded, false},
    {FileInfo::internalAttributes, false},
    {FileInfo::externalAttributes, false}
}};

const FileInfoFilter FileInfoFilter::mTarFilter {{
    {FileInfo::fileName, true},
    {FileInfo::modDateTime, true},
    {FileInfo::size, true},
    {FileInfo::fileMode, true},
    {FileInfo::userId, true},
    {FileInfo::groupId, true},
    {FileInfo::userName, true},
    {FileInfo::groupName, true},
    {FileInfo::devMajorMinor, true},
    {FileInfo::linkName, false}
}};

}
}
