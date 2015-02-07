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

#ifndef TYREX_PARSE_TRUETYPE_HPP
#define TYREX_PARSE_TRUETYPE_HPP

#include "parse/parser.tpl"
#include "data/font/font.hpp"

namespace tyrex {
namespace parse {

class Truetype : public DataParser<data::Font>
{
public:
    Truetype();

private:
    void doParse(const MemChunk& chunk, std::shared_ptr<data::Font>& data);
    void onError(const MemChunk& chunk, std::shared_ptr<data::Font>& data);

    void parseHead(const MemChunk& chunk, unsigned int pos, unsigned int size);
    void parseHhea(const MemChunk& chunk, unsigned int pos, unsigned int size);
    void parseMaxp(const MemChunk& chunk, unsigned int pos, unsigned int size);
    void parseHmtx(const MemChunk& chunk, unsigned int pos, unsigned int size);
    void parseLoca(const MemChunk& chunk, unsigned int pos, unsigned int size);
    void parseGlyf(const MemChunk& chunk, unsigned int pos, unsigned int size);
    bool parseGlyph(const MemChunk& glyf);
    bool parseCompoundGlyph(const MemChunk& glyf);

    void appendPath(std::shared_ptr<data::Path> path, const std::vector<std::pair<data::Path::Point, bool> >& points);

    static unsigned char mMagic1[4];
    static unsigned char mMagic2[4];
    static unsigned int mMagicHead;

    data::Colorizer mSrcColorizer;
    QList<std::shared_ptr<data::Path> > mGlyphs;
    data::Table mProperties;

    unsigned int mIndexToLocFormat;
    unsigned int mNumGlyphs;
    unsigned int mNumOfLongHorMetrics;
    std::vector<unsigned int> mLocations;
    std::vector<std::pair<unsigned int, int> > mHorizontalMetrics;
};

}
}

#endif // TYREX_PARSE_TRUETYPE_HPP
