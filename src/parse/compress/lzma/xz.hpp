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

#ifndef TYREX_PARSE_XZ_HPP
#define TYREX_PARSE_XZ_HPP

#include "parse/compress/parsecompress.hpp"

namespace tyrex {
namespace parse {

class Xz : public Compress
{
public:
    Xz() = default;

private:
    void doParse(const MemChunk& chunk, std::shared_ptr<data::Compress>& data);
    void onError(const MemChunk& chunk, std::shared_ptr<data::Compress>& data);

    struct Filter
    {
        unsigned int mPos;
        uint64_t mId;
        MemChunk mProperties;
    };

    void parseBlock(const MemChunk& chunk, unsigned int& processed, unsigned int size, unsigned char checkMethod, uint64_t& unpaddedSize, uint64_t& uncompressedSize);
    void parseBlockHeader(const MemChunk& chunk, unsigned int& processed, unsigned int size, std::vector<Filter>& filters, uint64_t& compressedSize, bool& hasCompressedSize, uint64_t& decompressedSize, bool& hasDecompressedSize);
    void parseIndex(const MemChunk& chunk, unsigned int& processed, unsigned int size, const std::vector<std::pair<uint64_t, uint64_t> >& records);
    unsigned int decodeLzma2(const MemChunk& src, MemChunk& dst, unsigned int& processed, unsigned int size, const Filter& filter);

    unsigned int checkPadding(const MemChunk& chunk, unsigned int& processed, unsigned int size, const std::string& who);
    bool getMultibyte(const MemChunk& chunk, unsigned int& pos, unsigned int maxpos, uint64_t& result);

    static unsigned char mMagic[6];
    static unsigned char mMagicFooter[2];

    data::Colorizer mSrcColorizer;
};

}
}

#endif // TYREX_PARSE_XZ_HPP
