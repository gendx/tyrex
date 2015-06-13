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

    static std::shared_ptr<data::Data> parse(MemChunk source, QWidget* parent);

private:
    Document(QWidget* parent);

    void doParse(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void onError(const MemChunk& chunk, std::shared_ptr<data::Data>& data);

    QStringList findTypes(const MemChunk& chunk);

    typedef std::map<QString, void (Document::*)(const MemChunk& chunk, std::shared_ptr<data::Data>& data)> TypeToParser;

    void parseArchiveTar(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseArchiveZip(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressBzip2(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressDeflate(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressGzip(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressLzma(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressLzma2(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressXz(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseCompressZlib(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseFontTruetype(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseImagePng(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseProgramElf32(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseProgramElf64(const MemChunk& chunk, std::shared_ptr<data::Data>& data);
    void parseProgramJava(const MemChunk& chunk, std::shared_ptr<data::Data>& data);

    QWidget* mParent;
};

}
}

#endif // TYREX_PARSE_DOCUMENT_HPP
