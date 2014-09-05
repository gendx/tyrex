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

#include "lzw.hpp"

#include "forwardstream.hpp"

namespace tyrex {
namespace parse {

Lzw::Lzw(bool earlyChange) :
    mEarlyChange(earlyChange)
{
}


void Lzw::onError(const MemChunk& chunk, Shared<data::Compress>& data)
{
    data = makeShared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

void Lzw::doParse(const MemChunk& chunk, Shared<data::Compress>& data)
{
    std::vector<std::pair<unsigned int, unsigned int> > dict;
    ForwardStream stream(chunk);
    unsigned int bitcount = 9;
    unsigned int sepwidth = 0;

    for (;;)
    {
        unsigned int code = stream.get(bitcount);
        unsigned int pos = mDecompChunk.size();

        if (sepwidth)
            mDecompColorizer.addSeparation(mDecompChunk.size(), sepwidth);

        if (code < 0x100)
            mDecompChunk.appendChar(code);
        else if (code == 0x100)
        {
            dict.clear();
            bitcount = 9;
            sepwidth = 2;
            continue;
        }
        else if (code == 0x101)
            break;
        else
        {
            code -= 0x102;
            if (code >= dict.size())
                Except::reportError(stream.pos(), "lzw, code", "code is above dictionary size");

            std::pair<unsigned int, unsigned int> copy = dict[code];
            for (unsigned int i = 0 ; i < copy.second ; ++i)
                mDecompChunk.appendChar(mDecompChunk[copy.first + i]);
        }

        sepwidth = 1;
        dict.push_back(std::make_pair(pos, mDecompChunk.size() + 1 - pos));
        if (dict.size() + (mEarlyChange ? 0x102 : 0x101) == 1 << bitcount)
        {
            ++bitcount;
            if (bitcount == 13)
                Except::reportError(stream.pos(), "lzw, dictionary", "reached dictionary size limit");
        }
    }

    data = makeShared<data::Compress>(chunk, mDecompChunk, mSrcColorizer, mDecompColorizer);
}

}
}
