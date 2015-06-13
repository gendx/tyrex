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

#ifndef TYREX_PARSEEXCEPTION_HPP
#define TYREX_PARSEEXCEPTION_HPP

#include "data/data.hpp"

namespace tyrex {
namespace parse {

class ParseException : public std::exception
{
public:
    ParseException(unsigned int byteOffset, const std::string& who, const std::string& what, const std::shared_ptr<data::Data>& data);
    ~ParseException() throw();

    const char* what() const throw();
    inline const std::string& whatString() const;
    inline const std::shared_ptr<data::Data>& data() const;

private:
    unsigned int mByteOffset;
    std::string mWho;
    std::string mWhat;
    std::string mWhatString;
    std::shared_ptr<data::Data> mData;
};

inline const std::string& ParseException::whatString() const
    {return mWhatString;}
inline const std::shared_ptr<data::Data>& ParseException::data() const
    {return mData;}


class Except
{
public:
    static void push();
    static void pop();

    static void reportError(unsigned int byteOffset, const std::string& who, const std::string& what, const std::shared_ptr<data::Data>& data = nullptr);
    static void reportWarning(unsigned int byteOffset, const std::string& who, const std::string& what, const std::shared_ptr<data::Data>& data = nullptr);

private:
    static std::vector<std::shared_ptr<Except> > mHandlerStack;
    static std::shared_ptr<Except> mHandler;

    std::vector<ParseException> mErrors;
    std::vector<ParseException> mWarnings;
};

}
}

#endif // TYREX_PARSEEXCEPTION_HPP
