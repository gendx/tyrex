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

#include "hexview.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include "graphic/mainwindow.hpp"
#include "graphic/dialog/consoledialog.hpp"
#include "misc/hash/hash.hpp"
#include "misc/util.hpp"

namespace tyrex {
namespace graphic {

HexView::HexView(const MemChunk& chunk, const data::Colorizer& colorizer, QWidget* parent) :
    ScrollView(new HexArea(chunk, colorizer, this), parent),
    mHexArea(reinterpret_cast<HexArea*>(mArea))
{
}


ActionSet HexView::getActions()
{
    QString folder(":/icons/");

    ActionSubset file;
    file.mTitle = "&File";

    QAction* saveAction = new QAction(QIcon(folder + "save.png"), "&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(saveAction()));
    file.mActions.append(saveAction);

    ActionSubset edit;
    edit.mTitle = "&Edit";

    QAction* findAction = new QAction(QIcon(folder + "find.png"), "&Find", this);
    findAction->setShortcut(QKeySequence::Find);
    QObject::connect(findAction, SIGNAL(triggered()), this, SLOT(findAction()));
    edit.mActions.append(findAction);

    QAction* exportAction = new QAction(QIcon(folder + "export.png"), "&Export", this);
    QObject::connect(exportAction, SIGNAL(triggered()), this, SLOT(exportAction()));
    edit.mActions.append(exportAction);

    QAction* extractAction = new QAction(QIcon(folder + "copy.png"), "Export &selection", this);
    QObject::connect(extractAction, SIGNAL(triggered()), this, SLOT(extractAction()));
    edit.mActions.append(extractAction);

    QAction* infoAction = new QAction(QIcon(folder + "help.png"), "&Information", this);
    QObject::connect(infoAction, SIGNAL(triggered()), this, SLOT(infoAction()));
    edit.mActions.append(infoAction);

    ActionSet result;
    result.mSubsets.append(file);
    result.mSubsets.append(edit);
    return result;
}

void HexView::saveAction()
{
    QString path = QFileDialog::getSaveFileName(this, "Save");
    if (path.isNull())
        return;

    if (QFileInfo(path).exists())
        if (QMessageBox::warning(this, "Save", QString("File %1 already exists. Do you want to overwrite it ?").arg(path), QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Save)
            return;

    std::ofstream ofs(QFile::encodeName(path).constData(), std::ios_base::binary);

    if (!ofs.good())
    {
        QMessageBox::critical(this, "Error", QString("Unable to save file %1").arg(path));
        return;
    }

    mHexArea->memChunk().write(ofs);
    ofs.close();

    MainWindow::showMessage("File saved");
}

void HexView::findAction()
{
    MainWindow::showFindDialog(mHexArea);
}

void HexView::exportAction()
{
    MainWindow::addFileFromMemChunk(mHexArea->memChunk());
}

void HexView::extractAction()
{
    MainWindow::addFileFromMemChunk(mHexArea->currentSelection());
}

void HexView::infoAction()
{
    MemChunk chunk = mHexArea->memChunk();

    ConsoleDialog dialog("Information", this);
    dialog.printInfo(QString("Length : ") + QString::number(chunk.size()) + " (0x" + QString::number(chunk.size(), 16) + ")");
    dialog.printInfo(QString("Sha256 : ") + Util::chunkToHex(Hasher::getSha256(chunk).chunk()));
    dialog.printInfo(QString("CRC32 : ") + Util::numToHex(Hasher::getCRC32(chunk), 8));

    dialog.exec();
}

}
}
