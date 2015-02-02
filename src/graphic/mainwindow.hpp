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

#ifndef TYREX_MAIN_WINDOW_HPP
#define TYREX_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSignalMapper>
#include "misc/memchunk.hpp"
#include "misc/shared.hpp"
#include "actionset.hpp"
#include "graphic/view/view.hpp"

namespace tyrex {
namespace graphic {

class Document;
class HexArea;
class HexFindDialog;
class Console;

// The application's main window.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    static void addFileFromMemChunk(const MemChunk& chunk);
    static void showFindDialog(HexArea* area);
    static void attachFindDialog(HexArea* area);
    static void detachFindDialog();
    static void showMessage(QString text);
    static inline Document* getCurrentDocument();
    static Console* console();

    void open(const char* path);

private slots:
    void open();
    void closeActive();
    void closeAll();

    void split();
    void unsplit();
    void clearConsole();

    void tileSubwin();
    void cascadeSubwin();
    void tabSubwin();

    void about();
    void aboutQt();

    void updateActions();
    void updateWindowMenu();
    void setActiveDocument(QWidget* window);
    void statusText(QString text);

private:
    void closeEvent(QCloseEvent* event);

    void createActions();
    void updateViewActions(Shared<View> currentView);
    static Document* createDocument(const MemChunk& chunk);
    bool openFromMemChunk(const MemChunk& chunk, const QString& path);
    bool openFromMemChunk(const MemChunk& chunk);

    Document* currentDocument();
    QMdiSubWindow* findDocument(const QString& path);

    static MainWindow* mMainWindow;
    static HexFindDialog* mHexFindDialog;


    Shared<View> mCurrentView;
    ActionSet mViewActions;
    QList<QToolBar*> mViewToolbars;

    QSplitter* mSplitter;
    QMdiArea* mMdi;
    QSignalMapper* mSignalMapper;
    Console* mConsole;

    QMenu* mFileMenu;
    QAction* mOpenAction;
    QAction* mCloseAction;
    QAction* mCloseallAction;

    QMenu* mViewMenu;
    QAction* mSplitAction;
    QAction* mUnsplitAction;
    QAction* mClearConsoleAction;

    QMenu* mWindowMenu;
    QAction* mTileAction;
    QAction* mCascadeAction;
    QAction* mTabAction;
    QAction* mPreviousAction;
    QAction* mNextAction;
    QAction* mSeparatorAction;

    QMenu* mHelpMenu;
    QAction* mAboutAction;
    QAction* mAboutQtAction;

    QToolBar* mFileToolbar;
    QToolBar* mViewToolbar;
};

inline Document* MainWindow::getCurrentDocument()
    {return mMainWindow->currentDocument();}

}
}

#endif // TYREX_MAIN_WINDOW_HPP
