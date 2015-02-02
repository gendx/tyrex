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

#include "elf32.hpp"

#include "elf32header.hpp"
#include "misc/util.hpp"
#include <cstring>

namespace tyrex {
namespace parse {

unsigned char Elf32::mMagic[5] = {0x7F, 0x45, 0x4C, 0x46, 0x01};

Elf32::Elf32()
{
}


void Elf32::onError(const MemChunk& chunk, Shared<data::Elf>& data)
{
    data = makeShared<data::Elf>(chunk, mSrcColorizer, QList<QStringList>(), QList<QStringList>(), data::Table());
}

void Elf32::doParse(const MemChunk& chunk, Shared<data::Elf>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 52)
        Except::reportError(size, "elf32, header", "unexpected end of data");
    // check magic
    if (chunk.uncompare(Elf32::mMagic, 5))
        Except::reportError(0, "elf32, magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 4, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(4, 2);

    // header
    data::Table properties("Properties", QStringList() << "Property" << "Value");

    properties.push("ELFCLASS", "32");
    Elf32Header header32 = Elf32Header(chunk, properties);

    mSrcColorizer.addHighlight(4, 48, QColor(255, 0, 0, 64));
    mSrcColorizer.addSeparation(52, 2);

    properties.push("ELFENTRY", "0x" + QString::number(header32.header().e_entry, 16));
    properties.push("ELFFLAGS", "0x" + QString::number(header32.header().e_flags, 16));
    properties.push("ELFHEADERSIZE", QString::number(header32.header().e_ehsize));
    properties.push("ELFSHSTRNDX", QString::number(header32.header().e_shstrndx));

    // program header table
    unsigned int phentsize = header32.header().e_phentsize;
    unsigned int phoffset = header32.header().e_phoff;
    unsigned int phsize = header32.header().e_phnum * phentsize;

    if (size < phoffset + phsize)
        Except::reportError(size, "elf32, program header table", "unexpected end of data");
    MemChunk programHeaderChunk = chunk.subChunk(phoffset, phsize);

    if (phsize)
    {
        mSrcColorizer.addSeparation(phoffset, 2);
        mSrcColorizer.addHighlight(phoffset, phsize, QColor(128, 255, 0, 64));
        mSrcColorizer.addSeparation(phoffset + phsize, 2);
    }

    // sections header table
    unsigned int shentsize = header32.header().e_shentsize;
    unsigned int shoffset = header32.header().e_shoff;
    unsigned int shsize = header32.header().e_shnum * shentsize;

    if (size < shoffset + shsize)
        Except::reportError(size, "elf32, sections header table", "unexpected end of data");
    MemChunk sectionHeaderChunk = chunk.subChunk(shoffset, shsize);

    if (shsize)
    {
        mSrcColorizer.addSeparation(shoffset, 2);
        mSrcColorizer.addHighlight(shoffset, shsize, QColor(128, 0, 255, 64));
        mSrcColorizer.addSeparation(shoffset + shsize, 2);
    }

    // segments
    QList<QStringList> segmentsHeader;
    for (unsigned int i = 0 ; i < header32.header().e_phnum ; ++i)
    {
        Elf32_Phdr phdr;
        MemChunk subchunk = programHeaderChunk.subChunk(i * phentsize, phentsize);
        if (i)
            mSrcColorizer.addSeparation(phoffset + i * phentsize, 1);

        std::memcpy(static_cast<void*>(&phdr), subchunk.data(), phentsize);
        segmentsHeader.append(Elf32::parseSegmentHeader(phdr));
    }

    // sections
    QList<QStringList> sectionsHeader;
    for (unsigned int i = 0 ; i < header32.header().e_shnum ; ++i)
    {
        Elf32_Shdr shdr;
        MemChunk subchunk = sectionHeaderChunk.subChunk(i * shentsize, shentsize);
        if (i)
            mSrcColorizer.addSeparation(shoffset + i * shentsize, 1);

        std::memcpy(static_cast<void*>(&shdr), subchunk.data(), shentsize);
        sectionsHeader.append(Elf32::parseSectionHeader(shdr));
    }

    data = makeShared<data::Elf>(chunk, mSrcColorizer, segmentsHeader, sectionsHeader, properties);
}


QStringList Elf32::parseSegmentHeader(const Elf32_Phdr& phdr)
{
    QString type;
    switch (phdr.p_type)
    {
    case PT_NULL:
        type = "PT_NULL";
        break;
    case PT_LOAD:
        type = "PT_LOAD";
        break;
    case PT_DYNAMIC:
        type = "PT_DYNAMIC";
        break;
    case PT_INTERP:
        type = "PT_INTERP";
        break;
    case PT_NOTE:
        type = "PT_NOTE";
        break;
    case PT_SHLIB:
        type = "PT_SHLIB";
        break;
    case PT_PHDR:
        type = "PT_PHDR";
        break;
    default:
        if (phdr.p_type >= PT_LOPROC && phdr.p_type <= PT_HIPROC)
            type = "PT_PROC";
        else
            type = QString::fromStdString("PT_INVALID (" + Util::hexToString(phdr.p_type) + ")");
    }

    QString mode = "---";
    if (phdr.p_flags & PF_R)
        mode[0] = 'R';
    if (phdr.p_flags & PF_W)
        mode[1] = 'W';
    if (phdr.p_flags & PF_X)
        mode[2] = 'X';

    return QStringList() << mode << type;
}

QStringList Elf32::parseSectionHeader(const Elf32_Shdr& shdr)
{
    QString type;
    switch (shdr.sh_type)
    {
    case SHT_NULL:
        type = "SHT_NULL";
        break;
    case SHT_PROGBITS:
        type = "SHT_PROGBITS";
        break;
    case SHT_SYMTAB:
        type = "SHT_SYMTAB";
        break;
    case SHT_STRTAB:
        type = "SHT_STRTAB";
        break;
    case SHT_RELA:
        type = "SHT_RELA";
        break;
    case SHT_HASH:
        type = "SHT_HASH";
        break;
    case SHT_DYNAMIC:
        type = "SHT_DYNAMIC";
        break;
    case SHT_NOTE:
        type = "SHT_NOTE";
        break;
    case SHT_NOBITS:
        type = "SHT_NOBITS";
        break;
    case SHT_REL:
        type = "SHT_REL";
        break;
    case SHT_SHLIB:
        type = "SHT_SHLIB";
        break;
    case SHT_DYNSYM:
        type = "SHT_DYNSYM";
        break;
    default:
        if (shdr.sh_type >= SHT_LOPROC && shdr.sh_type <= SHT_HIPROC)
            type = "SHT_PROC";
        else if (shdr.sh_type >= SHT_LOUSER && shdr.sh_type <= SHT_HIUSER)
            type = "SHT_PROC";
        else
            type = QString::fromStdString("SHT_INVALID (" + Util::hexToString(shdr.sh_type) + ")");
    }

    QString mode = "---";
    if (shdr.sh_flags & SHF_ALLOC)
        mode[0] = 'A';
    if (shdr.sh_flags & SHF_WRITE)
        mode[1] = 'W';
    if (shdr.sh_flags & SHF_EXECINSTR)
        mode[2] = 'X';

    return QStringList() << mode << type;
}

}
}
