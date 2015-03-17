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

#include "elfheader.hpp"

#include <cstring>
#include "parseelf.hpp"

namespace tyrex {
namespace parse {

Elf32Header::Elf32Header(const MemChunk& chunk, data::Table& properties)
{
    std::memcpy(static_cast<void*>(&mHeader), chunk.data(), 52);

    properties.push("ELFDATA", Elf::parseHeaderData(mHeader.e_ident[EI_DATA]));
    properties.push("ELFVERSION", Elf::parseHeaderVersion(mHeader.e_ident[EI_VERSION]));
    properties.push("ELFOSABI", Elf::parseHeaderOSABI(mHeader.e_ident[EI_OSABI]));
    properties.push("ELFABIVERSION", QString::number(mHeader.e_ident[EI_ABIVERSION]));
    properties.push("ELFTYPE", Elf::parseHeaderType(mHeader.e_type));
    properties.push("ELFMACHINE", Elf::parseHeaderMachine(mHeader.e_machine));
    properties.push("MACHINEVENDOR", Elf::parseHeaderMachineVendor(mHeader.e_machine));
    properties.push("ELFVERSION", Elf::parseHeaderVersion2(mHeader.e_version));
}

Elf64Header::Elf64Header(const MemChunk& chunk, data::Table& properties)
{
    std::memcpy(static_cast<void*>(&mHeader), chunk.data(), 64);

    properties.push("ELFDATA", Elf::parseHeaderData(mHeader.e_ident[EI_DATA]));
    properties.push("ELFVERSION", Elf::parseHeaderVersion(mHeader.e_ident[EI_VERSION]));
    properties.push("ELFOSABI", Elf::parseHeaderOSABI(mHeader.e_ident[EI_OSABI]));
    properties.push("ELFABIVERSION", QString::number(mHeader.e_ident[EI_ABIVERSION]));
    properties.push("ELFTYPE", Elf::parseHeaderType(mHeader.e_type));
    properties.push("ELFMACHINE", Elf::parseHeaderMachine(mHeader.e_machine));
    properties.push("MACHINEVENDOR", Elf::parseHeaderMachineVendor(mHeader.e_machine));
    properties.push("ELFVERSION", Elf::parseHeaderVersion2(mHeader.e_version));
}

}
}
