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

#include "parseexception.hpp"

#include "misc/util.hpp"

#include <iostream>

namespace tyrex {
namespace parse {

ParseException::ParseException(unsigned int byteOffset, const std::string& who, const std::string& what, const std::shared_ptr<data::Data>& data) :
    mByteOffset(byteOffset),
    mWho(who),
    mWhat(what),
    mWhatString(Util::numToString(byteOffset) + "(" + Util::hexToString(byteOffset) + "):" + who + ":" + what),
    mData(data)
{
}

ParseException::~ParseException() throw()
{
}


const char* ParseException::what() const throw()
{
    return mWhatString.c_str();
}


std::vector<std::shared_ptr<Except> > Except::mHandlerStack;
std::shared_ptr<Except> Except::mHandler;


void Except::push()
{
    mHandler = std::make_shared<Except>();
    mHandlerStack.push_back(mHandler);
}

void Except::pop()
{
    if (!mHandlerStack.empty())
    {
        mHandlerStack.pop_back();
        if (!mHandlerStack.empty())
            mHandler = mHandlerStack.back();
        else
            mHandler.reset();
    }
}


void Except::reportError(unsigned int byteOffset, const std::string& who, const std::string& what, const std::shared_ptr<data::Data>& data)
{
    mHandler->mErrors.push_back(ParseException(byteOffset, who, what, data));
    throw mHandler->mErrors.back();
}

void Except::reportWarning(unsigned int byteOffset, const std::string& who, const std::string& what, const std::shared_ptr<data::Data>& data)
{
    mHandler->mWarnings.push_back(ParseException(byteOffset, who, what, data));
    std::cerr << "WARNING: " << mHandler->mWarnings.back().what() << std::endl;
}

}
}
