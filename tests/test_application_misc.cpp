// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for Application miscellaneous methods (issue V-8176)
 *
 * Source: src/application.cpp
 *
 * Target methods (0-coverage per existing .gcda):
 *   Application::currentTool()
 *   Application::openFiles(...)
 *   Application::onAppQuit()
 */

#include <gtest/gtest.h>

#include <QSignalSpy>
#include <QImage>

#define protected public
#define private public
#include "publicApi.h"
#include "application.h"
#include "ccentralwidget.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#include "test_helpers.h"

// =========================================================================
// Application::currentTool()
// =========================================================================

TEST(ApplicationMisc, CurrentToolReturnsValidAfterPageCreation)
{
    ensurePageExists();
    int tool = drawApp->currentTool();
    EXPECT_GE(tool, 0);
}

TEST(ApplicationMisc, CurrentToolReturnsSelectionDefault)
{
    ensurePageExists();
    int tool = drawApp->currentTool();
    EXPECT_EQ(tool, selection);
}

// =========================================================================
// Application::openFiles(...)
// =========================================================================

TEST(ApplicationMisc, OpenFilesEmptyListNoCrash)
{
    ensurePageExists();
    drawApp->openFiles(QStringList(), true, true, true, true);
    SUCCEED();
}

TEST(ApplicationMisc, OpenFilesWithPathsNoCrash)
{
    ensurePageExists();
    QStringList files;
    files << "/tmp/test1.png" << "/tmp/test2.jpg";
    drawApp->openFiles(files, false, false, false, false);
    SUCCEED();
}

TEST(ApplicationMisc, OpenFilesMixedFlagsNoCrash)
{
    ensurePageExists();
    QStringList files;
    files << "/nonexistent/file.png";
    drawApp->openFiles(files, true, false, true, false);
    SUCCEED();
}

// =========================================================================
// Application::onAppQuit()
// =========================================================================
// onAppQuit() only emits the quitRequest signal (src/application.cpp);
// it neither quits the process nor invalidates application state, so
// these tests are safe to run in any order.

TEST(ApplicationMisc, OnAppQuitEmitsQuitRequest)
{
    ensurePageExists();

    QSignalSpy spy(drawApp, &Application::quitRequest);
    drawApp->onAppQuit();

    EXPECT_EQ(spy.count(), 1);
}

TEST(ApplicationMisc, OnAppQuitNoCrashWithoutConnections)
{
    ensurePageExists();
    drawApp->onAppQuit();
    SUCCEED();
}
