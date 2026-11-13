// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

// Keep gtest first, before publicApi.h may be pulled in under access-
// adjusting macros by including test files.
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QEvent>
#include <QImage>
#include <QList>
#include <QStackedWidget>

#include "publicApi.h"

// Shared test helpers for suites that operate on the harness MainWindow
// created in QTestMain's constructor (tests/test_qtestmain.cpp).

// ensurePageExists() is declared in publicApi.h (included above) and defined
// in tests/testItems/publicApi.cpp; getCurView() calls it automatically, so
// every test using getCurView() is guaranteed a non-null current page.

// Fixture that keeps the harness DrawBoard's page state clean between
// tests: SetUp() guarantees a baseline page exists and records the page
// count; TearDown() closes every page a test added so the next test
// starts from the same baseline state. Suites opt in by deriving an
// empty class from this one, e.g.
//     class MySuite : public PageStateGuard {};
// which keeps the original suite name (and GTEST_FILTER patterns) intact.
class PageStateGuard : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ensurePageExists();
        MainWindow *mw = (drawApp != nullptr) ? drawApp->topMainWindow() : nullptr;
        ASSERT_NE(mw, nullptr);
        DrawBoard *board = mw->drawBoard();
        ASSERT_NE(board, nullptr);
        m_baselineCount = board->count();
    }

    void TearDown() override
    {
        if (drawApp == nullptr)
            return;
        MainWindow *mw = drawApp->topMainWindow();
        if (mw == nullptr)
            return;
        DrawBoard *board = mw->drawBoard();
        if (board == nullptr)
            return;
        // Close the pages added during the test, stopping if one refuses so
        // the loop cannot spin forever.
        //
        // Locate pages through the board's QStackedWidget rather than
        // DrawBoard::page(count - 1): the latter indexes the parallel tab bar
        // (page(i) == page(_topTabs->key(i))), and after a series of rapid
        // closes that lookup can transiently miss a live stack page
        // (out-of-range/empty key -> page("") == nullptr) while count() is
        // still positive. The stack is the authoritative page list.
        while (board->count() > m_baselineCount) {
            QStackedWidget *stack = nullptr;
            const QList<QStackedWidget *> stacks = board->findChildren<QStackedWidget *>();
            for (QStackedWidget *s : stacks) {
                if (s->parentWidget() == board) {
                    stack = s;
                    break;
                }
            }
            if (stack == nullptr || stack->count() <= 0)
                break;
            Page *page = qobject_cast<Page *>(stack->widget(stack->count() - 1));
            if (page == nullptr || !page->close(true))
                break;
            // Run the board's queued autoClose MetaCall now, under our
            // control, instead of letting it land inside a later iteration.
            QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
        }
    }

    int m_baselineCount = 0;
};

#endif // TEST_HELPERS_H
