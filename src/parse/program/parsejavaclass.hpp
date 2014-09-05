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

#ifndef TYREX_PARSE_JAVACLASS_HPP
#define TYREX_PARSE_JAVACLASS_HPP

#include "parse/parser.tpl"
#include "data/javaclass.hpp"

namespace tyrex {
namespace parse {

class JavaClass : public DataParser<data::JavaClass>
{
public:
    JavaClass();

private:
    void doParse(const MemChunk& chunk, Shared<data::JavaClass>& data);
    void onError(const MemChunk& chunk, Shared<data::JavaClass>& data);

    static unsigned char mMagic[4];

    data::Colorizer mSrcColorizer;
};

}
}

#endif // TYREX_PARSE_JAVACLASS_HPP
