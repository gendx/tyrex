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

#include "mainwindow.hpp"

#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include "document.hpp"
#include "dialog/hexfinddialog.hpp"
#include "misc/memchunk.hpp"

namespace tyrex {
namespace graphic {

MainWindow* MainWindow::mMainWindow = nullptr;
HexFindDialog* MainWindow::mHexFindDialog = nullptr;

MainWindow::MainWindow() :
    mSplitter(new QSplitter(Qt::Vertical)),
    mMdi(new QMdiArea),
    mConsole(new Console),
    mFileToolbar(this->addToolBar("&File")),
    mViewToolbar(this->addToolBar("&View"))
{
    this->createActions();

    this->setWindowTitle("Tyrex");

    this->setCentralWidget(mSplitter);
    mSplitter->addWidget(mMdi);
    mSplitter->addWidget(mConsole);

    mMdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mMdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->tabSubwin();

    // actions
    this->updateActions();
    this->updateWindowMenu();

    mSeparatorAction->setSeparator(true);

    mFileToolbar->setIconSize(QSize(16, 16));
    mFileToolbar->addAction(mOpenAction);
    mFileToolbar->addAction(mCloseAction);
    mFileToolbar->addAction(mCloseallAction);

    mViewToolbar->setIconSize(QSize(16, 16));
    mViewToolbar->addAction(mSplitAction);
    mViewToolbar->addAction(mUnsplitAction);
    mViewToolbar->addAction(mClearConsoleAction);

    // connexion des signaux et slots
    QObject::connect(mMdi, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateActions()));
    mSignalMapper = new QSignalMapper(this);
    QObject::connect(mSignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveDocument(QWidget*)));

    QObject::connect(mWindowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    QObject::connect(mOpenAction, SIGNAL(triggered()), this, SLOT(open()));
    QObject::connect(mCloseAction, SIGNAL(triggered()), this, SLOT(closeActive()));
    QObject::connect(mCloseallAction, SIGNAL(triggered()), this, SLOT(closeAll()));

    QObject::connect(mSplitAction, SIGNAL(triggered()), this, SLOT(split()));
    QObject::connect(mUnsplitAction, SIGNAL(triggered()), this, SLOT(unsplit()));
    QObject::connect(mClearConsoleAction, SIGNAL(triggered()), this, SLOT(clearConsole()));

    QObject::connect(mTileAction, SIGNAL(triggered()), this, SLOT(tileSubwin()));
    QObject::connect(mCascadeAction, SIGNAL(triggered()), this, SLOT(cascadeSubwin()));
    QObject::connect(mTabAction, SIGNAL(triggered()), this, SLOT(tabSubwin()));
    QObject::connect(mPreviousAction, SIGNAL(triggered()), mMdi, SLOT(activatePreviousSubWindow()));
    QObject::connect(mNextAction, SIGNAL(triggered()), mMdi, SLOT(activateNextSubWindow()));

    QObject::connect(mAboutAction, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(mAboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));

    MainWindow::mMainWindow = this;
    MainWindow::mHexFindDialog = new HexFindDialog(this);
}


Console* MainWindow::console()
{
    mMainWindow->mConsole->show();
    return mMainWindow->mConsole;
}


void MainWindow::addFileFromMemChunk(const MemChunk& chunk)
{
    MainWindow::mMainWindow->openFromMemChunk(chunk);
}

void MainWindow::showFindDialog(HexArea* area)
{
    MainWindow::mHexFindDialog->setArea(area);
}

void MainWindow::attachFindDialog(HexArea* area)
{
    MainWindow::mHexFindDialog->attachArea(area);
}

void MainWindow::detachFindDialog()
{
    MainWindow::mHexFindDialog->detach();
}

void MainWindow::showMessage(QString text)
{
    MainWindow::mMainWindow->statusText(text);
}


void MainWindow::createActions()
{
    QString folder(":/icons/");

    mFileMenu = this->menuBar()->addMenu("&File");
    mOpenAction = mFileMenu->addAction(QIcon(folder + "open.png"), "&Open...");
    mCloseAction = mFileMenu->addAction(QIcon(folder + "close.png"), "&Close");
    mCloseallAction = mFileMenu->addAction(QIcon(folder + "close_all.png"), "Close all");

    mViewMenu = this->menuBar()->addMenu("&View");
    mSplitAction = mViewMenu->addAction(QIcon(folder + "split.png"), "&Split");
    mUnsplitAction = mViewMenu->addAction(QIcon(folder + "unsplit.png"), "&Unsplit");
    mClearConsoleAction = mViewMenu->addAction(QIcon(folder + "clear.png"), "&Clear console");

    mWindowMenu = this->menuBar()->addMenu("&Window");
    mTileAction = new QAction("Ti&le", this);
    mCascadeAction = new QAction("&Cascade", this);
    mTabAction = new QAction("&Tab", this);
    mPreviousAction = new QAction(QIcon(folder + "left.png"), "&Previous", this);
    mNextAction = new QAction(QIcon(folder + "right.png"), "&Next", this);
    mSeparatorAction = new QAction(this);

    mHelpMenu = this->menuBar()->addMenu("&Help");
    mAboutAction = mHelpMenu->addAction("&About...");
    mAboutQtAction = mHelpMenu->addAction("About &Qt...");
}


void MainWindow::closeEvent(QCloseEvent*/* event*/)
{
    this->closeAll();
}

void MainWindow::closeActive()
{
    MainWindow::mHexFindDialog->detach();
    mMdi->closeActiveSubWindow();
}

void MainWindow::closeAll()
{
    MainWindow::mHexFindDialog->detach();
    mMdi->closeAllSubWindows();
}


void MainWindow::split()
{
    Document* doc = this->currentDocument();
    if (doc)
        doc->split();
}

void MainWindow::unsplit()
{
    Document* doc = this->currentDocument();
    if (doc)
        doc->unsplit();
}

void MainWindow::clearConsole()
{
    mMainWindow->mConsole->clear();
}


void MainWindow::statusText(QString text)
{
    this->statusBar()->showMessage(text);
}


Document* MainWindow::createDocument(const MemChunk& chunk)
{
    bool success;
    Document* document = new Document(chunk, success, MainWindow::mMainWindow);

    if (success)
    {
        QObject::connect(document, SIGNAL(viewChanged()), MainWindow::mMainWindow, SLOT(updateActions()));
        return document;
    }

    delete document;
    return nullptr;
}

bool MainWindow::openFromMemChunk(const MemChunk& chunk)
{
    static int id = 0;
    Document* document = MainWindow::createDocument(chunk);
    if (!document)
        return false;

    mMdi->addSubWindow(document);
    document->setUntitledPath(QString("document %1").arg(++id));
    document->show();
    return true;
}

bool MainWindow::openFromMemChunk(const MemChunk& chunk, const QString& path)
{
    Document* document = MainWindow::createDocument(chunk);
    if (!document)
        return false;

    mMdi->addSubWindow(document);
    document->setPath(path);
    document->show();
    return true;
}

void MainWindow::open(const char* path)
{
    QString sPath = QFile::decodeName(path);
    if (!sPath.isEmpty())
    {
        QMdiSubWindow* existing = this->findDocument(sPath);
        if (existing)
        {
            mMdi->setActiveSubWindow(existing);
            return;
        }

        std::ifstream ifs(path, std::ios_base::binary);
        if (!ifs.good())
        {
            QMessageBox::critical(this, "File does not exists", "Unable to open file : " + sPath);
            return;
        }

        MemChunk chunk;
        chunk.append(ifs);
        ifs.close();

        if (this->openFromMemChunk(chunk, sPath))
            statusBar()->showMessage("File loaded", 2000);
    }
}

void MainWindow::open()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (!path.isEmpty())
    {
        QMdiSubWindow* existing = this->findDocument(path);
        if (existing)
        {
            mMdi->setActiveSubWindow(existing);
            return;
        }

        std::ifstream ifs(QFile::encodeName(path).constData(), std::ios_base::binary);
        if (!ifs.good())
        {
            QMessageBox::critical(this, "File does not exists", "Unable to open file : " + path);
            return;
        }

        MemChunk chunk;
        chunk.append(ifs);
        ifs.close();

        if (this->openFromMemChunk(chunk, path))
            statusBar()->showMessage("File loaded", 2000);
    }
}

void MainWindow::updateActions()
{
    Document* doc = this->currentDocument();
    bool hasDocument = (doc != 0);

    QString title = hasDocument ? doc->path() : "";
    this->setWindowTitle(title + (title.isEmpty() ? "" : " - ") + "Tyrex");

    mCloseAction->setEnabled(hasDocument);
    mCloseallAction->setEnabled(hasDocument);

    mSplitAction->setEnabled(hasDocument);
    mUnsplitAction->setEnabled(hasDocument && doc->hasMultipleViews());

    mTileAction->setEnabled(hasDocument);
    mCascadeAction->setEnabled(hasDocument);
    mTabAction->setEnabled(hasDocument);
    mNextAction->setEnabled(hasDocument);
    mPreviousAction->setEnabled(hasDocument);
    mSeparatorAction->setVisible(hasDocument);

    this->updateViewActions(hasDocument ? doc->currentView() : std::shared_ptr<View>());
}

void MainWindow::updateViewActions(std::shared_ptr<View> currentView)
{
    if (mCurrentView == currentView)
        return;

    for (auto& toolbar : mViewToolbars)
    {
        this->removeToolBar(toolbar);
        delete toolbar;
    }
    mViewToolbars.clear();

    if (currentView)
        mViewActions = currentView->getActions();
    else
        mViewActions.clear();

    // build menus & toolbars
    for (auto& subset : mViewActions.mSubsets)
    {
        QToolBar* toolbar(this->addToolBar(subset.mTitle));
        toolbar->setIconSize(QSize(16, 16));
        toolbar->addActions(subset.mActions);
        mViewToolbars.append(toolbar);
    }

    mCurrentView = currentView;
}

void MainWindow::updateWindowMenu()
{
    mWindowMenu->clear();
    mWindowMenu->addAction(mTileAction);
    mWindowMenu->addAction(mCascadeAction);
    mWindowMenu->addAction(mTabAction);
    mWindowMenu->addSeparator();
    mWindowMenu->addAction(mNextAction);
    mWindowMenu->addAction(mPreviousAction);
    mWindowMenu->addAction(mSeparatorAction);

    QList<QMdiSubWindow*> windows = mMdi->subWindowList();
    mSeparatorAction->setVisible(!windows.isEmpty());

    for (int i = 0 ; i < windows.size() ; ++i)
    {
        Document* document = qobject_cast<Document*>(windows[i]->widget());

        QString text;
        if (i < 9)
            text = "&%1 %2";
        else
            text = "%1 %2";
        text = text.arg(i + 1).arg(document->userFriendlyPath());

        QAction* action = mWindowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(document == this->currentDocument());
        QObject::connect(action, SIGNAL(triggered()), mSignalMapper, SLOT(map()));
        mSignalMapper->setMapping(action, windows[i]);
    }
}

Document* MainWindow::currentDocument()
{
    if (QMdiSubWindow* current = mMdi->currentSubWindow())
        return qobject_cast<Document*>(current->widget());
    return 0;
}

QMdiSubWindow* MainWindow::findDocument(const QString& path)
{
    QString canonicalPath = QFileInfo(path).canonicalFilePath();

    for (auto& window : mMdi->subWindowList())
    {
        Document* document = qobject_cast<Document*>(window->widget());
        if (document->path() == canonicalPath)
            return window;
    }
    return 0;
}

void MainWindow::setActiveDocument(QWidget* window)
{
    if (!window)
        return;
    mMdi->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}

void MainWindow::tileSubwin()
{
    mMdi->setViewMode(QMdiArea::SubWindowView);
    mMdi->tileSubWindows();
}

void MainWindow::cascadeSubwin()
{
    mMdi->setViewMode(QMdiArea::SubWindowView);
    mMdi->cascadeSubWindows();
}

void MainWindow::tabSubwin()
{
    mMdi->setViewMode(QMdiArea::TabbedView);
}

void MainWindow::about()
{
    QString message =
            "Tyrex - the versatile file decoder.\n"
            "Copyright (C) 2014 - 2015  G. Endignoux\n"
            "\n"
            "This program is free software: you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as published by "
            "the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version.\n"
            "\n"
            "This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
            "GNU General Public License for more details.\n"
            "\n"
            "You should have received a copy of the GNU General Public License "
            "along with this program.  If not, see http://www.gnu.org/licenses/gpl-3.0.txt";
    QMessageBox::about(this, "About Tyrex", message);
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

}
}
