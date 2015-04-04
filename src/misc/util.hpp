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

#ifndef TYREX_UTIL_HPP
#define TYREX_UTIL_HPP

#include <string>
#include <cstdint>
#include <QString>
#include <initializer_list>

namespace tyrex {

class MemChunk;

class Util
{
public:
    static unsigned int charToHex(unsigned char c);
    static unsigned int charToOctal(unsigned char c);

    static std::string numToString(uint64_t nbre);
    static std::string numSignedToString(int64_t nbre);
    static std::string doubleToString(double nbre);
    static std::string hexToString(unsigned int nbre);
    static std::string hexToString(unsigned int nbre, unsigned int numdigits);
    static std::string binToString(unsigned int nbre, unsigned int numbits);

    static unsigned int numFromOctal(const MemChunk& chunk, bool& ok);
    static QString chunkToUtf8(const MemChunk& chunk);
    static QString chunkToHex(const MemChunk& chunk);
    static QString numToHex(unsigned int nbre, unsigned int numdigits);

    // start + len <= max && start + len >= start
    static inline bool checkRange(unsigned int start, unsigned int len, unsigned int max);
    static bool checkRanges(unsigned int start, const std::initializer_list<unsigned int>& lens, unsigned int max);
};

inline bool Util::checkRange(unsigned int start, unsigned int len, unsigned int max)
{
    return start + len <= max && start + len >= start;
}

}

#endif // TYREX_UTIL_HPP
