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

#ifndef TYREX_PARSE_ELF_HPP
#define TYREX_PARSE_ELF_HPP

#include <QString>
#include "elftraits.hpp"
#include "parse/parser.tpl"
#include "data/elf.hpp"

namespace tyrex {
namespace parse {

class Elf
{
public:
    static QString parseHeaderData(unsigned char& data);
    static QString parseHeaderVersion(unsigned char& version);
    static QString parseHeaderVersion2(uint32_t& version);
    static QString parseHeaderOSABI(unsigned char& osabi);
    static QString parseHeaderType(uint16_t& type);
    static QString parseHeaderMachine(uint16_t& machine);
    static QString parseHeaderMachineVendor(const uint16_t& machine);

    static QString parseSegmentHeaderType(const uint32_t& type);
    static QString parseSegmentHeaderFlags(const uint32_t& flags);

    static QString parseSectionHeaderType(const uint32_t& type);
    static QString parseSectionHeaderFlags(const uint32_t& flags);
};


template <typename Traits>
class ParseElf : public DataParser<data::Elf>
{
public:
    ParseElf();

private:
    void doParse(const MemChunk& chunk, std::shared_ptr<data::Elf>& data);
    void onError(const MemChunk& chunk, std::shared_ptr<data::Elf>& data);

    static QStringList parseSegmentHeader(const typename Traits::phdr_type& phdr);
    static QStringList parseSectionHeader(const typename Traits::shdr_type& shdr);

    data::Colorizer mSrcColorizer;
};

typedef ParseElf<ElfTraits<64>> Elf64;
typedef ParseElf<ElfTraits<32>> Elf32;

}
}

#endif // TYREX_PARSE_ELF_HPP
