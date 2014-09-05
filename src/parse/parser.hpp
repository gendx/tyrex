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

#ifndef TYREX_PARSER_HPP
#define TYREX_PARSER_HPP

#include "misc/memchunk.hpp"
#include "misc/shared.hpp"

namespace tyrex {
namespace parse {

template <typename inputT, typename outputT>
class Parser
{
public:
    bool parse(const inputT& in, outputT& out);

protected:
    virtual void doParse(const inputT& in, outputT& out) = 0;
};


template <typename outputT>
class MemchunkParser : public Parser<MemChunk, outputT>
{
};


template <typename dataT>
class DataParser : public MemchunkParser<Shared<dataT> >
{
public:
    bool parse(const MemChunk& in, Shared<dataT>& out);

protected:
    virtual void onError(const MemChunk& in, Shared<dataT>& out) = 0;
};

}
}

#endif // TYREX_PARSER_HPP
