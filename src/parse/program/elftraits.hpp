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

#ifndef TYREX_PARSE_ELF_TRAITS_HPP
#define TYREX_PARSE_ELF_TRAITS_HPP

#include "elfheader.hpp"
#include "external/elf.h"

namespace tyrex {
namespace parse {

template <size_t S>
class ElfTraits
{
};

template <>
class ElfTraits<32>
{
public:
    typedef Elf32Header header_type;
    typedef Elf32_Phdr phdr_type;
    typedef Elf32_Shdr shdr_type;

    static unsigned char mMagic[5];
    static constexpr unsigned int mHeaderSize = 52;
    static constexpr size_t mBits = 32;
};

template <>
class ElfTraits<64>
{
public:
    typedef Elf64Header header_type;
    typedef Elf64_Phdr phdr_type;
    typedef Elf64_Shdr shdr_type;

    static unsigned char mMagic[5];
    static constexpr unsigned int mHeaderSize = 64;
    static constexpr size_t mBits = 64;
};

}
}

#endif // TYREX_PARSE_ELF_TRAITS_HPP
