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

#include "parseelf.hpp"

#include "external/elf.h"
#include "misc/util.hpp"

namespace tyrex {
namespace parse {

QString Elf::parseSegmentHeaderType(const uint32_t& type)
{
    switch (type)
    {
    case PT_NULL:
        return "PT_NULL";
    case PT_LOAD:
        return "PT_LOAD";
    case PT_DYNAMIC:
        return "PT_DYNAMIC";
    case PT_INTERP:
        return "PT_INTERP";
    case PT_NOTE:
        return "PT_NOTE";
    case PT_SHLIB:
        return "PT_SHLIB";
    case PT_PHDR:
        return "PT_PHDR";
    default:
        if (type >= PT_LOPROC && type <= PT_HIPROC)
            return "PT_PROC";
        else
            return QString::fromStdString("PT_INVALID (" + Util::hexToString(type) + ")");
    }
}

QString Elf::parseSegmentHeaderFlags(const uint32_t& flags)
{
    QString mode = "---";
    if (flags & PF_R)
        mode[0] = 'R';
    if (flags & PF_W)
        mode[1] = 'W';
    if (flags & PF_X)
        mode[2] = 'X';
    return mode;
}


QString Elf::parseSectionHeaderType(const uint32_t& type)
{
    switch (type)
    {
    case SHT_NULL:
        return "SHT_NULL";
    case SHT_PROGBITS:
        return "SHT_PROGBITS";
    case SHT_SYMTAB:
        return "SHT_SYMTAB";
    case SHT_STRTAB:
        return "SHT_STRTAB";
    case SHT_RELA:
        return "SHT_RELA";
    case SHT_HASH:
        return "SHT_HASH";
    case SHT_DYNAMIC:
        return "SHT_DYNAMIC";
    case SHT_NOTE:
        return "SHT_NOTE";
    case SHT_NOBITS:
        return "SHT_NOBITS";
    case SHT_REL:
        return "SHT_REL";
    case SHT_SHLIB:
        return "SHT_SHLIB";
    case SHT_DYNSYM:
        return "SHT_DYNSYM";
    default:
        if (type >= SHT_LOPROC && type <= SHT_HIPROC)
            return "SHT_PROC";
        else if (type >= SHT_LOUSER && type <= SHT_HIUSER)
            return "SHT_PROC";
        else
            return QString::fromStdString("SHT_INVALID (" + Util::hexToString(type) + ")");
    }
}

QString Elf::parseSectionHeaderFlags(const uint32_t& flags)
{
    QString mode = "---";
    if (flags & SHF_ALLOC)
        mode[0] = 'A';
    if (flags & SHF_WRITE)
        mode[1] = 'W';
    if (flags & SHF_EXECINSTR)
        mode[2] = 'X';
    return mode;
}


QString Elf::parseHeaderData(unsigned char& data)
{
    switch (data)
    {
    case ELFDATA2LSB:
        return "2LSB";
    case ELFDATA2MSB:
        return "2MSB";
    default:
        data = ELFDATANONE;
        return "NONE";
    }
}

QString Elf::parseHeaderVersion(unsigned char& version)
{
    switch (version)
    {
    case EV_CURRENT:
        return "CURRENT";
    default:
        version = EV_NONE;
        return "NONE";
    }
}

QString Elf::parseHeaderVersion2(uint32_t& version)
{
    switch (version)
    {
    case EV_CURRENT:
        return "CURRENT";
    default:
        version = EV_NONE;
        return "NONE";
    }
}

QString Elf::parseHeaderOSABI(unsigned char& osabi)
{
    switch (osabi)
    {
    case ELFOSABI_SYSV:
        return "SYSV";
    case ELFOSABI_HPUX:
        return "HPUX";
    case ELFOSABI_NETBSD:
        return "NETBSD";
    case ELFOSABI_GNU:
        return "GNU/LINUX";
    //case ELFOSABI_LINUX:
    //    return "LINUX";
    case ELFOSABI_SOLARIS:
        return "SOLARIS";
    case ELFOSABI_AIX:
        return "AIX";
    case ELFOSABI_IRIX:
        return "IRIX";
    case ELFOSABI_FREEBSD:
        return "FREEBSD";
    case ELFOSABI_TRU64:
        return "TRU64";
    case ELFOSABI_MODESTO:
        return "MODESTO";
    case ELFOSABI_OPENBSD:
        return "OPENBSD";
    case ELFOSABI_ARM_AEABI:
        return "ARM_AEABI";
    case ELFOSABI_ARM:
        return "ARM";
    case ELFOSABI_STANDALONE:
        return "STANDALONE";
    default:
        osabi = ELFOSABI_NONE;
        return "NONE";
    }
}

QString Elf::parseHeaderType(uint16_t& type)
{
    switch (type)
    {
    case ET_REL:
        return "REL";
    case ET_EXEC:
        return "EXEC";
    case ET_DYN:
        return "DYN";
    case ET_CORE:
        return "CORE";
    default:
        if ((type & 0xFF00) == 0xFE00)
            return "OS-SPECIFIC";
        else if ((type & 0xFF00) == 0xFF00)
            return "PROCESSOR-SPECIFIC";
        else
        {
            type = ET_NONE;
            return "NONE";
        }
    }
}

QString Elf::parseHeaderMachine(uint16_t& machine)
{
    switch (machine)
    {
    case EM_M32:
        return "M32";
    case EM_SPARC:
        return "SPARC";
    case EM_386:
        return "386";
    case EM_68K:
        return "68K";
    case EM_88K:
        return "88K";
    case EM_860:
        return "860";
    case EM_MIPS:
        return "MIPS";
    case EM_S370:
        return "S370";
    case EM_MIPS_RS3_LE:
        return "MIPS_RS3_LE";
    case EM_PARISC:
        return "PARISC";
    case EM_VPP500:
        return "VPP500";
    case EM_SPARC32PLUS:
        return "SPARC32PLUS";
    case EM_960:
        return "960";
    case EM_PPC:
        return "PPC";
    case EM_PPC64:
        return "PPC64";
    case EM_S390:
        return "S390";
    case EM_V800:
        return "V800";
    case EM_FR20:
        return "FR20";
    case EM_RH32:
        return "RH32";
    case EM_RCE:
        return "RCE";
    case EM_ARM:
        return "ARM";
    case EM_FAKE_ALPHA:
        return "FAKE_ALPHA";
    case EM_SH:
        return "SH";
    case EM_SPARCV9:
        return "SPARCV9";
    case EM_TRICORE:
        return "TRICORE";
    case EM_ARC:
        return "ARC";
    case EM_H8_300:
        return "H8_300";
    case EM_H8_300H:
        return "H8_300H";
    case EM_H8S:
        return "H8S";
    case EM_H8_500:
        return "H8_500";
    case EM_IA_64:
        return "IA_64";
    case EM_MIPS_X:
        return "MIPS_X";
    case EM_COLDFIRE:
        return "COLDFIRE";
    case EM_68HC12:
        return "68HC12";
    case EM_MMA:
        return "MMA";
    case EM_PCP:
        return "PCP";
    case EM_NCPU:
        return "NCPU";
    case EM_NDR1:
        return "NDR1";
    case EM_STARCORE:
        return "STARCORE";
    case EM_ME16:
        return "ME16";
    case EM_ST100:
        return "ST100";
    case EM_TINYJ:
        return "TINYJ";
    case EM_X86_64:
        return "X86_64";
    case EM_PDSP:
        return "PDSP";
    case EM_FX66:
        return "FX66";
    case EM_ST9PLUS:
        return "ST9PLUS";
    case EM_ST7:
        return "ST7";
    case EM_68HC16:
        return "68HC16";
    case EM_68HC11:
        return "68HC11";
    case EM_68HC08:
        return "68HC08";
    case EM_68HC05:
        return "68HC05";
    case EM_SVX:
        return "SVX";
    case EM_ST19:
        return "ST19";
    case EM_VAX:
        return "VAX";
    case EM_CRIS:
        return "CRIS";
    case EM_JAVELIN:
        return "JAVELIN";
    case EM_FIREPATH:
        return "FIREPATH";
    case EM_ZSP:
        return "ZSP";
    case EM_MMIX:
        return "MMIX";
    case EM_HUANY:
        return "HUANY";
    case EM_PRISM:
        return "PRISM";
    case EM_AVR:
        return "AVR";
    case EM_FR30:
        return "FR30";
    case EM_D10V:
        return "D10V";
    case EM_D30V:
        return "D30V";
    case EM_V850:
        return "V850";
    case EM_M32R:
        return "M32R";
    case EM_MN10300:
        return "MN10300";
    case EM_MN10200:
        return "MN10200";
    case EM_PJ:
        return "PJ";
    case EM_OPENRISC:
        return "OPENRISC";
    case EM_ARC_A5:
        return "ARC_A5";
    case EM_XTENSA:
        return "XTENSA";
    default:
        machine = EM_NONE;
        return "NONE";
    }
}

QString Elf::parseHeaderMachineVendor(const uint16_t& machine)
{
    switch (machine)
    {
    case EM_M32:
        return "AT&T";
    case EM_SPARC:
    case EM_SPARC32PLUS:
        return "Sun";
    case EM_386:
    case EM_860:
    case EM_960:
    case EM_IA_64:
        return "Intel";
    case EM_68K:
    case EM_88K:
    case EM_RCE:
    case EM_COLDFIRE:
    case EM_68HC12:
    case EM_STARCORE:
    case EM_68HC16:
    case EM_68HC11:
    case EM_68HC08:
    case EM_68HC05:
        return "Motorola";
    case EM_S370:
    case EM_S390:
        return "IBM";
    case EM_VPP500:
    case EM_FR20:
    case EM_MMA:
    case EM_FR30:
        return "Fujitsu";
    case EM_SH:
    case EM_H8_300:
    case EM_H8_300H:
    case EM_H8S:
    case EM_H8_500:
        return "Hitachi";
    case EM_TRICORE:
    case EM_PCP:
    case EM_FX66:
        return "Siemens";
    case EM_MIPS_X:
        return "Stanford";
    case EM_NCPU:
    case EM_PDSP:
        return "Sony";
    case EM_ME16:
        return "Toyota";
    case EM_ST100:
    case EM_ST9PLUS:
    case EM_ST7:
    case EM_ST19:
        return "STMicroelectronics";
    case EM_TINYJ:
        return "Advanced Logic Corp.";
    case EM_X86_64:
        return "AMD";
    case EM_SVX:
        return "Silicon Graphics";
    case EM_JAVELIN:
        return "Infineon Technologies";
    case EM_MMIX:
        return "Donald Knuth";
    case EM_HUANY:
        return "Harvard University";
    case EM_D10V:
    case EM_D30V:
    case EM_M32R:
        return "Mitsubishi";
    case EM_MN10300:
    case EM_MN10200:
        return "Matsushita";
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
    case EM_PARISC:
    case EM_PPC:
    case EM_PPC64:
    case EM_V800:
    case EM_RH32:
    case EM_ARM:
    case EM_FAKE_ALPHA:
    case EM_SPARCV9:
    case EM_ARC:
    case EM_NDR1:
    case EM_VAX:
    case EM_CRIS:
    case EM_FIREPATH:
    case EM_ZSP:
    case EM_PRISM:
    case EM_AVR:
    case EM_V850:
    case EM_PJ:
    case EM_OPENRISC:
    case EM_ARC_A5:
    case EM_XTENSA:
    default:
        return "UNKNOWN";
    }
}

}
}
