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

#ifndef TYREX_PARSE_TARFILE_HPP
#define TYREX_PARSE_TARFILE_HPP

#include "parse/parser.tpl"
#include "data/file.hpp"
#include "data/colorizer.hpp"

namespace tyrex {
namespace parse {

class TarFile : public MemchunkParser<data::File>
{
public:
    TarFile(data::Colorizer& srcColorizer, unsigned int processed);

    inline unsigned int processed() const;

private:
    void doParse(const MemChunk& chunk, data::File& file);

    data::Colorizer& mSrcColorizer;
    unsigned int mProcessed;
};

inline unsigned int TarFile::processed() const
    {return mProcessed;}

}
}

#endif // TYREX_PARSE_TARFILE_HPP
