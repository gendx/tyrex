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

#include "elf32header.hpp"

#include <cstring>

namespace tyrex {
namespace parse {

Elf32Header::Elf32Header(const MemChunk& chunk, data::Table& properties)
{
    std::memcpy(static_cast<void*>(&mHeader), chunk.data(), 52);

    switch (mHeader.e_ident[EI_DATA])
    {
    case ELFDATA2LSB:
        properties.push("ELFDATA", "2LSB"); break;
    case ELFDATA2MSB:
        properties.push("ELFDATA", "2MSB"); break;
    default:
        mHeader.e_ident[EI_DATA] = ELFDATANONE;
        properties.push("ELFDATA", "NONE");
    }

    switch (mHeader.e_ident[EI_VERSION])
    {
    case EV_CURRENT:
        properties.push("ELFVERSION", "CURRENT"); break;
    default:
        mHeader.e_ident[EI_VERSION] = EV_NONE;
        properties.push("ELFVERSION", "NONE");
    }

    switch (mHeader.e_ident[EI_OSABI])
    {
    case ELFOSABI_SYSV:
        properties.push("ELFOSABI", "SYSV"); break;
    case ELFOSABI_HPUX:
        properties.push("ELFOSABI", "HPUX"); break;
    case ELFOSABI_NETBSD:
        properties.push("ELFOSABI", "NETBSD");  break;
    case ELFOSABI_GNU:
        properties.push("ELFOSABI", "GNU/LINUX"); break;
    //case ELFOSABI_LINUX:
    //    properties.push("ELFOSABI", "LINUX"); break;
    case ELFOSABI_SOLARIS:
        properties.push("ELFOSABI", "SOLARIS"); break;
    case ELFOSABI_AIX:
        properties.push("ELFOSABI", "AIX"); break;
    case ELFOSABI_IRIX:
        properties.push("ELFOSABI", "IRIX"); break;
    case ELFOSABI_FREEBSD:
        properties.push("ELFOSABI", "FREEBSD"); break;
    case ELFOSABI_TRU64:
        properties.push("ELFOSABI", "TRU64"); break;
    case ELFOSABI_MODESTO:
        properties.push("ELFOSABI", "MODESTO"); break;
    case ELFOSABI_OPENBSD:
        properties.push("ELFOSABI", "OPENBSD"); break;
    case ELFOSABI_ARM_AEABI:
        properties.push("ELFOSABI", "ARM_AEABI"); break;
    case ELFOSABI_ARM:
        properties.push("ELFOSABI", "ARM"); break;
    case ELFOSABI_STANDALONE:
        properties.push("ELFOSABI", "STANDALONE"); break;
    default:
        mHeader.e_ident[EI_OSABI] = ELFOSABI_NONE;
        properties.push("ELFOSABI", "NONE");
    }

    properties.push("ELFABIVERSION", QString::number(mHeader.e_ident[EI_ABIVERSION]));

    switch (mHeader.e_type)
    {
    case ET_REL:
        properties.push("ELFTYPE", "REL"); break;
    case ET_EXEC:
        properties.push("ELFTYPE", "EXEC"); break;
    case ET_DYN:
        properties.push("ELFTYPE", "DYN"); break;
    case ET_CORE:
        properties.push("ELFTYPE", "CORE"); break;
    default:
        if ((mHeader.e_type & 0xFF00) == 0xFE00)
            properties.push("ELFTYPE", "OS-SPECIFIC");
        else if ((mHeader.e_type & 0xFF00) == 0xFF00)
            properties.push("ELFTYPE", "PROCESSOR-SPECIFIC");
        else
        {
            mHeader.e_type = ET_NONE;
            properties.push("ELFTYPE", "NONE");
        }
    }

    switch (mHeader.e_machine)
    {
    case EM_M32:
        properties.push("ELFMACHINE", "M32"); break;
    case EM_SPARC:
        properties.push("ELFMACHINE", "SPARC"); break;
    case EM_386:
        properties.push("ELFMACHINE", "386"); break;
    case EM_68K:
        properties.push("ELFMACHINE", "68K"); break;
    case EM_88K:
        properties.push("ELFMACHINE", "88K"); break;
    case EM_860:
        properties.push("ELFMACHINE", "860"); break;
    case EM_MIPS:
        properties.push("ELFMACHINE", "MIPS"); break;
    case EM_S370:
        properties.push("ELFMACHINE", "S370"); break;
    case EM_MIPS_RS3_LE:
        properties.push("ELFMACHINE", "MIPS_RS3_LE"); break;
    case EM_PARISC:
        properties.push("ELFMACHINE", "PARISC"); break;
    case EM_VPP500:
        properties.push("ELFMACHINE", "VPP500"); break;
    case EM_SPARC32PLUS:
        properties.push("ELFMACHINE", "SPARC32PLUS"); break;
    case EM_960:
        properties.push("ELFMACHINE", "960"); break;
    case EM_PPC:
        properties.push("ELFMACHINE", "PPC"); break;
    case EM_PPC64:
        properties.push("ELFMACHINE", "PPC64"); break;
    case EM_S390:
        properties.push("ELFMACHINE", "S390"); break;
    case EM_V800:
        properties.push("ELFMACHINE", "V800"); break;
    case EM_FR20:
        properties.push("ELFMACHINE", "FR20"); break;
    case EM_RH32:
        properties.push("ELFMACHINE", "RH32"); break;
    case EM_RCE:
        properties.push("ELFMACHINE", "RCE"); break;
    case EM_ARM:
        properties.push("ELFMACHINE", "ARM"); break;
    case EM_FAKE_ALPHA:
        properties.push("ELFMACHINE", "FAKE_ALPHA"); break;
    case EM_SH:
        properties.push("ELFMACHINE", "SH"); break;
    case EM_SPARCV9:
        properties.push("ELFMACHINE", "SPARCV9"); break;
    case EM_TRICORE:
        properties.push("ELFMACHINE", "TRICORE"); break;
    case EM_ARC:
        properties.push("ELFMACHINE", "ARC"); break;
    case EM_H8_300:
        properties.push("ELFMACHINE", "H8_300"); break;
    case EM_H8_300H:
        properties.push("ELFMACHINE", "H8_300H"); break;
    case EM_H8S:
        properties.push("ELFMACHINE", "H8S"); break;
    case EM_H8_500:
        properties.push("ELFMACHINE", "H8_500"); break;
    case EM_IA_64:
        properties.push("ELFMACHINE", "IA_64"); break;
    case EM_MIPS_X:
        properties.push("ELFMACHINE", "MIPS_X"); break;
    case EM_COLDFIRE:
        properties.push("ELFMACHINE", "COLDFIRE"); break;
    case EM_68HC12:
        properties.push("ELFMACHINE", "68HC12"); break;
    case EM_MMA:
        properties.push("ELFMACHINE", "MMA"); break;
    case EM_PCP:
        properties.push("ELFMACHINE", "PCP"); break;
    case EM_NCPU:
        properties.push("ELFMACHINE", "NCPU"); break;
    case EM_NDR1:
        properties.push("ELFMACHINE", "NDR1"); break;
    case EM_STARCORE:
        properties.push("ELFMACHINE", "STARCORE"); break;
    case EM_ME16:
        properties.push("ELFMACHINE", "ME16"); break;
    case EM_ST100:
        properties.push("ELFMACHINE", "ST100"); break;
    case EM_TINYJ:
        properties.push("ELFMACHINE", "TINYJ"); break;
    case EM_X86_64:
        properties.push("ELFMACHINE", "X86_64"); break;
    case EM_PDSP:
        properties.push("ELFMACHINE", "PDSP"); break;
    case EM_FX66:
        properties.push("ELFMACHINE", "FX66"); break;
    case EM_ST9PLUS:
        properties.push("ELFMACHINE", "ST9PLUS"); break;
    case EM_ST7:
        properties.push("ELFMACHINE", "ST7"); break;
    case EM_68HC16:
        properties.push("ELFMACHINE", "68HC16"); break;
    case EM_68HC11:
        properties.push("ELFMACHINE", "68HC11"); break;
    case EM_68HC08:
        properties.push("ELFMACHINE", "68HC08"); break;
    case EM_68HC05:
        properties.push("ELFMACHINE", "68HC05"); break;
    case EM_SVX:
        properties.push("ELFMACHINE", "SVX"); break;
    case EM_ST19:
        properties.push("ELFMACHINE", "ST19"); break;
    case EM_VAX:
        properties.push("ELFMACHINE", "VAX"); break;
    case EM_CRIS:
        properties.push("ELFMACHINE", "CRIS"); break;
    case EM_JAVELIN:
        properties.push("ELFMACHINE", "JAVELIN"); break;
    case EM_FIREPATH:
        properties.push("ELFMACHINE", "FIREPATH"); break;
    case EM_ZSP:
        properties.push("ELFMACHINE", "ZSP"); break;
    case EM_MMIX:
        properties.push("ELFMACHINE", "MMIX"); break;
    case EM_HUANY:
        properties.push("ELFMACHINE", "HUANY"); break;
    case EM_PRISM:
        properties.push("ELFMACHINE", "PRISM"); break;
    case EM_AVR:
        properties.push("ELFMACHINE", "AVR"); break;
    case EM_FR30:
        properties.push("ELFMACHINE", "FR30"); break;
    case EM_D10V:
        properties.push("ELFMACHINE", "D10V"); break;
    case EM_D30V:
        properties.push("ELFMACHINE", "D30V"); break;
    case EM_V850:
        properties.push("ELFMACHINE", "V850"); break;
    case EM_M32R:
        properties.push("ELFMACHINE", "M32R"); break;
    case EM_MN10300:
        properties.push("ELFMACHINE", "MN10300"); break;
    case EM_MN10200:
        properties.push("ELFMACHINE", "MN10200"); break;
    case EM_PJ:
        properties.push("ELFMACHINE", "PJ"); break;
    case EM_OPENRISC:
        properties.push("ELFMACHINE", "OPENRISC"); break;
    case EM_ARC_A5:
        properties.push("ELFMACHINE", "ARC_A5"); break;
    case EM_XTENSA:
        properties.push("ELFMACHINE", "XTENSA"); break;
    default:
        mHeader.e_machine = EM_NONE;
        properties.push("ELFMACHINE", "NONE");
    }

    switch (mHeader.e_machine)
    {
    case EM_M32:
        properties.push("MACHINEVENDOR", "AT&T"); break;
    case EM_SPARC:
    case EM_SPARC32PLUS:
        properties.push("MACHINEVENDOR", "Sun"); break;
    case EM_386:
    case EM_860:
    case EM_960:
    case EM_IA_64:
        properties.push("MACHINEVENDOR", "Intel"); break;
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
        properties.push("MACHINEVENDOR", "Motorola"); break;
    case EM_S370:
    case EM_S390:
        properties.push("MACHINEVENDOR", "IBM"); break;
    case EM_VPP500:
    case EM_FR20:
    case EM_MMA:
    case EM_FR30:
        properties.push("MACHINEVENDOR", "Fujitsu"); break;
    case EM_SH:
    case EM_H8_300:
    case EM_H8_300H:
    case EM_H8S:
    case EM_H8_500:
        properties.push("MACHINEVENDOR", "Hitachi"); break;
    case EM_TRICORE:
    case EM_PCP:
    case EM_FX66:
        properties.push("MACHINEVENDOR", "Siemens"); break;
    case EM_MIPS_X:
        properties.push("MACHINEVENDOR", "Stanford"); break;
    case EM_NCPU:
    case EM_PDSP:
        properties.push("MACHINEVENDOR", "Sony"); break;
    case EM_ME16:
        properties.push("MACHINEVENDOR", "Toyota"); break;
    case EM_ST100:
    case EM_ST9PLUS:
    case EM_ST7:
    case EM_ST19:
        properties.push("MACHINEVENDOR", "STMicroelectronics"); break;
    case EM_TINYJ:
        properties.push("MACHINEVENDOR", "Advanced Logic Corp."); break;
    case EM_X86_64:
        properties.push("MACHINEVENDOR", "AMD"); break;
    case EM_SVX:
        properties.push("MACHINEVENDOR", "Silicon Graphics"); break;
    case EM_JAVELIN:
        properties.push("MACHINEVENDOR", "Infineon Technologies"); break;
    case EM_MMIX:
        properties.push("MACHINEVENDOR", "Donald Knuth"); break;
    case EM_HUANY:
        properties.push("MACHINEVENDOR", "Harvard University"); break;
    case EM_D10V:
    case EM_D30V:
    case EM_M32R:
        properties.push("MACHINEVENDOR", "Mitsubishi"); break;
    case EM_MN10300:
    case EM_MN10200:
        properties.push("MACHINEVENDOR", "Matsushita"); break;
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
        properties.push("MACHINEVENDOR", "UNKNOWN"); break;
    }

    switch (mHeader.e_version)
    {
    case EV_CURRENT:
        properties.push("ELFVERSION", "CURRENT"); break;
    default:
        mHeader.e_version = EV_NONE;
        properties.push("ELFVERSION", "NONE");
    }
}

}
}
