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

#include "truetype.hpp"

#include <QDateTime>
#include <map>

#include <iostream>

namespace tyrex {
namespace parse {

unsigned char Truetype::mMagic1[4] = {0x00, 0x01, 0x00, 0x00};
unsigned char Truetype::mMagic2[4] = {0x74, 0x72, 0x75, 0x65};
unsigned int Truetype::mMagicHead = 0x5F0F3CF5;


Truetype::Truetype() :
    mProperties("Properties", QStringList() << "Property" << "Value")
{
}


void Truetype::onError(const MemChunk& chunk, Shared<data::Font>& data)
{
    data = makeShared<data::Font>(chunk, mSrcColorizer, mGlyphs, mProperties);
}

void Truetype::doParse(const MemChunk& chunk, Shared<data::Font>& data)
{
    unsigned int size = chunk.size();

    // check initial size
    if (size < 4)
        Except::reportError(size, "truetype magic", "unexpected end of data");
    // check magic
    if (chunk.uncompare(Truetype::mMagic1, 4) && chunk.uncompare(Truetype::mMagic2, 4))
        Except::reportError(0, "truetype magic", "invalid magic");

    mSrcColorizer.addHighlight(0, 4, QColor(255, 128, 0, 64));
    mSrcColorizer.addSeparation(4, 2);

    // check header
    if (size < 12)
        Except::reportError(size, "truetype header", "unexpected end of data");

    unsigned int numTables = chunk.getUint16BE(4);
    mSrcColorizer.addSeparation(6, 1);
    unsigned int searchRange = chunk.getUint16BE(6);
    mSrcColorizer.addSeparation(8, 1);
    unsigned int entrySelector = chunk.getUint16BE(8);
    mSrcColorizer.addSeparation(10, 1);
    unsigned int rangeShift = chunk.getUint16BE(10);
    mSrcColorizer.addSeparation(12, 2);
    mSrcColorizer.addHighlight(4, 8, QColor(255, 0, 0, 64));

    if (size < 12 + 16 * numTables)
        Except::reportError(size, "truetype table directory", "unexpected end of data");

    mSrcColorizer.addHighlight(12, 16 * numTables, QColor(128, 255, 0, 64));
    unsigned int processed = 12;

    std::map<unsigned int, std::pair<unsigned int, unsigned int> > typeToTable;
    for (unsigned int i = 0 ; i < numTables ; ++i)
    {
        mSrcColorizer.addSeparation(processed + 4, 1);
        mSrcColorizer.addSeparation(processed + 8, 1);
        mSrcColorizer.addSeparation(processed + 12, 1);
        mSrcColorizer.addSeparation(processed + 16, 2);

        unsigned int type = chunk.getUint32BE(processed);
        unsigned int pos = chunk.getUint32BE(processed + 8);
        unsigned int length = chunk.getUint32BE(processed + 12);

        if (size < pos + length)
            Except::reportError(processed, "truetype table dictionnary", "invalid table position and/or length");

        typeToTable[type] = std::make_pair(pos, length);

        processed += 16;
    }

    // tables
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator cmap = typeToTable.find(0x636D6170);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator glyf = typeToTable.find(0x676C7966);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator head = typeToTable.find(0x68656164);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator hhea = typeToTable.find(0x68686561);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator hmtx = typeToTable.find(0x686D7478);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator loca = typeToTable.find(0x6C6F6361);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator maxp = typeToTable.find(0x6D617870);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator name = typeToTable.find(0x6E616D65);
    std::map<unsigned int, std::pair<unsigned int, unsigned int> >::const_iterator post = typeToTable.find(0x706F7374);

    // head
    if (head == typeToTable.end())
        Except::reportError(size, "truetype head table", "table not found");
    this->parseHead(chunk, head->second.first, head->second.second);

    // hhea
    if (hhea == typeToTable.end())
        Except::reportError(size, "truetype hhea table", "table not found");
    this->parseHhea(chunk, hhea->second.first, hhea->second.second);

    // maxp
    if (maxp == typeToTable.end())
        Except::reportError(size, "truetype maxp table", "table not found");
    this->parseMaxp(chunk, maxp->second.first, maxp->second.second);

    // loca
    if (loca == typeToTable.end())
        Except::reportError(size, "truetype loca table", "table not found");
    this->parseLoca(chunk, loca->second.first, loca->second.second);

    // hmtx
    if (hmtx == typeToTable.end())
        Except::reportError(size, "truetype hmtx table", "table not found");
    this->parseHmtx(chunk, hmtx->second.first, hmtx->second.second);

    // glyf
    if (glyf == typeToTable.end())
        Except::reportError(size, "truetype glyf table", "table not found");
    this->parseGlyf(chunk, glyf->second.first, glyf->second.second);

    data = makeShared<data::Font>(chunk, mSrcColorizer, mGlyphs, mProperties);
}


void Truetype::parseHead(const MemChunk& chunk, unsigned int pos, unsigned int size)
{
    if (size != 54)
        Except::reportError(pos + size, "truetype head table", "invalid table size");

    mSrcColorizer.addHighlight(pos, 54, QColor(128, 0, 255, 64));
    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + 4, 1);
    mSrcColorizer.addSeparation(pos + 8, 1);
    mSrcColorizer.addSeparation(pos + 12, 1);
    mSrcColorizer.addSeparation(pos + 16, 1);
    mSrcColorizer.addSeparation(pos + 18, 1);
    mSrcColorizer.addSeparation(pos + 20, 1);
    mSrcColorizer.addSeparation(pos + 28, 1);
    mSrcColorizer.addSeparation(pos + 36, 1);
    mSrcColorizer.addSeparation(pos + 38, 1);
    mSrcColorizer.addSeparation(pos + 40, 1);
    mSrcColorizer.addSeparation(pos + 42, 1);
    mSrcColorizer.addSeparation(pos + 44, 1);
    mSrcColorizer.addSeparation(pos + 46, 1);
    mSrcColorizer.addSeparation(pos + 48, 1);
    mSrcColorizer.addSeparation(pos + 50, 1);
    mSrcColorizer.addSeparation(pos + 52, 1);
    mSrcColorizer.addSeparation(pos + 54, 2);

    MemChunk head = chunk.subChunk(pos, size);

    unsigned int version = head.getUint32BE(0);
    unsigned int fontRevision = head.getUint32BE(4);
    unsigned int checkSumAdjustment = head.getUint32BE(8);
    unsigned int magicNumber = head.getUint32BE(12);
    unsigned int flags = head.getUint16BE(16);
    unsigned int unitsPerEm = head.getUint16BE(18);
    int64_t created = head.getUint64BE(20);
    int64_t modified = head.getUint64BE(28);
    int xmin = head.getInt16BE(36);
    int ymin = head.getInt16BE(38);
    int xmax = head.getInt16BE(40);
    int ymax = head.getInt16BE(42);
    unsigned int macStyle = head.getUint16BE(44);
    unsigned int lowestRecPPEM = head.getUint16BE(46);
    int fontDirectionHint = head.getInt16BE(48);
    mIndexToLocFormat = head.getInt16BE(50);
    int glyphDataFormat = head.getInt16BE(52);

    if (version != 0x10000)
        Except::reportError(pos, "truetype version", "invalid version");
    if (magicNumber != Truetype::mMagicHead)
        Except::reportError(pos + 12, "truetype head magic", "invalid magic");

    QDateTime creationDate = QDateTime(QDate(1904, 1, 1)).addDays(created / 86400).addSecs(created % 86400);
    mProperties.push("Creation", creationDate.toString("dd MMM yyyy  hh:mm:ss"));
    QDateTime modificationDate = QDateTime(QDate(1904, 1, 1)).addDays(modified / 86400).addSecs(modified % 86400);
    mProperties.push("Modification", modificationDate.toString("dd MMM yyyy  hh:mm:ss"));
    mProperties.push("Units per em", QString::number(unitsPerEm));
    mProperties.push("X min", QString::number(xmin));
    mProperties.push("Y min", QString::number(ymin));
    mProperties.push("X max", QString::number(xmax));
    mProperties.push("Y max", QString::number(ymax));

    QString style;
    if (macStyle & 0x1) style += "bold ";
    if (macStyle & 0x2) style += "italic ";
    if (macStyle & 0x4) style += "underline ";
    if (macStyle & 0x8) style += "outline ";
    if (macStyle & 0x10) style += "shadow ";
    if (macStyle & 0x20) style += "condensed ";
    if (macStyle & 0x40) style += "extended ";

    if (style.isEmpty())
        style = "normal";
    else
        style.chop(1);
    mProperties.push("Style", style);
}

void Truetype::parseHhea(const MemChunk& chunk, unsigned int pos, unsigned int size)
{
    if (size != 36)
        Except::reportError(pos + size, "truetype hhea table", "invalid table size");

    mSrcColorizer.addHighlight(pos, 36, QColor(128, 0, 255, 64));
    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + 4, 1);
    mSrcColorizer.addSeparation(pos + 6, 1);
    mSrcColorizer.addSeparation(pos + 8, 1);
    mSrcColorizer.addSeparation(pos + 10, 1);
    mSrcColorizer.addSeparation(pos + 12, 1);
    mSrcColorizer.addSeparation(pos + 14, 1);
    mSrcColorizer.addSeparation(pos + 16, 1);
    mSrcColorizer.addSeparation(pos + 18, 1);
    mSrcColorizer.addSeparation(pos + 20, 1);
    mSrcColorizer.addSeparation(pos + 22, 1);
    mSrcColorizer.addSeparation(pos + 24, 1);
    mSrcColorizer.addSeparation(pos + 26, 1);
    mSrcColorizer.addSeparation(pos + 28, 1);
    mSrcColorizer.addSeparation(pos + 30, 1);
    mSrcColorizer.addSeparation(pos + 32, 1);
    mSrcColorizer.addSeparation(pos + 34, 1);
    mSrcColorizer.addSeparation(pos + 36, 2);

    MemChunk hhea = chunk.subChunk(pos, size);

    unsigned int version = hhea.getUint32BE(0);
    int ascent = hhea.getInt16BE(4);
    int descent = hhea.getInt16BE(6);
    int lineGap = hhea.getInt16BE(8);
    unsigned int advanceWidthMax = hhea.getUint16BE(10);
    int minLeftSideBearing = hhea.getInt16BE(12);
    int minRightSideBearing = hhea.getInt16BE(14);
    int xMaxExtent = hhea.getInt16BE(16);
    int caretSlopeRise = hhea.getInt16BE(18);
    int caretSlopeRun = hhea.getInt16BE(20);
    int caretOffset = hhea.getInt16BE(22);
    int reserved1 = hhea.getInt16BE(24);
    int reserved2 = hhea.getInt16BE(26);
    int reserved3 = hhea.getInt16BE(28);
    int reserved4 = hhea.getInt16BE(30);
    int metricDataFormat = hhea.getInt16BE(32);
    mNumOfLongHorMetrics = hhea.getUint16BE(34);

    if (version != 0x10000)
        Except::reportError(pos, "truetype version", "invalid version");
    if (!mNumOfLongHorMetrics)
        Except::reportError(pos + 34, "truetype hhea table", "numOfLongHorMetrics must be non zero");
}

void Truetype::parseMaxp(const MemChunk& chunk, unsigned int pos, unsigned int size)
{
    if (size != 32)
        Except::reportError(pos + size, "truetype maxp table", "invalid table size");

    mSrcColorizer.addHighlight(pos, 32, QColor(128, 0, 255, 64));
    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + 4, 1);
    mSrcColorizer.addSeparation(pos + 6, 1);
    mSrcColorizer.addSeparation(pos + 8, 1);
    mSrcColorizer.addSeparation(pos + 10, 1);
    mSrcColorizer.addSeparation(pos + 12, 1);
    mSrcColorizer.addSeparation(pos + 14, 1);
    mSrcColorizer.addSeparation(pos + 16, 1);
    mSrcColorizer.addSeparation(pos + 18, 1);
    mSrcColorizer.addSeparation(pos + 20, 1);
    mSrcColorizer.addSeparation(pos + 22, 1);
    mSrcColorizer.addSeparation(pos + 24, 1);
    mSrcColorizer.addSeparation(pos + 26, 1);
    mSrcColorizer.addSeparation(pos + 28, 1);
    mSrcColorizer.addSeparation(pos + 30, 1);
    mSrcColorizer.addSeparation(pos + 32, 2);

    MemChunk maxp = chunk.subChunk(pos, size);

    unsigned int version = maxp.getUint32BE(0);
    mNumGlyphs = maxp.getUint16BE(4);
    unsigned int maxPoints = maxp.getUint16BE(6);
    unsigned int maxContours = maxp.getUint16BE(8);
    unsigned int maxComponentPoints = maxp.getUint16BE(10);
    unsigned int maxComponentContours = maxp.getUint16BE(12);
    unsigned int maxZones = maxp.getUint16BE(14);
    unsigned int maxTwilightPoints = maxp.getUint16BE(16);
    unsigned int maxStorage = maxp.getUint16BE(18);
    unsigned int maxFunctionDefs = maxp.getUint16BE(20);
    unsigned int maxInstructionDefs = maxp.getUint16BE(22);
    unsigned int maxStackElements = maxp.getUint16BE(24);
    unsigned int maxSizeOfInstructions = maxp.getUint16BE(26);
    unsigned int maxComponentElements = maxp.getUint16BE(28);
    unsigned int maxComponentDepth = maxp.getUint16BE(30);

    if (version != 0x10000)
        Except::reportError(pos, "truetype version", "invalid version");
}

void Truetype::parseLoca(const MemChunk& chunk, unsigned int pos, unsigned int size)
{
    if (size != (mNumGlyphs + 1) * (mIndexToLocFormat ? 4 : 2))
        Except::reportError(pos + size, "truetype loca table", "invalid table size");

    mSrcColorizer.addHighlight(pos, size, QColor(128, 128, 255, 64));
    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + size, 2);

    if (mIndexToLocFormat)
        for (unsigned int i = 0 ; i <= mNumGlyphs ; ++i)
            mLocations.push_back(chunk.getUint32BE(pos + (i << 2)));
    else
        for (unsigned int i = 0 ; i <= mNumGlyphs ; ++i)
            mLocations.push_back(chunk.getUint16BE(pos + (i << 1)) << 1);
}

void Truetype::parseHmtx(const MemChunk& chunk, unsigned int pos, unsigned int size)
{
    if (size != (mNumGlyphs + mNumOfLongHorMetrics) * 2)
        Except::reportError(pos + size, "truetype hmtx table", "invalid table size");

    mSrcColorizer.addHighlight(pos, size, QColor(128, 128, 255, 64));
    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + size, 2);

    MemChunk hmtx = chunk.subChunk(pos, size);

    for (unsigned int i = 0 ; i < mNumOfLongHorMetrics ; ++i)
        mHorizontalMetrics.push_back(std::make_pair(hmtx.getUint16BE(i << 2), hmtx.getInt16BE((i << 2) + 2)));

    unsigned int advanceWidth = mHorizontalMetrics.back().first;
    unsigned int off = mNumOfLongHorMetrics << 1;

    for (unsigned int i = mNumOfLongHorMetrics ; i < mNumGlyphs ; ++i)
        mHorizontalMetrics.push_back(std::make_pair(advanceWidth, hmtx.getInt16BE(off + (i << 1))));
}

void Truetype::parseGlyf(const MemChunk& chunk, unsigned int pos, unsigned int size)
{
    for (unsigned int i = 0 ; i < mNumGlyphs ; ++i)
    {
        if (i)
            mSrcColorizer.addSeparation(pos + mLocations[i], 1);

        MemChunk glyf = chunk.subChunk(pos + mLocations[i], mLocations[i + 1] - mLocations[i]);

        if (this->parseGlyph(glyf))
            std::cout << "glyf[" << i << "].size() = " << glyf.size() << std::endl;
        else
            std::cout << "glyf[" << i << "] : ERROR" << std::endl;
    }

    mSrcColorizer.addHighlight(pos, size, QColor(0, 255, 255, 64));
    mSrcColorizer.addSeparation(pos, 2);
    mSrcColorizer.addSeparation(pos + size, 2);
}

bool Truetype::parseCompoundGlyph(const MemChunk& glyf)
{
    return false;
}

bool Truetype::parseGlyph(const MemChunk& glyf)
{
    unsigned int size = glyf.size();
    if (!size)
        return true;
    if (size < 10)
        return false;

    int numberOfContours = glyf.getInt16BE(0);
    int xMin = glyf.getInt16BE(2);
    int yMin = glyf.getInt16BE(4);
    int xMax = glyf.getInt16BE(6);
    int yMax = glyf.getInt16BE(8);

    if (numberOfContours == -1)
        return this->parseCompoundGlyph(glyf);
    if (numberOfContours < 0)
        return false;
    if (size < 10 + (numberOfContours << 1))
        return false;

    unsigned int processed = 10;

    unsigned int lastPoint = 0;
    std::vector<unsigned int> endPtsOfContours;
    for (int j = 0 ; j < numberOfContours ; ++j)
    {
        lastPoint = glyf.getUint16BE(processed);
        endPtsOfContours.push_back(lastPoint);
        processed += 2;
    }

    if (size < processed + 2)
        return false;
    unsigned int instructionLength = glyf.getUint16BE(processed);
    processed += 2;

    if (size < processed + instructionLength)
        return false;
    processed += instructionLength;

    // TODO: process instructions
    if (instructionLength)
        return false;

    std::vector<unsigned char> flags;
    while (flags.size() <= lastPoint)
    {
        if (size <= processed)
            return false;
        unsigned char flag = glyf[processed];
        ++processed;

        flags.push_back(flag);

        if (flag & 0x8)
        {
            if (size <= processed)
                return false;
            unsigned char repeats = glyf[processed];
            ++processed;
            for (unsigned int j = 0 ; j < repeats ; ++j)
                flags.push_back(flag);
        }
    }

    // mark begin points
    flags[0] ^= 0x80;
    for (unsigned int j = 0 ; j < numberOfContours ; ++j)
    {
        unsigned int k = endPtsOfContours[j] + 1;
        if (k <= lastPoint)
            flags[k] ^= 0x80;
    }

    std::vector<int> x;
    int last = 0;
    for (unsigned int j = 0 ; j <= lastPoint ; ++j)
    {
        unsigned char flag = flags[j];
        int val;

        if (flag & 0x2)
        {
            if (size <= processed)
                return false;
            val = glyf[processed];
            ++processed;
            if (!(flag & 0x10))
                val = -val;
        }
        else if (flag & 0x10)
            val = 0;
        else
        {
            if (size < processed + 2)
                return false;
            val = glyf.getInt16BE(processed);
            processed += 2;
        }

        last += val;
        x.push_back(last);
    }

    std::vector<int> y;
    last = 0;
    for (unsigned int j = 0 ; j <= lastPoint ; ++j)
    {
        unsigned char flag = flags[j];
        int val;

        if (flag & 0x4)
        {
            if (size <= processed)
                return false;
            val = glyf[processed];
            ++processed;
            if (!(flag & 0x20))
                val = -val;
        }
        else if (flag & 0x20)
            val = 0;
        else
        {
            if (size < processed + 2)
                return false;
            val = glyf.getInt16BE(processed);
            processed += 2;
        }

        last += val;
        y.push_back(last);
    }

    Shared<data::Path> path(makeShared<data::Path>());

    std::vector<std::pair<data::Path::Point, bool> > points;
    for (unsigned int j = 0 ; j <= lastPoint ; ++j)
    {
        unsigned char flag = flags[j];

        if (flag & 0x80)
        {
            this->appendPath(path, points);
            points.clear();
        }

        points.push_back(std::make_pair(data::Path::Point(x[j], y[j]), !(flag & 0x1)));
    }

    this->appendPath(path, points);

    mGlyphs.append(path);
    return true;
}

void Truetype::appendPath(Shared<data::Path> path, const std::vector<std::pair<data::Path::Point, bool> >& points)
{
    if (points.empty())
        return;

    std::pair<data::Path::Point, bool> last = points.back();
    std::pair<data::Path::Point, bool> first = points.front();

    // Move to starting point
    if (first.second) // first is control point
    {
        if (last.second) // two successive control points -> middle
            path->moveTo(data::Path::Point::mid(last.first, first.first));
        else // begin at last point
            path->moveTo(last.first);
    }
    else // first is regular
        path->moveTo(first.first);

    last = first;

    // Draw inner lines
    for (unsigned int i = 1 ; i < points.size() ; ++i)
    {
        std::pair<data::Path::Point, bool> me = points[i];

        if (me.second && last.second) // two successive control points -> middle
            path->quadTo(last.first, data::Path::Point::mid(last.first, me.first));
        else if (last.second) // last was control point -> draw quad
            path->quadTo(last.first, me.first);
        else if (!me.second) // neither is control -> draw line
            path->lineTo(me.first);

        last = me;
    }

    // Close path
    if (first.second && last.second) // two successive control points -> middle
        path->quadTo(last.first, data::Path::Point::mid(last.first, first.first));
    else if (last.second) // last was control point -> draw quad
        path->quadTo(last.first, first.first);
    else if (!first.second) // neither is control -> draw line
        path->lineTo(first.first);
}

}
}
