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

#ifndef TYREX_PARSE_ZIP_HPP
#define TYREX_PARSE_ZIP_HPP

#include "data/archive.hpp"
#include "parse/parser.tpl"

namespace tyrex {
namespace parse {

class Zip : public DataParser<data::Archive>
{
public:
    Zip();

private:
    void doParse(const MemChunk& chunk, std::shared_ptr<data::Archive>& data);
    void onError(const MemChunk& chunk, std::shared_ptr<data::Archive>& data);

    unsigned int findCentralDirectory(const MemChunk& chunk);

    data::Colorizer mSrcColorizer;
    std::vector<data::File> mExtractedFiles;
};

}
}

#endif // TYREX_PARSE_ZIP_HPP
