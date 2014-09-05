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

#include "tar.hpp"

#include "tarfile.hpp"

namespace tyrex {
namespace parse {

Tar::Tar()
{
}


void Tar::onError(const MemChunk& chunk, Shared<data::Archive>& data)
{
    data = makeShared<data::Archive>(chunk, mSrcColorizer, data::FileInfoFilter::mTarFilter, mExtractedFiles);
}

void Tar::doParse(const MemChunk& chunk, Shared<data::Archive>& data)
{
    unsigned int size = chunk.size();
    unsigned int processed = 0;

    while (processed < size)
    {
        TarFile tarFile(mSrcColorizer, processed);

        data::File file;
        if (!tarFile.parse(chunk.subChunk(processed), file))
        {
            Except::reportWarning(processed, "tar file extraction", "error decoding file");
            break;
        }

        processed = tarFile.processed();
        mExtractedFiles.push_back(file);
    }

    data = makeShared<data::Archive>(chunk, mSrcColorizer, data::FileInfoFilter::mTarFilter, mExtractedFiles);
}

}
}
