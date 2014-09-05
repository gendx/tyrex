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

#include "util.hpp"

#include "memchunk.hpp"
#include <sstream>

namespace tyrex {

unsigned int Util::charToHex(unsigned char c)
{
    if (c >= '0' && c <= '9') // 0-9
        return c - '0';
    else if (c >= 'A' && c <= 'F') // A-F
        return c + 10 - 'A';
    else if (c >= 'a' && c <= 'f') // a-f
        return c + 10 - 'a';
    return 0xFFFFFFFF;
}

unsigned int Util::charToOctal(unsigned char c)
{
    if (c >= '0' && c <= '7') // 0-7
        return c - '0';
    return 0xFFFFFFFF;
}


std::string Util::numToString(uint64_t nbre)
{
    std::ostringstream oss;
    oss << nbre;
    return oss.str();
}

std::string Util::numSignedToString(int64_t nbre)
{
    std::ostringstream oss;
    oss << nbre;
    return oss.str();
}

std::string Util::doubleToString(double nbre)
{
    std::ostringstream oss;
    oss << nbre;
    return oss.str();
}

std::string Util::hexToString(unsigned int nbre)
{
    std::ostringstream oss;
    oss << "0x" << std::hex << nbre;
    std::string str = oss.str();

    for (unsigned int i = 2 ; i < str.size() ; ++i)
        str[i] = std::toupper(str[i]);

    return str;
}

std::string Util::hexToString(unsigned int nbre, unsigned int numdigits)
{
    std::ostringstream oss;
    oss << std::hex << nbre;
    std::string str = oss.str();

    std::string result;
    if (str.size() < numdigits)
        result.append(numdigits - str.size(), '0');

    for (char c : str)
        result.push_back(std::toupper(c));

    return result;
}

std::string Util::binToString(unsigned int nbre, unsigned int numbits)
{
    std::string result(numbits, '0');

    unsigned int mask = 1;
    for (unsigned int i = 0 ; i < numbits ; ++i)
    {
        if (nbre & mask)
            result[numbits - i - 1] = '1';
        mask <<= 1;
    }

    return result;
}


unsigned int Util::numFromOctal(const MemChunk& chunk, bool& ok)
{
    ok = true;
    unsigned int i = 0;

    for ( ; i < chunk.size() ; ++i)
    {
        if (chunk[i] >= '0' && chunk[i] <= '7')
            break;
        else if (chunk[i] != 0 && chunk[i] != ' ')
            ok = false;
    }

    unsigned int result = 0;
    for ( ; i < chunk.size() ; ++i)
    {
        if (chunk[i] >= '0' && chunk[i] <= '7')
        {
            result <<= 3;
            result += chunk[i] - '0';
        }
        else
            break;
    }

    for ( ; i < chunk.size() ; ++i)
        if (chunk[i] != 0 && chunk[i] != ' ')
            ok = false;

    return result;
}

QString Util::chunkToUtf8(const MemChunk& chunk)
{
    return QString::fromUtf8(reinterpret_cast<const char*>(chunk.data()), chunk.size());
}

QString Util::chunkToHex(const MemChunk& chunk)
{
    QString result;

    for (unsigned int i = 0 ; i < chunk.size() ; ++i)
    {
        unsigned int value = chunk[i];
        unsigned int h1 = value >> 4;
        unsigned int h2 = value & 0xF;

        QChar c;
        if (h1 < 10)
            c = '0' + h1;
        else if (h1 < 16)
            c = 'a' + h1 - 10;
        result.append(c);

        if (h2 < 10)
            c = '0' + h2;
        else if (h2 < 16)
            c = 'a' + h2 - 10;
        result.append(c);
    }

    return result;
}

QString Util::numToHex(unsigned int nbre, unsigned int numdigits)
{
    QString result;

    for (unsigned int i = 0 ; i < numdigits ; ++i)
    {
        unsigned int digit = (nbre >> (i << 2)) & 0xF;

        QChar c;
        if (digit < 10)
            c = '0' + digit;
        else if (digit < 16)
            c = 'a' + digit - 10;
        result.prepend(c);
    }

    return result;
}

}
