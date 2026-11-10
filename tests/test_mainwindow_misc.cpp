// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for MainWindow miscellaneous methods (issue V-8176)
 *
 * Source: src/frame/mainwindow.cpp
 *
 * Target methods (0-coverage per existing .gcda):
 *   MainWindow::loadFiles(const QStringList &)
 *   MainWindow::closeEvent(QCloseEvent *)
 *   MainWindow::dragMoveEvent(QDragMoveEvent *)
 *   MainWindow::onViewShortcut()
 */

#include <gtest/gtest.h>

#include <QApplication>
#include <QCloseEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QUrl>
#include <QStringList>

#define protected public
#define private public
#include "publicApi.h"
#include "mainwindow.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#include "test_helpers.h"

// Shared fixture (see test_helpers.h) that restores the DrawBoard page
// state after every test; the suite name is kept for GTEST_FILTER stability.
class MainWindowMisc : public PageStateGuard {};

// =========================================================================
// MainWindow::loadFiles
// =========================================================================

TEST_F(MainWindowMisc, LoadFilesEmptyListNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    mw->loadFiles(QStringList());

    // loadFiles routes through Application::execPicturesLimit, which raises
    // its modal warning DDialog only when more than 30 supported pictures
    // are requested (src/application.cpp). An empty list must not leave a
    // modal widget behind.
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

TEST_F(MainWindowMisc, LoadFilesNonexistentPathNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    QStringList files;
    files << "/nonexistent/path/file.png";

    // Precondition for "no modal dialog": execPicturesLimit warns (via a
    // modal DDialog::exec()) only above 30 supported pictures.
    ASSERT_LE(files.count(), 30);

    mw->loadFiles(files);
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

TEST_F(MainWindowMisc, LoadFilesMultiplePathsNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    QStringList files;
    files << "/tmp/test1.png" << "/tmp/test2.jpg" << "/tmp/test3.bmp";

    // All three suffixes are supported pictures; keep the count at or
    // below the 30-picture threshold so execPicturesLimit returns without
    // popping its modal warning dialog.
    ASSERT_LE(files.count(), 30);

    mw->loadFiles(files);
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// =========================================================================
// MainWindow::closeEvent
// =========================================================================

TEST_F(MainWindowMisc, CloseEventIsIgnored)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    QCloseEvent event;
    mw->closeEvent(&event);

    // closeEvent calls event->ignore()
    EXPECT_FALSE(event.isAccepted());
}

TEST_F(MainWindowMisc, CloseEventNullNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    // closeEvent with nullptr should return early (first line checks)
    mw->closeEvent(nullptr);
    SUCCEED();
}

// =========================================================================
// MainWindow::dragMoveEvent
// =========================================================================

TEST_F(MainWindowMisc, DragMoveEventWithUrlsAccepted)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    QMimeData mimeData;
    QList<QUrl> urls;
    urls << QUrl("file:///tmp/test.png");
    mimeData.setUrls(urls);

    // QRect(0,0,10,10) as the rect, MoveAction
    QDragMoveEvent event(QPoint(5, 5), Qt::MoveAction, &mimeData,
                         Qt::LeftButton, Qt::NoModifier);
    mw->dragMoveEvent(&event);

    EXPECT_TRUE(event.isAccepted());
}

TEST_F(MainWindowMisc, DragMoveEventWithoutUrlsNotAccepted)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    QMimeData mimeData;
    // No URLs set

    QDragMoveEvent event(QPoint(5, 5), Qt::MoveAction, &mimeData,
                         Qt::LeftButton, Qt::NoModifier);
    mw->dragMoveEvent(&event);

    // Falls through to DWidget::dragMoveEvent which does not accept
    EXPECT_FALSE(event.isAccepted());
}

// =========================================================================
// MainWindow::onViewShortcut
// =========================================================================

TEST_F(MainWindowMisc, OnViewShortcutNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    // Starts a detached process (deepin-shortcut-viewer); may fail silently
    mw->onViewShortcut();
    SUCCEED();
}
