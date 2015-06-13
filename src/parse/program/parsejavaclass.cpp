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

#include "parsejavaclass.hpp"

namespace tyrex {
namespace parse {

unsigned char JavaClass::mMagic[4] = {0xCA, 0xFE, 0xBA, 0xBE};


void JavaClass::onError(const MemChunk& chunk, std::shared_ptr<data::JavaClass>& data)
{
    data = std::make_shared<data::JavaClass>(chunk, mSrcColorizer, data::Table(), data::Table());
}

void JavaClass::doParse(const MemChunk& chunk, std::shared_ptr<data::JavaClass>& data)
{
    unsigned int chunkSize = chunk.size();

    // check magic
    if (chunkSize < 4)
        Except::reportError(chunkSize, "javaclass, magic", "unexpected end of data");
    if (chunk.uncompare(JavaClass::mMagic, 4))
        Except::reportError(0, "javaclass, magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 4, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(4, 2);

    data::Table properties("Properties", QStringList() << "Property" << "Value");

    // version
    if (chunkSize < 8)
        Except::reportError(chunkSize, "javaclass, version", "unexpected end of data");

    unsigned int minorVersion = chunk.getUint16BE(4);
    unsigned int majorVersion = chunk.getUint16BE(6);
    properties.push("Version", QString::number(majorVersion) + "." + QString::number(minorVersion));

    mSrcColorizer.addHighlight(4, 4, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(6, 1);
    mSrcColorizer.addSeparation(8, 2);

    // constant pool
    if (chunkSize < 10)
        Except::reportError(chunkSize, "javaclass, constant pool, size", "unexpected end of data");
    unsigned int poolSize = chunk.getUint16BE(8);

    unsigned int processed = 10;

    data::Table constantPool("Constant pool", QStringList() << "Index" << "Type" << "Value");

    constantPool.push("0x0", "", "");
    unsigned int numItems = 1;

    for (unsigned int i = 1 ; i < poolSize ; ++i)
    {
        if (!Util::checkRange(processed, 1, chunkSize))
            Except::reportError(chunkSize, "javaclass, constant pool", "unexpected end of data");
        unsigned char tag = chunk[processed];

        unsigned int size;
        switch (tag)
        {
        case data::java::CONSTANT_Class:
        case data::java::CONSTANT_String:
        case data::java::CONSTANT_MethodType:
            size = 3;
            break;
        case data::java::CONSTANT_MethodHandle:
            size = 4;
            break;
        case data::java::CONSTANT_Fieldref:
        case data::java::CONSTANT_Methodref:
        case data::java::CONSTANT_InterfaceMethodref:
        case data::java::CONSTANT_Integer:
        case data::java::CONSTANT_Float:
        case data::java::CONSTANT_NameAndType:
        case data::java::CONSTANT_InvokeDynamic:
            size = 5;
            break;
        case data::java::CONSTANT_Long:
        case data::java::CONSTANT_Double:
            size = 9;
            break;
        case data::java::CONSTANT_Utf8:
            if (!Util::checkRange(processed, 3, chunkSize))
                Except::reportError(chunkSize, "javaclass, constant pool item, utf8", "unexpected end of data");
            {
                unsigned int length = chunk.getUint16BE(processed + 1);
                size = 3 + length;
            }
            break;
        default:
            Except::reportError(chunkSize, "javaclass, constant pool item", "unknown item type");
        }

        unsigned short val1;
        unsigned short val2;
        QString value;
        switch (tag)
        {
        case data::java::CONSTANT_Class:
        case data::java::CONSTANT_String:
        case data::java::CONSTANT_MethodType:
            val1 = chunk.getUint16BE(processed + 1);
            value = "0x" + QString::number(val1, 16);
            break;
        case data::java::CONSTANT_MethodHandle:
            val1 = chunk[processed + 1];
            val2 = chunk.getUint16BE(processed + 2);
            value = "0x" + QString::number(val1, 16) + ", " + "0x" + QString::number(val2, 16);
            break;
        case data::java::CONSTANT_Fieldref:
        case data::java::CONSTANT_Methodref:
        case data::java::CONSTANT_InterfaceMethodref:
        case data::java::CONSTANT_NameAndType:
        case data::java::CONSTANT_InvokeDynamic:
            val1 = chunk.getUint16BE(processed + 1);
            val2 = chunk.getUint16BE(processed + 3);
            value = "0x" + QString::number(val1, 16) + ", " + "0x" + QString::number(val2, 16);
            break;
        case data::java::CONSTANT_Integer:
            value = QString::number(chunk.getInt32BE(processed + 1));
            break;
        case data::java::CONSTANT_Float:
            value = QString::number(chunk.getFloatBE(processed + 1));
            break;
        case data::java::CONSTANT_Long:
            value = QString::number(chunk.getInt64BE(processed + 1));
            break;
        case data::java::CONSTANT_Double:
            value = QString::number(chunk.getDoubleBE(processed + 1));
            break;
        case data::java::CONSTANT_Utf8:
            {
                unsigned int length = chunk.getUint16BE(processed + 1);
                value = QString::fromUtf8(reinterpret_cast<const char*>(chunk.subChunk(processed + 3, length).data()), length);
            }
        }

        QString tagName;
        switch (tag)
        {
        case data::java::CONSTANT_Class:
            tagName = "Class"; break;
        case data::java::CONSTANT_String:
            tagName = "String"; break;
        case data::java::CONSTANT_MethodType:
            tagName = "MethodType"; break;
        case data::java::CONSTANT_MethodHandle:
            tagName = "MethodHandle"; break;
        case data::java::CONSTANT_Fieldref:
            tagName = "Fieldref"; break;
        case data::java::CONSTANT_Methodref:
            tagName = "Methodref"; break;
        case data::java::CONSTANT_InterfaceMethodref:
            tagName = "InterfaceMethodref"; break;
        case data::java::CONSTANT_Integer:
            tagName = "Integer"; break;
        case data::java::CONSTANT_Float:
            tagName = "Float"; break;
        case data::java::CONSTANT_NameAndType:
            tagName = "NameAndType"; break;
        case data::java::CONSTANT_InvokeDynamic:
            tagName = "InvokeDynamic"; break;
        case data::java::CONSTANT_Long:
            tagName = "Long"; break;
        case data::java::CONSTANT_Double:
            tagName = "Double"; break;
        case data::java::CONSTANT_Utf8:
            tagName = "Utf8"; break;
        }

        constantPool.push("0x" + QString::number(numItems, 16), tagName, value);

        if (!Util::checkRange(processed, size, chunkSize))
            Except::reportError(chunkSize, "javaclass, constant pool item", "unexpected end of data");

        ++numItems;
        mSrcColorizer.addSeparation(processed, 1);
        if (tag == data::java::CONSTANT_Long || tag == data::java::CONSTANT_Double)
        {
            ++i;
            ++numItems;
        }

        processed += size;
    }

    mSrcColorizer.addHighlight(8, processed - 8, QColor(128, 255, 0, 64));
    mSrcColorizer.addSeparation(processed, 2);

    data = std::make_shared<data::JavaClass>(chunk, mSrcColorizer, constantPool, properties);
}

}
}
