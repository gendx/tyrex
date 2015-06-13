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

#include "zlib.hpp"

#include "deflate.hpp"
#include "misc/hash/hash.hpp"

namespace tyrex {
namespace parse {

void Zlib::onError(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Zlib::doParse(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 2)
        Except::reportError(size, "zlib header", "unexpected end of data");

    unsigned int header = chunk.getUint16BE(0);
    if (header % 31 != 0)
        Except::reportError(0, "zlib header", "invalid header check");

    mSrcColorizer.addHighlight(0, 2, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(2, 2);

    unsigned int method = (header >> 8) & 0x0F;
    unsigned int info = (header >> 12) & 0x0F;
    unsigned int fdict = (header >> 5) & 0x01;
    unsigned int flags = (header >> 6) & 0x03;

    if (method != 8)
        Except::reportError(0, "zlib header, method", "invalid method");
    if (info > 7)
        Except::reportError(0, "zlib header, window size", "invalid window size");
    if (fdict)
        Except::reportError(1, "zlib header, preset dictionary", "preset dictionaries are not supported");

    unsigned int windowSize = 1 << (info + 8);
    Deflate deflate(windowSize);
    std::shared_ptr<data::Compress> deflateData;

    if (!deflate.parse(chunk.subChunk(2, chunk.size() - 2), deflateData))
        Except::reportError(size, "zlib, deflate", "error parsing deflate stream");

    mDecompChunk = deflateData->decomp().chunk();
    mDecompColorizer = deflateData->decomp().colorizer();

    unsigned int processed = 2 + deflate.end();
    if (!Util::checkRange(processed, 4, size))
        Except::reportError(size, "zlib adler32", "unexpected end of data");

    mSrcColorizer.addSeparation(processed, 2);

    unsigned int adler = chunk.getUint32BE(processed);
    if (adler != Hasher::getAdler32(deflateData->decomp().chunk()))
        Except::reportError(processed, "zlib adler32", "invalid adler32");

    mSrcColorizer.addHighlight(processed, 4, QColor(0, 255, 0, 64));

    processed += 4;
    if (size != processed)
        Except::reportError(processed, "zlib", "expected end of data");

    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

}
}
