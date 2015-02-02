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

#ifndef TYREX_PARSE_BZIP2_HPP
#define TYREX_PARSE_BZIP2_HPP

#include "parsecompress.hpp"

#include "forwardstream.hpp"
#include "huffmantree.hpp"
#include "movetofront.hpp"

namespace tyrex {
namespace parse {

class Bzip2 : public Compress
{
public:
    Bzip2();

private:
    void doParse(const MemChunk& chunk, Shared<data::Compress>& data);
    void onError(const MemChunk& chunk, Shared<data::Compress>& data);

    bool parseBlock(ForwardStream& stream, unsigned int blockSize100k);
    void appendRepeats(ForwardStream& stream, Chunk<unsigned int>& blockUint, const std::vector<unsigned char>& usedMap, const MoveToFront& mtf, std::vector<unsigned int>& unzftab, unsigned int repeats);

    void getUsedSymbols(ForwardStream& stream, std::vector<unsigned char>& usedMap);
    unsigned int getSelectors(ForwardStream& stream, std::vector<unsigned int>& selectors, unsigned int groups);
    void getHuffmanTrees(ForwardStream& stream, std::vector<HuffmanTree>& trees, unsigned int groups, unsigned int alphaSize);

    static unsigned char mMagic[3];
    static unsigned char mMagicPi[6];
    static unsigned char mMagicSqrtPi[6];

    data::Colorizer mSrcColorizer;
    unsigned int mCombinedCRC;
};

}
}

#endif // TYREX_PARSE_BZIP2_HPP
