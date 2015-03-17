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

#include "parsedocument.hpp"

#include "graphic/dialog/typeselector.hpp"
#include "graphic/dialog/inputdialog.hpp"
#include "parse/archive/tar.hpp"
#include "parse/archive/zip.hpp"
#include "parse/compress/bzip2.hpp"
#include "parse/compress/deflate/deflate.hpp"
#include "parse/compress/deflate/gzip.hpp"
#include "parse/compress/deflate/zlib.hpp"
#include "parse/compress/lzma/lzma.hpp"
#include "parse/compress/lzma/lzma2.hpp"
#include "parse/font/truetype.hpp"
#include "parse/image/png.hpp"
#include "parse/program/parseelf.tpl"
#include "parse/program/parsejavaclass.hpp"

namespace tyrex {
namespace parse {

Document::Document(QWidget* parent) :
    mParent(parent)
{
}


std::shared_ptr<data::Data> Document::parse(MemChunk source, QWidget* parent)
{
    Document p(parent);
    std::shared_ptr<data::Data> data;
    p.parse(source, data);
    return data;
}


void Document::onError(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    data = std::make_shared<data::ByteSequence>(chunk);
}

void Document::doParse(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    static const TypeToParser typeToParser {
        {"archive/tar", &Document::parseArchiveTar},
        {"archive/zip", &Document::parseArchiveZip},
        {"compress/bzip2", &Document::parseCompressBzip2},
        {"compress/deflate", &Document::parseCompressDeflate},
        {"compress/gzip", &Document::parseCompressGzip},
        {"compress/lzma", &Document::parseCompressLzma},
        {"compress/lzma2", &Document::parseCompressLzma2},
        {"compress/zlib", &Document::parseCompressZlib},
        {"font/truetype", &Document::parseFontTruetype},
        {"image/png", &Document::parseImagePng},
        {"program/elf32", &Document::parseProgramElf32},
        {"program/elf64", &Document::parseProgramElf64},
        {"program/java", &Document::parseProgramJava}
    };

    QStringList types = Document::findTypes(chunk);
    bool cancelled;
    QString type = graphic::TypeSelector::select(mParent, types, cancelled);
    if (cancelled)
        return;

    auto found = typeToParser.find(type);
    if (found != typeToParser.end() && (this->*found->second)(chunk, data))
        return;

    data = std::make_shared<data::ByteSequence>(chunk);
}


QStringList Document::findTypes(const MemChunk& chunk)
{
    static const std::vector<std::pair<std::vector<unsigned char>, QString> > magics {
        {{0x1F, 0x8B}, "compress/gzip"},
        {{0x42, 0x5a, 0x68}, "compress/bzip2"},
        {{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, "image/png"},
        {{0x7F, 0x45, 0x4C, 0x46, 0x01}, "program/elf32"},
        {{0x7F, 0x45, 0x4C, 0x46, 0x02}, "program/elf64"},
        {{0xCA, 0xFE, 0xBA, 0xBE}, "program/java"},
        {{0x00, 0x01, 0x00, 0x00}, "font/truetype"},
        {{0x74, 0x72, 0x75, 0x65}, "font/truetype"},
        //{{0x4D, 0x54, 0x68, 0x64}, "audio/midi"},
        {{0x50, 0x4B, 0x03, 0x04}, "archive/zip"}//,
        //{{0x3C, 0x3F, 0x78, 0x6D, 0x6C}, "document/xml"},
        //{{0x25, 0x50, 0x44, 0x46, 0x2D}, "document/pdf"},
        //{{0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00}, "compress/xz"},
        //{{0x46, 0x57, 0x53}, "animation/flash"},
        //{{0x43, 0x57, 0x53}, "animation/flash"},
        //{{0x5A, 0x57, 0x53}, "animation/flash"}
    };

    QStringList result;
    for (auto& magic : magics)
        if (!chunk.uncompare(magic.first))
            result.append(magic.second);

    return result;
}


bool Document::parseArchiveTar(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Tar tar;
    std::shared_ptr<data::Archive> parsedData;

    if (tar.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseArchiveZip(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Zip zip;
    std::shared_ptr<data::Archive> parsedData;

    if (zip.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseCompressBzip2(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Bzip2 bzip2;
    std::shared_ptr<data::Compress> parsedData;

    if (bzip2.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseCompressDeflate(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    unsigned int window;
    if (graphic::InputDialog::getUint(window, "Size of decompression window", mParent))
    {
        Deflate deflate(window);
        std::shared_ptr<data::Compress> parsedData;

        if (deflate.parse(chunk, parsedData))
        {
            data = parsedData;
            return true;
        }
    }

    return false;
}

bool Document::parseCompressGzip(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Gzip gzip;
    std::shared_ptr<data::Compress> parsedData;

    if (gzip.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseCompressLzma(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Lzma lzma;
    std::shared_ptr<data::Compress> parsedData;

    if (lzma.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseCompressLzma2(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Lzma2 lzma2;
    std::shared_ptr<data::Compress> parsedData;

    if (lzma2.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseCompressZlib(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Zlib zlib;
    std::shared_ptr<data::Compress> parsedData;

    if (zlib.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseFontTruetype(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Truetype ttf;
    std::shared_ptr<data::Font> parsedData;

    if (ttf.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseImagePng(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Png png;
    std::shared_ptr<data::Image> parsedData;

    if (png.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseProgramElf32(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Elf32 elf32;
    std::shared_ptr<data::Elf> parsedData;

    if (elf32.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseProgramElf64(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    Elf64 elf64;
    std::shared_ptr<data::Elf> parsedData;

    if (elf64.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

bool Document::parseProgramJava(const MemChunk& chunk, std::shared_ptr<data::Data>& data)
{
    JavaClass javaClass;
    std::shared_ptr<data::JavaClass> parsedData;

    if (javaClass.parse(chunk, parsedData))
    {
        data = parsedData;
        return true;
    }

    return false;
}

}
}
