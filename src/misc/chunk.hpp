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

#ifndef TYREX_CHUNK_HPP
#define TYREX_CHUNK_HPP

#include <vector>
#include <memory>

namespace tyrex {

// A class to share a block of memory, or part of a block.
// Memory units are objects of class T (usually unsigned char or unsigned int).
template <typename T>
class Chunk
{
public:
    Chunk();
    Chunk(unsigned int size);
    Chunk(const Chunk& other);
    void operator=(const Chunk& other);

    bool operator<(const Chunk& other) const;
    bool operator==(const Chunk& other) const;
    inline bool operator!=(const Chunk& other) const;

    void append(T value);
    void append(T value, unsigned int length);
    void append(const Chunk& other);
    void append(const T* data, unsigned int size);
    void clear();
    Chunk subChunk(unsigned int start, unsigned int size, T fillWith) const;
    Chunk subChunk(unsigned int start, unsigned int size) const;
    Chunk subChunk(unsigned int start) const;

    inline unsigned int offset() const;
    inline unsigned int size() const;
    // Warning : unchecked access to data !
    inline const T* data() const;
    inline T operator[](unsigned int pos) const;
    inline T& operator[](unsigned int pos);
    inline T back() const;
    inline T& back();

protected:
    void clone();

    unsigned int mStart;
    unsigned int mSize;
    std::shared_ptr<std::vector<T> > mData;
};

template <typename T>
inline bool Chunk<T>::operator!=(const Chunk& other) const
    {return !(*this == other);}

template <typename T>
inline unsigned int Chunk<T>::offset() const
    {return mStart;}
template <typename T>
inline unsigned int Chunk<T>::size() const
    {return mSize;}
template <typename T>
inline const T* Chunk<T>::data() const
    {return &(*mData)[mStart];}
template <typename T>
inline T Chunk<T>::operator[](unsigned int pos) const
    {return (*mData)[mStart + pos];}
template <typename T>
inline T& Chunk<T>::operator[](unsigned int pos)
    {return (*mData)[mStart + pos];}
template <typename T>
inline T Chunk<T>::back() const
    {return (*mData)[mStart + mSize - 1];}
template <typename T>
inline T& Chunk<T>::back()
    {return (*mData)[mStart + mSize - 1];}

}

#endif // TYREX_CHUNK_HPP
