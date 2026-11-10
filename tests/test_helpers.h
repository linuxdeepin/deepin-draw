// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

// Keep gtest first, before publicApi.h may be pulled in under access-
// adjusting macros by including test files.
#include <gtest/gtest.h>

#include <QImage>

#include "publicApi.h"

// Shared test helpers for suites that operate on the harness MainWindow
// created in QTestMain's constructor (tests/test_qtestmain.cpp).

// Ensure at least one page exists so DrawBoard::currentPage() is non-null.
// Null-checks drawApp / topMainWindow() so the helper cannot dereference a
// null pointer if the harness failed to create the main window.
inline void ensurePageExists()
{
    if (drawApp == nullptr)
        return;
    MainWindow *mw = drawApp->topMainWindow();
    if (mw == nullptr)
        return;
    DrawBoard *board = mw->drawBoard();
    if (board && board->currentPage() == nullptr) {
        QImage img(100, 100, QImage::Format_ARGB32);
        img.fill(Qt::white);
        board->addPage(img);
    }
}

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
        // Close the pages added during the test (count() drops
        // synchronously per closePage), stopping if one refuses so the
        // loop cannot spin forever.
        while (board->count() > m_baselineCount) {
            Page *page = board->page(board->count() - 1);
            if (page == nullptr || !board->closePage(page->key()))
                break;
        }
    }

    int m_baselineCount = 0;
};

#endif // TEST_HELPERS_H
