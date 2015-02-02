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

#ifndef TYREX_CHUNK_TPL
#define TYREX_CHUNK_TPL

#include "chunk.hpp"
#include "util.hpp"

namespace tyrex {

template <typename T>
Chunk<T>::Chunk() :
    mStart(0),
    mSize(0),
    mData(std::make_shared<std::vector<T> >())
{
}

template <typename T>
Chunk<T>::Chunk(unsigned int size) :
    mStart(0),
    mSize(size),
    mData(std::make_shared<std::vector<T> >(size, 0))
{
}

template <typename T>
Chunk<T>::Chunk(const Chunk& other) :
    mStart(other.mStart),
    mSize(other.mSize),
    mData(other.mData)
{
}

template <typename T>
void Chunk<T>::operator=(const Chunk& other)
{
    mData = other.mData;
    mStart = other.mStart;
    mSize = other.mSize;
}

template <typename T>
void Chunk<T>::clone()
{
    std::shared_ptr<std::vector<T> > data(std::make_shared<std::vector<T> >(mSize));
    const std::vector<T>& src = *mData;
    std::vector<T>& dst = *data;

    for (unsigned int i = 0, k = mStart ; i < mSize ; ++i, ++k)
        dst[i] = src[k];

    mData = data;
    mStart = 0;
}


template <typename T>
bool Chunk<T>::operator<(const Chunk& other) const
{
    for (unsigned int i = 0 ; i < mSize && i < other.mSize ; ++i)
    {
        if ((*mData)[mStart + i] < (*other.mData)[other.mStart + i])
            return true;
        else if ((*mData)[mStart + i] > (*other.mData)[other.mStart + i])
            return false;
    }

    if (mSize < other.mSize)
        return true;
    return false;
}

template <typename T>
bool Chunk<T>::operator==(const Chunk& other) const
{
    if (mSize != other.mSize)
        return false;

    for (unsigned int i = 0 ; i < mSize ; ++i)
        if ((*mData)[mStart + i] != (*other.mData)[other.mStart + i])
            return false;

    return true;
}


template <typename T>
void Chunk<T>::append(T value)
{
    mData->push_back(value);
    ++mSize;
}

template <typename T>
void Chunk<T>::append(T value, unsigned int length)
{
    for (unsigned int i = 0 ; i < length ; ++i)
        mData->push_back(value);
    mSize += length;
}

template <typename T>
void Chunk<T>::append(const Chunk& other)
{
    if (mData == other.mData)
        clone();

    const std::vector<T>& src = *other.mData;
    std::vector<T>& dst = *mData;
    for (unsigned int i = other.mStart ; i < other.mStart + other.mSize ; ++i)
        dst.push_back(src[i]);
    mSize += other.mSize;
}

template <typename T>
void Chunk<T>::append(const T* data, unsigned int size)
{
    for (unsigned int i = 0 ; i < size ; ++i)
        mData->push_back(data[i]);
    mSize += size;
}

template <typename T>
void Chunk<T>::clear()
{
    mStart = 0;
    mSize = 0;
    mData = std::make_shared<std::vector<T> >();
}

template <typename T>
Chunk<T> Chunk<T>::subChunk(unsigned int start, unsigned int size, T fillWith) const
{
    Chunk chunk;

    if (Util::checkRange(start, size, mSize))
    {
        chunk.mData = mData;
        chunk.mStart = mStart + start;
        chunk.mSize = size;
    }
    else
    {
        const std::vector<T>& src = *mData;
        std::vector<T>& dst = *chunk.mData;

        for (unsigned int i = start ; i < mSize ; ++i)
            dst.push_back(src[mStart + i]);
        for (unsigned int i = mSize ; i < start + size ; ++i)
            dst.push_back(fillWith);
        chunk.mSize = size;
    }

    return chunk;
}

template <typename T>
Chunk<T> Chunk<T>::subChunk(unsigned int start, unsigned int size) const
{
    Chunk chunk;

    if (Util::checkRange(start, size, mSize))
    {
        chunk.mData = mData;
        chunk.mStart = mStart + start;
        chunk.mSize = size;
    }

    return chunk;
}

template <typename T>
Chunk<T> Chunk<T>::subChunk(unsigned int start) const
{
    Chunk chunk;

    if (start < mSize)
    {
        chunk.mData = mData;
        chunk.mStart = mStart + start;
        chunk.mSize = mSize - start;
    }

    return chunk;
}

}

#endif // TYREX_CHUNK_TPL
