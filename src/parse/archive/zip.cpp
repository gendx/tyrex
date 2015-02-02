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

#include "zip.hpp"

#include "zipfile.hpp"

namespace tyrex {
namespace parse {

Zip::Zip()
{
}


void Zip::onError(const MemChunk& chunk, Shared<data::Archive>& data)
{
    data = makeShared<data::Archive>(chunk, mSrcColorizer, data::FileInfoFilter::mZipFilter, mExtractedFiles);
}

void Zip::doParse(const MemChunk& chunk, Shared<data::Archive>& data)
{
    unsigned int size = chunk.size();
    unsigned int pos = this->findCentralDirectory(chunk);

    mSrcColorizer.addHighlight(pos, 4, QColor(255, 128, 0, 64));
    mSrcColorizer.addHighlight(pos + 4, 18, QColor(255, 0, 0, 64));

    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + 4, 2);
    mSrcColorizer.addSeparation(pos + 6, 1);
    mSrcColorizer.addSeparation(pos + 8, 1);
    mSrcColorizer.addSeparation(pos + 10, 1);
    mSrcColorizer.addSeparation(pos + 12, 1);
    mSrcColorizer.addSeparation(pos + 16, 1);
    mSrcColorizer.addSeparation(pos + 20, 1);
    mSrcColorizer.addSeparation(pos + 22, 2);


    unsigned int thisDisk = chunk.getUint16LE(pos + 4);
    unsigned int centralDirDisk = chunk.getUint16LE(pos + 6);
    unsigned int entriesOnDisk = chunk.getUint16LE(pos + 8);
    unsigned int entriesInCentralDir = chunk.getUint16LE(pos + 10);
    unsigned int centralDirSize = chunk.getUint32LE(pos + 12);
    unsigned int centralDirStart = chunk.getUint32LE(pos + 16);

    if (thisDisk != centralDirDisk || entriesOnDisk != entriesInCentralDir)
        Except::reportError(pos - 20, "zip central directory", "multi-files zip archives not supported");

    unsigned int processed = 0;
    for (unsigned int i = 0 ; i < entriesInCentralDir ; ++i)
    {
        if (processed > centralDirSize)
            Except::reportWarning(i, "zip file extraction", "end of central directory");

        MemChunk centralDir = chunk.subChunk(centralDirStart + processed, centralDirSize - processed);
        ZipFile zipFile(mSrcColorizer, centralDir, centralDirStart + processed, centralDirSize - processed);

        data::File file;
        if (!zipFile.parse(chunk, file))
            Except::reportWarning(i, "zip file extraction", "error decoding file");

        processed += zipFile.processed();
        mExtractedFiles.push_back(file);
    }

    data = makeShared<data::Archive>(chunk, mSrcColorizer, data::FileInfoFilter::mZipFilter, mExtractedFiles);
}

unsigned int Zip::findCentralDirectory(const MemChunk& chunk)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 22)
        Except::reportError(size, "zip central directory finder", "unexpected end of data");

    unsigned int pos = size - 22;
    std::vector<unsigned int> possible;
    std::vector<unsigned int> probable;

    for (;; --pos)
    {
        if (chunk.getUint32LE(pos) == 0x06054B50)
        {
            possible.push_back(pos);
            if (chunk.getUint16LE(pos + 20) == size - 22 - pos)
                probable.push_back(pos);
        }

        if (pos == 0 || size - 22 - pos == 0xFFFF)
            break;
    }

    if (possible.empty())
        Except::reportError(size, "zip central directory finder", "central directory not found");

    if (probable.empty())
    {
        pos = possible[0];

        if (chunk.getUint16LE(pos + 20) > size - 22 - pos)
            Except::reportWarning(pos, "zip central directory finder", "zip seems to be truncated, trying anyway");
        else if (chunk.getUint16LE(pos + 20) < size - 22 - pos)
            Except::reportWarning(pos, "zip central directory finder", "zip comment does not fill the file, trying anyway");
    }
    else
    {
        pos = probable[0];
        if (probable.size() >= 2)
            Except::reportWarning(pos, "zip central directory finder", "several central directories found, selecting the last match");
    }

    return pos;
}

}
}
