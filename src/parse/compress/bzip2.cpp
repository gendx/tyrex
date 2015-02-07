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

#include "bzip2.hpp"

#include "misc/hash/hash.hpp"

namespace tyrex {
namespace parse {

unsigned char Bzip2::mMagic[3] = {0x42, 0x5a, 0x68};
unsigned char Bzip2::mMagicPi[6] = {0x31, 0x41, 0x59, 0x26, 0x53, 0x59};
unsigned char Bzip2::mMagicSqrtPi[6] = {0x17, 0x72, 0x45, 0x38, 0x50, 0x90};

Bzip2::Bzip2() :
    mCombinedCRC(0)
{
}


void Bzip2::onError(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Bzip2::doParse(const MemChunk& chunk, std::shared_ptr<data::Compress>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 4)
        Except::reportError(size, "bzip2, magic", "unexpected end of data");
    if (chunk.uncompare(Bzip2::mMagic, 3))
        Except::reportError(0, "bzip2, magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 3, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(3, 2);

    unsigned int blockSize100k = chunk[3]; // 0x30 = '0'
    if (blockSize100k <= 0x30 || blockSize100k >= 0x40)
        Except::reportError(3, "bzip2, block size", "invalid block size");
    blockSize100k = (blockSize100k - 0x30) * 100000;

    // Bit stream
    ForwardStream stream(chunk);
    stream.skipBytes(4);
    while (this->parseBlock(stream, blockSize100k))
        ;

    for (unsigned int i = 1 ; i < 6 ; ++i)
        if (stream.get(8) != mMagicSqrtPi[i])
            Except::reportError(stream.pos(), "bzip2, end of stream magic", "invalid magic");

    unsigned int crc32 = stream.get(32);
    if (mCombinedCRC != crc32)
        Except::reportError(stream.pos(), "bzip2", "invalid CRC");

    stream.flushByte();
    if (stream.pos() != size)
        Except::reportError(stream.pos(), "bzip2", "expected end of data");

    mSrcColorizer.addHighlight(3, size - 3, QColor(128, 128, 255, 64));

    data = std::make_shared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}


bool Bzip2::parseBlock(ForwardStream& stream, unsigned int blockSize100k)
{
    unsigned char c = stream.get(8);
    if (c == mMagicSqrtPi[0])
        return false;

    for (unsigned int i = 0 ; i < 6 ; ++i)
    {
        if (c != mMagicPi[i])
            Except::reportError(stream.pos(), "bzip2, block magic", "invalid magic");
        if (i < 5)
            c = stream.get(8);
    }

    unsigned int crc = stream.get(32);

    bool randomized = stream.get();
    if (randomized)
        Except::reportError(stream.pos(), "bzip2, block", "deprecated randomized feature");

    unsigned int origPtr = stream.get(24);

    std::vector<unsigned char> symbolMap;
    this->getUsedSymbols(stream, symbolMap);
    unsigned int alphaSize = symbolMap.size() + 2;

    unsigned int groups = stream.get(3);
    if (groups < 2 || groups > 6)
        Except::reportError(stream.pos(), "bzip2, block", "invalid number of huffman tables");

    std::vector<unsigned int> selectors;
    unsigned int numSelectors = this->getSelectors(stream, selectors, groups);

    std::vector<HuffmanTree> trees;
    this->getHuffmanTrees(stream, trees, groups, alphaSize);

    std::vector<unsigned int> unzftab(0x100, 0);
    MoveToFront mtf;

    unsigned int EOB = symbolMap.size() + 1;
    unsigned int group = -1;
    unsigned int groupPos = 0;
    HuffmanTree* tree = NULL;
    unsigned int repeats = 0;
    unsigned int nRepeats = 0;

    Chunk<unsigned int> blockUint;

    for (;;)
    {
        if (!groupPos)
        {
            ++group;
            if (group >= numSelectors)
                Except::reportError(stream.pos(), "bzip2, huffman tree selection", "no selector for this tree");

            groupPos = 50;
            tree = &trees[selectors[group]];
        }

        --groupPos;
        unsigned int val = tree->parse(stream);

        if (val == EOB)
            break;
        else if (val == 0) // RUNA
        {
            repeats += (1 << nRepeats);
            ++nRepeats;
        }
        else if (val == 1) // RUNB
        {
            ++nRepeats;
            repeats += (1 << nRepeats);
        }
        else
        {
            if (nRepeats)
            {
                this->appendRepeats(stream, blockUint, symbolMap, mtf, unzftab, repeats);
                nRepeats = 0;
                repeats = 0;
            }

            unsigned char byte = symbolMap[mtf[val - 1]];
            ++unzftab[byte];
            blockUint.append(byte);
        }
    }

    if (nRepeats)
        this->appendRepeats(stream, blockUint, symbolMap, mtf, unzftab, repeats);

    unsigned int blockLength = blockUint.size();
    if (blockLength > blockSize100k)
        Except::reportError(stream.pos(), "bzip2, BWT", "block length greater than expected");
    if (origPtr >= blockLength)
        Except::reportError(stream.pos(), "bzip2, BWT", "invalid origin pointer");

    std::vector<unsigned int> cftab(0x100, 0);
    for (unsigned int i = 1 ; i < 0x100 ; ++i)
        cftab[i] = unzftab[i - 1];
    for (unsigned int i = 1 ; i < 0x100 ; ++i)
        cftab[i] += cftab[i - 1];

    blockUint.append(0u, blockSize100k - blockLength);
    for (unsigned int i = 0 ; i < blockLength ; ++i)
    {
        unsigned char byte = blockUint[i] & 0xFF;
        blockUint[cftab[byte]] |= (i << 8);
        ++cftab[byte];
    }

    MemChunk block;
    for (unsigned int pos = blockUint[origPtr] >> 8 ; pos < blockSize100k && block.size() < blockLength ;)
    {
        pos = blockUint[pos];
        block.appendChar(pos);
        pos >>= 8;
    }

    MemChunk result;
    unsigned int last = 0xFFFFFFFF;
    unsigned int count = 0;
    for (unsigned int i = 0 ; i < block.size() ; ++i)
    {
        if (block[i] != last)
        {
            last = block[i];
            count = 0;
        }

        ++count;
        result.appendChar(block[i]);

        if (count == 4)
        {
            result.append(block[i], block[i + 1]);
            ++i;
            last = 0xFFFFFFFF;
        }
    }

    if (Hasher::getCRC32Reverse(result) != crc)
        Except::reportError(stream.pos(), "bzip2", "invalid block CRC");

    mCombinedCRC = ((mCombinedCRC << 1) | (mCombinedCRC >> 31)) ^ crc;
    mDecompChunk.append(result);

    return true;
}


void Bzip2::appendRepeats(ForwardStream& stream, Chunk<unsigned int>& blockUint, const std::vector<unsigned char>& symbolMap, const MoveToFront& mtf, std::vector<unsigned int>& unzftab, unsigned int repeats)
{
    if (repeats > 0x200000)
        Except::reportError(stream.pos(), "bzip2, RLE", "too many repeats");

    unsigned char byte = symbolMap[mtf.front()];
    unzftab[byte] += repeats;

    blockUint.append(byte, repeats);
}


void Bzip2::getUsedSymbols(ForwardStream& stream, std::vector<unsigned char>& symbolMap)
{
    std::vector<bool> inUse16;
    for (unsigned int i = 0 ; i < 16 ; ++i)
        inUse16.push_back(stream.get());

    std::vector<bool> inUse(256, false);
    for (unsigned int i = 0 ; i < 16 ; ++i)
        if (inUse16[i])
            for (unsigned int j = 0 ; j < 16 ; ++j)
                inUse[(i << 4) ^ j] = stream.get();

    for (unsigned int i = 0 ; i < 256 ; ++i)
        if (inUse[i])
            symbolMap.push_back(i);

    if (!symbolMap.size())
        Except::reportError(stream.pos(), "bzip2, symbol map", "no symbol used");
}

unsigned int Bzip2::getSelectors(ForwardStream& stream, std::vector<unsigned int>& selectors, unsigned int groups)
{
    unsigned int numSelectors = stream.get(15);
    if (numSelectors < 1)
        Except::reportError(stream.pos(), "bzip2, selectors", "no selector declared");

    std::vector<unsigned int> selectorsMTF;
    for (unsigned int i = 0 ; i < numSelectors ; ++i)
    {
        unsigned int j = 0;
        while (stream.get())
        {
            ++j;
            if (j >= groups)
                Except::reportError(stream.pos(), "bzip2, selector", "too large selector");
        }
        selectorsMTF.push_back(j);
    }

    std::vector<unsigned int> pos;
    for (unsigned int v = 0 ; v < groups ; ++v)
        pos.push_back(v);

    for (unsigned int i = 0 ; i < numSelectors ; ++i)
    {
        unsigned int v = selectorsMTF[i];
        unsigned int tmp = pos[v];
        for (; v > 0 ; --v)
            pos[v] = pos[v - 1];
        pos[0] = tmp;
        selectors.push_back(tmp);
    }

    return numSelectors;
}

void Bzip2::getHuffmanTrees(ForwardStream& stream, std::vector<HuffmanTree>& trees, unsigned int groups, unsigned int alphaSize)
{
    std::vector<std::vector<unsigned int> > len;
    for (unsigned int i = 0 ; i < groups ; ++i)
    {
        unsigned int tmp = stream.get(5);
        len.push_back(std::vector<unsigned int>());
        for (unsigned int j = 0 ; j < alphaSize ; ++j)
        {
            for (;;)
            {
                if (tmp < 1 || tmp > 20)
                    Except::reportError(stream.pos(), "bzip2, coding", "invalid length");
                if (!stream.get())
                    break;
                if (stream.get())
                    --tmp;
                else
                    ++tmp;
            }

            len[i].push_back(tmp);
        }
    }

    for (unsigned int i = 0 ; i < groups ; ++i)
    {
        bool check;
        trees.push_back(HuffmanTree(len[i], check));
        if (!check)
            Except::reportError(stream.pos(), "bzip2, huffman tree", "invalid huffman tree");
    }
}

}
}
