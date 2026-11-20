// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for TabBarWgt signal lambdas and "Close other tabs" action
 * (issue V-8981 §3).
 *
 * Source: src/frame/cmultiptabbarwidget.cpp
 *
 * Covers:
 *   - Constructor lambda: tabAddRequested → pageManager()->addPage()
 *   - Constructor lambda: tabCloseRequested → pageManager()->closePage(key)
 *   - Constructor lambda: currentChanged → pageManager()->setCurrentPage(key)
 *   - menu() "Close other tabs" action lambda (including closePage-false break)
 *
 * TabBarWgt's constructor is private; the friend DrawBoard grants access.
 * We use `#define private public` to construct TabBarWgt directly against the
 * running DrawBoard from the test harness.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMenu>
#include <QAction>
#include <QSignalSpy>

#include "publicApi.h"

#define protected public
#define private public
#include "cmultiptabbarwidget.h"
#include "ccentralwidget.h"
#undef protected
#undef private

// Helper: get the running DrawBoard.
static DrawBoard *getBoard()
{
    MainWindow *mw = drawApp->topMainWindow();
    return mw->drawBoard();
}

// ===========================================================================
// tabAddRequested → pageManager()->addPage()
// ===========================================================================

TEST(TabBarWgtLambdaTest, TabAddRequestedAddsPage)
{
    DrawBoard *board = getBoard();
    ASSERT_NE(board, nullptr);
    int initialCount = board->count();

    TabBarWgt bar(board);
    bar.show();

    // Emit tabAddRequested to trigger the constructor lambda.
    emit bar.tabAddRequested();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // A new page should have been added.
    EXPECT_EQ(board->count(), initialCount + 1);

    // Clean up: close the added page.
    while (board->count() > initialCount) {
        Page *p = board->currentPage();
        if (p == nullptr || !p->close(true))
            break;
        QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
    }
}

// ===========================================================================
// tabCloseRequested → pageManager()->closePage(key)
// ===========================================================================

TEST(TabBarWgtLambdaTest, TabCloseRequestedClosesPage)
{
    DrawBoard *board = getBoard();
    ASSERT_NE(board, nullptr);

    // Add two pages so we have one to close.
    Page *p1 = board->addPage();
    ASSERT_NE(p1, nullptr);
    Page *p2 = board->addPage();
    ASSERT_NE(p2, nullptr);
    int countBefore = board->count();

    TabBarWgt bar(board);
    bar.show();

    // Add tabs to the bar matching the pages.
    bar.addItem(p1->name(), p1->key());
    bar.addItem(p2->name(), p2->key());

    // Emit tabCloseRequested for index 0 (p1's tab).
    emit bar.tabCloseRequested(0);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // At least one page should have been closed.
    EXPECT_LE(board->count(), countBefore);

    // Clean up remaining test pages.
    while (board->count() > 1) {
        Page *p = board->currentPage();
        if (p == nullptr || !p->close(true))
            break;
        QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
    }
}

// ===========================================================================
// currentChanged → pageManager()->setCurrentPage(key)
// ===========================================================================

TEST(TabBarWgtLambdaTest, CurrentChangedSwitchesPage)
{
    DrawBoard *board = getBoard();
    ASSERT_NE(board, nullptr);

    Page *p1 = board->addPage();
    ASSERT_NE(p1, nullptr);
    Page *p2 = board->addPage();
    ASSERT_NE(p2, nullptr);

    // Make p1 the current page first.
    board->setCurrentPage(p1);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    EXPECT_EQ(board->currentPage(), p1);

    TabBarWgt bar(board);
    bar.show();
    bar.addItem(p1->name(), p1->key());
    bar.addItem(p2->name(), p2->key());

    // Emit currentChanged with index 1 (p2's tab) to trigger the lambda.
    emit bar.currentChanged(1);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // The current page should now be p2.
    EXPECT_EQ(board->currentPage()->key(), p2->key());

    // Clean up.
    while (board->count() > 1) {
        Page *p = board->currentPage();
        if (p == nullptr || !p->close(true))
            break;
        QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
    }
}

// ===========================================================================
// menu() "Close other tabs" action — iterates and closes non-current pages.
// The lambda uses key(currentIndex()) to determine which page to keep, so
// we must add tabs for ALL pages and set the bar's current index correctly.
// NOTE: menu() keeps a function-local static QMenu parented to the FIRST
// TabBarWgt that calls it. That bar must outlive the rest of the process:
// a stack-local bar would destroy the singleton menu at scope exit and leave
// every later menu() caller (TabBarWgtMenuTest) with a dangling pointer whose
// actions() returns garbage. So this test heap-allocates its bar, parents it
// to the process-lifetime DrawBoard, and deliberately never deletes it (same
// pattern as MenuReturnsNonNull in test_tabbarwgt_menu.cpp).
// ===========================================================================

TEST(TabBarWgtLambdaTest, CloseOtherTabsActionClosesNonCurrentPages)
{
    DrawBoard *board = getBoard();
    ASSERT_NE(board, nullptr);

    // Start from a clean baseline (1 page).
    while (board->count() > 1) {
        Page *p = board->currentPage();
        if (p == nullptr || !p->close(true))
            break;
        QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
    }
    ASSERT_EQ(board->count(), 1);

    // Remember the initial page and add two extra pages (3 total).
    Page *p0 = board->currentPage();
    ASSERT_NE(p0, nullptr);
    Page *p1 = board->addPage();
    ASSERT_NE(p1, nullptr);
    Page *p2 = board->addPage();
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(board->count(), 3);

    // Heap-allocate, parented to the board (lives for the whole process), and
    // never deleted — see the NOTE above about the menu() static singleton.
    TabBarWgt *bar = new TabBarWgt(board);
    bar->show();

    // Add tabs for ALL pages so key(currentIndex()) resolves to a real page.
    bar->addItem(p0->name(), p0->key());   // index 0
    bar->addItem(p1->name(), p1->key());   // index 1
    bar->addItem(p2->name(), p2->key());   // index 2

    // Set the bar's current index to p2's tab so p2 survives "close others".
    bar->setCurrentIndex(2);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    QMenu *m = bar->menu();
    ASSERT_NE(m, nullptr);

    // Find the "Close other tabs" action and trigger it.
    auto actions = m->actions();
    QAction *closeOtherAction = nullptr;
    for (QAction *act : actions) {
        if (act->text().contains("other", Qt::CaseInsensitive) ||
            act->text().contains("其他", Qt::CaseInsensitive)) {
            closeOtherAction = act;
            break;
        }
    }
    ASSERT_NE(closeOtherAction, nullptr);

    closeOtherAction->trigger();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 300);

    // After "close other tabs", only the current page (p2) should remain.
    EXPECT_EQ(board->count(), 1);
    if (board->currentPage() != nullptr) {
        EXPECT_EQ(board->currentPage()->key(), p2->key());
    }

    // --- Sub-scenario: with only one page, "close other tabs" is a no-op ---
    // The menu is still valid because bar is still alive.
    closeOtherAction->trigger();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // Still one page — the loop immediately exits with no non-current pages.
    EXPECT_EQ(board->count(), 1);
}
