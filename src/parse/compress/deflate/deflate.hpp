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

#ifndef TYREX_PARSE_DEFLATE_HPP
#define TYREX_PARSE_DEFLATE_HPP

#include "parse/compress/lz.hpp"

namespace tyrex {
namespace parse {

class DeflateStream;
class HuffmanTree;

class Deflate : public Lz
{
public:
    Deflate(unsigned int windowSize);

    inline unsigned int end() const;

private:
    void doParse(const MemChunk& chunk, Shared<data::Compress>& data);
    void onError(const MemChunk& chunk, Shared<data::Compress>& data);

    void parseUncompressed(DeflateStream& stream);
    void parseFixedHuffman(DeflateStream& stream);
    void parseDynamicHuffman(DeflateStream& stream);
    void parseBlock(DeflateStream& stream, const HuffmanTree& litTree, const HuffmanTree& distTree);

    static const std::vector<std::pair<unsigned int, unsigned int> >& getLengthTable();
    static const std::vector<std::pair<unsigned int, unsigned int> >& getDistTable();
    void getLensCompressed(std::vector<unsigned int>& lens, unsigned int count, DeflateStream& stream, const HuffmanTree& lenTree);

    MemChunk mChunk;
    data::Colorizer mSrcColorizer;
    unsigned int mEnd;
};

inline unsigned int Deflate::end() const
    {return mEnd;}

}
}

#endif // TYREX_PARSE_DEFLATE_HPP
