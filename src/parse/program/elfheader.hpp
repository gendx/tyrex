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

#ifndef TYREX_PARSE_ELF_HEADER_HPP
#define TYREX_PARSE_ELF_HEADER_HPP

#include "external/elf.h"
#include "misc/memchunk.hpp"
#include "data/table.hpp"

namespace tyrex {
namespace parse {

class Elf32Header
{
public:
    inline Elf32Header();
    Elf32Header(const MemChunk& chunk, data::Table& properties);
    inline const Elf32_Ehdr& header() const;

private:
    Elf32_Ehdr mHeader;
};

inline Elf32Header::Elf32Header()
    {}
inline const Elf32_Ehdr& Elf32Header::header() const
    {return mHeader;}


class Elf64Header
{
public:
    inline Elf64Header();
    Elf64Header(const MemChunk& chunk, data::Table& properties);
    inline const Elf64_Ehdr& header() const;

private:
    Elf64_Ehdr mHeader;
};

inline Elf64Header::Elf64Header()
    {}
inline const Elf64_Ehdr& Elf64Header::header() const
    {return mHeader;}

}
}

#endif // TYREX_PARSE_ELF_HEADER_HPP
