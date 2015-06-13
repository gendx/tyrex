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

#ifndef TYREX_PARSER_TPL
#define TYREX_PARSER_TPL

#include "parser.hpp"

#include "graphic/console.hpp"
#include "graphic/mainwindow.hpp"
#include "parseexception.hpp"
#include <iostream>

namespace tyrex {
namespace parse {

template <typename inputT, typename outputT>
bool Parser<inputT, outputT>::parse(const inputT& in, outputT& out)
{
    try
    {
        this->doParse(in, out);
    }
    catch (const ParseException& e)
    {
        std::cerr << "ERROR:   " << e.what() << std::endl;
        graphic::MainWindow::console()->error(e.what());
        return false;
    }
    return true;
}


template <typename dataT>
bool DataParser<dataT>::parse(const MemChunk& in, std::shared_ptr<dataT>& out)
{
    bool success = true;

    Except::push();
    try
    {
        this->doParse(in, out);
    }
    catch (const ParseException& e)
    {
        std::cerr << "ERROR:   " << e.what() << std::endl;
        graphic::MainWindow::console()->error(e.what());
        this->onError(in, out);

        const std::shared_ptr<data::Data>& errorData = e.data();
        if (errorData)
            out->addError(errorData);

        success = false;
    }
    Except::pop();

    return success;
}

}
}

#endif // TYREX_PARSER_TPL
