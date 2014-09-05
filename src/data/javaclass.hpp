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

#ifndef TYREX_DATA_JAVACLASS_HPP
#define TYREX_DATA_JAVACLASS_HPP

#include "bytesequence.hpp"
#include "table.hpp"

namespace tyrex {
namespace data {

namespace java {
enum ConstantPoolTag {
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_String = 8,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_NameAndType = 12,
    CONSTANT_Utf8 = 1,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
};
}

class JavaClass : public Data
{
public:
    JavaClass(const MemChunk& srcChunk, const Colorizer& srcColorizer, const Table& constantPool, const Table& properties);

    void appendToTree(graphic::TreeNodeModel& tree) const;

private:
    ByteSequence mSource;
    Table mConstantPool;
    Table mProperties;
};

}
}

#endif // TYREX_DATA_JAVACLASS_HPP
