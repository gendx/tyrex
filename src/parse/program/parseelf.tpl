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

#ifndef TYREX_PARSE_ELF_TPL
#define TYREX_PARSE_ELF_TPL

#include "parseelf.hpp"

#include "elftraits.tpl"
#include <cstring>

namespace tyrex {
namespace parse {

template <typename Traits>
ParseElf<Traits>::ParseElf()
{
}


template <typename Traits>
void ParseElf<Traits>::onError(const MemChunk& chunk, std::shared_ptr<data::Elf>& data)
{
    data = std::make_shared<data::Elf>(chunk, mSrcColorizer, QList<QStringList>(), QList<QStringList>(), data::Table());
}

template <typename Traits>
void ParseElf<Traits>::doParse(const MemChunk& chunk, std::shared_ptr<data::Elf>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < Traits::mHeaderSize)
        Except::reportError(size, "elf, header", "unexpected end of data");
    // check magic
    if (chunk.uncompare(Traits::mMagic, 5))
        Except::reportError(0, "elf, magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 4, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(4, 2);

    // header
    data::Table properties("Properties", QStringList() << "Property" << "Value");

    properties.push("ELFCLASS", QString::number(Traits::mBits));
    typename Traits::header_type header = typename Traits::header_type(chunk, properties);

    //mHeaderChunk = chunk.subChunk(4, 60);
    mSrcColorizer.addHighlight(4, Traits::mHeaderSize - 4, QColor(255, 0, 0, 64));
    mSrcColorizer.addSeparation(Traits::mHeaderSize, 2);

    properties.push("ELFENTRY", "0x" + QString::number(header.header().e_entry, 16));
    properties.push("ELFFLAGS", "0x" + QString::number(header.header().e_flags, 16));
    properties.push("ELFHEADERSIZE", QString::number(header.header().e_ehsize));
    properties.push("ELFSHSTRNDX", QString::number(header.header().e_shstrndx));

    // program header table
    unsigned int phentsize = header.header().e_phentsize;
    unsigned int phoffset = header.header().e_phoff;
    unsigned int phsize = header.header().e_phnum * phentsize;

    if (!Util::checkRange(phoffset, phsize, size))
        Except::reportError(size, "elf, program header table", "unexpected end of data");
    MemChunk programHeaderChunk = chunk.subChunk(phoffset, phsize);

    if (phsize)
    {
        mSrcColorizer.addSeparation(phoffset, 2);
        mSrcColorizer.addHighlight(phoffset, phsize, QColor(128, 255, 0, 64));
        mSrcColorizer.addSeparation(phoffset + phsize, 2);
    }

    // sections header table
    unsigned int shentsize = header.header().e_shentsize;
    unsigned int shoffset = header.header().e_shoff;
    unsigned int shsize = header.header().e_shnum * shentsize;

    if (!Util::checkRange(shoffset, shsize, size))
        Except::reportError(size, "elf, sections header table", "unexpected end of data");
    MemChunk sectionHeaderChunk = chunk.subChunk(shoffset, shsize);

    if (shsize)
    {
        mSrcColorizer.addSeparation(shoffset, 2);
        mSrcColorizer.addHighlight(shoffset, shsize, QColor(128, 0, 255, 64));
        mSrcColorizer.addSeparation(shoffset + shsize, 2);
    }

    // segments
    QList<QStringList> segmentsHeader;
    for (unsigned int i = 0 ; i < header.header().e_phnum ; ++i)
    {
        typename Traits::phdr_type phdr;
        MemChunk subchunk = programHeaderChunk.subChunk(i * phentsize, phentsize);
        if (i)
            mSrcColorizer.addSeparation(phoffset + i * phentsize, 1);

        std::memcpy(static_cast<void*>(&phdr), subchunk.data(), phentsize);
        segmentsHeader.append(parseSegmentHeader(phdr));
    }

    // sections
    QList<QStringList> sectionsHeader;
    for (unsigned int i = 0 ; i < header.header().e_shnum ; ++i)
    {
        typename Traits::shdr_type shdr;
        MemChunk subchunk = sectionHeaderChunk.subChunk(i * shentsize, shentsize);
        if (i)
            mSrcColorizer.addSeparation(shoffset + i * shentsize, 1);

        std::memcpy(static_cast<void*>(&shdr), subchunk.data(), shentsize);
        sectionsHeader.append(parseSectionHeader(shdr));
    }

    data = std::make_shared<data::Elf>(chunk, mSrcColorizer, segmentsHeader, sectionsHeader, properties);
}


template <typename Traits>
QStringList ParseElf<Traits>::parseSegmentHeader(const typename Traits::phdr_type& phdr)
{
    QString type = Elf::parseSegmentHeaderType(phdr.p_type);
    QString mode = Elf::parseSegmentHeaderFlags(phdr.p_flags);
    return QStringList() << mode << type;
}

template <typename Traits>
QStringList ParseElf<Traits>::parseSectionHeader(const typename Traits::shdr_type& shdr)
{
    QString type = Elf::parseSectionHeaderType(shdr.sh_type);
    QString mode = Elf::parseSectionHeaderFlags(shdr.sh_flags);
    return QStringList() << mode << type;
}

}
}

#endif // TYREX_PARSE_ELF_TPL
