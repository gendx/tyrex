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

#ifndef TYREX_DOCUMENT_HPP
#define TYREX_DOCUMENT_HPP

#include <QWidget>
#include <QFileInfo>
#include <QSplitter>

#include "sidetree.hpp"
#include "console.hpp"
#include "data/data.hpp"
#include "misc/memchunk.hpp"

namespace tyrex {
namespace graphic {

class Document : public QWidget
{
    Q_OBJECT

public:
    Document(const MemChunk& chunk, bool& success, QWidget* parent);

    Shared<View> currentView();
    bool hasMultipleViews();

    void split();
    void unsplit();

    void setPath(const QString& path);
    void setUntitledPath(const QString& title);

    inline QString userFriendlyPath() const;
    inline QString path() const;
    inline bool untitled() const;

signals:
    void viewChanged();

private slots:
    void focusChanged(SideTree* sideTree);

private:
    inline static QString strippedName(const QString& fullPath);

    void setFocus(SideTree* sideTree);

    QString mPath;
    bool mUntitled;
    Shared<data::Data> mData;

    QVBoxLayout* mLayout;
    QSplitter* mSplitter;
    QList<SideTree*> mSideTrees;
    SideTree* mCurrentSideTree;
    Console* mConsole;
};

inline QString Document::strippedName(const QString& fullPath)
    {return QFileInfo(fullPath).fileName();}
inline QString Document::userFriendlyPath() const
    {return this->strippedName(mPath);}
inline QString Document::path() const
    {return mPath;}
inline bool Document::untitled() const
    {return mUntitled;}

}
}

#endif // TYREX_DOCUMENT_HPP
