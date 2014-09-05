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

#ifndef TYREX_PARSEEXCEPTION_HPP
#define TYREX_PARSEEXCEPTION_HPP

#include <QList>

namespace tyrex {
namespace parse {

class ParseException : public std::exception
{
public:
    ParseException(unsigned int byteOffset, const std::string& who, const std::string& what);
    ~ParseException() throw();

    const char* what() const throw();
    const std::string& whatString() const;

private:
    unsigned int mByteOffset;
    std::string mWho;
    std::string mWhat;
    std::string mWhatString;
};


class Except
{
public:
    static Except mHandler;

    static void reportError(unsigned int byteOffset, const std::string& who, const std::string& what);
    static void reportWarning(unsigned int byteOffset, const std::string& who, const std::string& what);

private:
    Except();

    QList<ParseException> mErrors;
    QList<ParseException> mWarnings;
};

}
}

#endif // TYREX_PARSEEXCEPTION_HPP
