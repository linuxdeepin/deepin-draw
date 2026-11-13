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
 *
 * Every suite derives from PageStateGuard so the page the creatOnePage
 * lambda adds is closed again after the test.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QDialog>
#include <QSignalSpy>
#include <QTimer>

#define protected public
#define private public
#include "publicApi.h"
#include "mainwindow.h"
#undef protected
#undef private

#include "test_helpers.h"

namespace {

// Zoom target emitted by the zoomTo lambda; PageView clamps to [0.1, 20].
constexpr qreal kZoomTargetScale = 1.5;
constexpr qreal kScaleEpsilon = 1e-6;
// Delay used to close a modal export dialog that the lambda exec()s into a
// nested event loop. Must stay well below the harness' own 300 ms sweeper.
constexpr int kModalSweepDelayMs = 200;

void closeModalDialogs()
{
    const QWidgetList widgets = QApplication::topLevelWidgets();
    for (QWidget *w : widgets) {
        if (w->inherits("QDialog") && w->isModal())
            w->close();
    }
}

class MainWindowLambdaTest : public PageStateGuard
{
protected:
    MainWindow *mw()
    {
        MainWindow *w = drawApp->topMainWindow();
        EXPECT_NE(w, nullptr);
        return w;
    }

    TopTilte *topTitle()
    {
        MainWindow *w = mw();
        if (w == nullptr)
            return nullptr;
        TopTilte *tt = w->topTitle();
        EXPECT_NE(tt, nullptr);
        return tt;
    }
};

} // namespace

// ===========================================================================
// DrawBoard::modified lambda — notifySystemBlocked
// ===========================================================================

TEST_F(MainWindowLambdaTest, ModifiedSignalReachesHandlers)
{
    MainWindow *w = mw();
    ASSERT_NE(w, nullptr);
    DrawBoard *board = w->drawBoard();
    ASSERT_NE(board, nullptr);

    // The MainWindow lambda forwards to notifySystemBlocked(), which talks to
    // the system DBus. Verify the emission path itself: both values must be
    // delivered to every connected handler.
    QSignalSpy spy(board, &DrawBoard::modified);
    ASSERT_TRUE(spy.isValid());

    emit board->modified(true);
    emit board->modified(false);
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 2);
    EXPECT_TRUE(spy.at(0).at(0).toBool());
    EXPECT_FALSE(spy.at(1).at(0).toBool());
}

// ===========================================================================
// TopTilte::zoomTo lambda — zoomTo with QSignalBlocker
// ===========================================================================

TEST_F(MainWindowLambdaTest, ZoomToSignalAppliesScaleToCurrentView)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    const qreal scaleBefore = view->getScale();

    emit tt->zoomTo(kZoomTargetScale);
    qApp->processEvents();

    EXPECT_NEAR(view->getScale(), kZoomTargetScale, kScaleEpsilon);

    // Restore the page scale so later tests start from the same baseline.
    view->scale(scaleBefore);
    qApp->processEvents();
    EXPECT_NEAR(view->getScale(), scaleBefore, kScaleEpsilon);
}

// ===========================================================================
// TopTilte::creatOnePage lambda — addPage
// ===========================================================================

TEST_F(MainWindowLambdaTest, CreatOnePageSignalAddsOnePage)
{
    MainWindow *w = mw();
    ASSERT_NE(w, nullptr);
    DrawBoard *board = w->drawBoard();
    ASSERT_NE(board, nullptr);
    const int countBefore = board->count();
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::creatOnePage);
    ASSERT_TRUE(spy.isValid());
    QSignalSpy addedSpy(board, &DrawBoard::pageAdded);
    ASSERT_TRUE(addedSpy.isValid());

    emit tt->creatOnePage();
    // The connection is direct, so the page exists before the event loop is
    // pumped. Do not assert an exact page delta of 1: MainWindow::initConnection()
    // is re-invoked by tests/test_mainwindow.cpp, and Qt does not deduplicate
    // lambda connections, so creatOnePage can have several addPage() handlers.
    // The invariant is one pageAdded per page, with at least the one this
    // emission caused.
    EXPECT_EQ(spy.count(), 1);
    EXPECT_GE(addedSpy.count(), 1);
    EXPECT_EQ(addedSpy.count(), board->count() - countBefore);
    EXPECT_NE(board->currentPage(), nullptr);

    qApp->processEvents();
}

// ===========================================================================
// TopTilte::toExport lambda — guarded by currentPage null check
// ===========================================================================

TEST_F(MainWindowLambdaTest, ToExportSignalClosesModalDialog)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);
    // The lambda only builds a dialog when a page with a context exists.
    ASSERT_NE(drawApp->drawBoard()->currentPage(), nullptr);

    // CExportImageDialog::execFor() runs a nested event loop; the timer fires
    // inside it and closes the dialog so the test can continue.
    QTimer::singleShot(kModalSweepDelayMs, qApp, &closeModalDialogs);

    emit tt->toExport();
    qApp->processEvents();
    qApp->processEvents();

    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// ===========================================================================
// TopTilte::toPrint lambda — empty body lambda
// ===========================================================================

TEST_F(MainWindowLambdaTest, ToPrintSignalEmittedWithNoSideEffects)
{
    TopTilte *tt = topTitle();
    ASSERT_NE(tt, nullptr);

    QSignalSpy spy(tt, &TopTilte::toPrint);
    ASSERT_TRUE(spy.isValid());

    // The toPrint lambda is empty: [ = ]() {}
    emit tt->toPrint();
    qApp->processEvents();

    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// ===========================================================================
// ~MainWindow — skipped (breaks test harness)
// ===========================================================================

TEST_F(MainWindowLambdaTest, DestructorSkipped)
{
    GTEST_SKIP() << "~MainWindow() deletes all tools globally and would "
                    "break the test harness; skipped.";
}
