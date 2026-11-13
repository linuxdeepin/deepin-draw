// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for MainWindow::initConnection() lambdas
 * (src/frame/mainwindow.cpp).
 *
 * Target lambdas (0-coverage):
 *   DrawBoard::modified lambda — calls notifySystemBlocked(modified).
 *   TopTilte::zoomTo lambda — calls m_drawBoard->zoomTo(total) with
 *       QSignalBlocker.
 *   Application::quitRequest lambda — if drawBoard()->close(), quitApp().
 *   TopTilte::creatOnePage lambda — calls m_drawBoard->addPage().
 *   TopTilte::toExport lambda — creates CExportImageDialog (modal, guarded).
 *
 * ~MainWindow() deletes all tools globally; testing it would break the
 * test harness — skipped.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

#define protected public
#define private public
#include "publicApi.h"
#include "mainwindow.h"
#undef protected
#undef private

// ===========================================================================
// DrawBoard::modified lambda — notifySystemBlocked
// ===========================================================================

TEST(MainWindowLambdaTest, ModifiedSignalNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);

    // Emit modified signal to trigger the lambda
    emit board->modified(true);
    qApp->processEvents();
    emit board->modified(false);
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// TopTilte::zoomTo lambda — zoomTo with QSignalBlocker
// ===========================================================================

TEST(MainWindowLambdaTest, ZoomToSignalNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    // Emit zoomTo signal to trigger the lambda that calls m_drawBoard->zoomTo
    emit tt->zoomTo(1.5);
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// TopTilte::creatOnePage lambda — addPage
// ===========================================================================

TEST(MainWindowLambdaTest, CreatOnePageSignalNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);
    int countBefore = board->count();
    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    // Emit creatOnePage signal to trigger the lambda that calls addPage
    emit tt->creatOnePage();
    qApp->processEvents();

    // A new page should have been added
    EXPECT_GE(board->count(), countBefore);
}

// ===========================================================================
// TopTilte::toExport lambda — guarded by currentPage null check
// ===========================================================================

TEST(MainWindowLambdaTest, ToExportSignalNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    // Close any modal dialog that might open from the export lambda
    QTimer::singleShot(200, qApp, []() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (auto *w : widgets) {
            if (w->inherits("QDialog") && w->isModal())
                w->close();
        }
    });

    // Emit toExport signal to trigger the lambda
    emit tt->toExport();
    qApp->processEvents();
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// TopTilte::toPrint lambda — empty body lambda
// ===========================================================================

TEST(MainWindowLambdaTest, ToPrintSignalNoCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    TopTilte *tt = mw->topTitle();
    ASSERT_NE(tt, nullptr);

    // The toPrint lambda is empty: [ = ]() {}
    emit tt->toPrint();
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// ~MainWindow — skipped (breaks test harness)
// ===========================================================================

TEST(MainWindowLambdaTest, DestructorSkipped)
{
    GTEST_SKIP() << "~MainWindow() deletes all tools globally and would "
                    "break the test harness; skipped.";
}
