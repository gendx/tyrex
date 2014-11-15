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

#ifndef TYREX_PARSE_DOCUMENT_HPP
#define TYREX_PARSE_DOCUMENT_HPP

#include "parser.tpl"
#include "data/data.hpp"

namespace tyrex {
namespace parse {

class Document : public DataParser<data::Data>
{
public:
    using DataParser<data::Data>::parse;

    static Shared<data::Data> parse(MemChunk source, QWidget* parent);

private:
    Document(QWidget* parent);

    void doParse(const MemChunk& chunk, Shared<data::Data>& data);
    void onError(const MemChunk& chunk, Shared<data::Data>& data);

    QStringList findTypes(const MemChunk& chunk);

    typedef std::map<QString, bool (Document::*)(const MemChunk& chunk, Shared<data::Data>& data)> TypeToParser;

    bool parseArchiveTar(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseArchiveZip(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseCompressBzip2(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseCompressDeflate(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseCompressGzip(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseCompressLzma(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseCompressLzma2(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseCompressZlib(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseFontTruetype(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseImagePng(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseProgramElf32(const MemChunk& chunk, Shared<data::Data>& data);
    bool parseProgramJava(const MemChunk& chunk, Shared<data::Data>& data);

    QWidget* mParent;
};

}
}

#endif // TYREX_PARSE_DOCUMENT_HPP
