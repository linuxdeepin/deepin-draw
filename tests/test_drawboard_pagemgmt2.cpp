// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawBoard page management and tool methods (issue V-8176)
 *
 * Source: src/frame/ccentralwidget.cpp / .h
 *
 * Target methods (0-coverage per existing .gcda):
 *   DrawBoard::addPage(const QImage &img)
 *   DrawBoard::closePage(const QString &key)
 *   DrawBoard::setCurrentTool(int tool)
 *   Page::setName(const QString &name)
 */

#include <gtest/gtest.h>

#include <QImage>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"
#undef protected
#undef private

#include "test_helpers.h"

// Shared fixture (see test_helpers.h) that restores the DrawBoard page
// state after every test; the suite name is kept for GTEST_FILTER stability.
class DrawBoardPageMgmt2 : public PageStateGuard {};


// =========================================================================
// DrawBoard::addPage(const QImage&)
// =========================================================================

TEST_F(DrawBoardPageMgmt2, AddPageValidImageReturnsNonNull)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    int before = board->count();

    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    Page *page = board->addPage(img);
    EXPECT_NE(page, nullptr);

    // A new page should have been added
    EXPECT_EQ(board->count(), before + 1);
}

TEST_F(DrawBoardPageMgmt2, AddPageNullImageReturnsNull)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    QImage nullImg;
    Page *page = board->addPage(nullImg);
    EXPECT_EQ(page, nullptr);
}

// =========================================================================
// DrawBoard::closePage(const QString &key)
// =========================================================================

TEST_F(DrawBoardPageMgmt2, ClosePageByKeyDecreasesCount)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Add a page to close
    QImage img(50, 50, QImage::Format_ARGB32);
    img.fill(Qt::red);
    Page *page = board->addPage(img);
    ASSERT_NE(page, nullptr);

    QString key = page->key();
    EXPECT_FALSE(key.isEmpty());

    int before = board->count();
    bool result = board->closePage(key);
    EXPECT_TRUE(result);

    // Page::close() -> closeEvent() calls closePageHelper() synchronously,
    // which removes the page from the board's stack widget immediately.
    EXPECT_EQ(board->count(), before - 1);
}

TEST_F(DrawBoardPageMgmt2, ClosePageInvalidKeyReturnsNullPage)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // page(key) returns nullptr for a non-existent key.
    // NOTE: closePage(key) would crash because it calls closePage(nullptr)
    // which dereferences the null pointer — a source defect in
    // DrawBoard::closePage(const QString&) that we do not fix here.
    Page *page = board->page(QString("nonexistent-key-12345"));
    EXPECT_EQ(page, nullptr);
}

// =========================================================================
// DrawBoard::setCurrentTool(int)
// =========================================================================

TEST_F(DrawBoardPageMgmt2, SetCurrentToolReturnsTrue)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // selection tool (typically 1) should be settable
    bool result = board->setCurrentTool(selection);
    EXPECT_TRUE(result);
}

TEST_F(DrawBoardPageMgmt2, SetCurrentToolInvalidReturnsFalse)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // An invalid tool ID should return false
    bool result = board->setCurrentTool(-1);
    EXPECT_FALSE(result);
}

// =========================================================================
// Page::setName(const QString&)
// =========================================================================

TEST_F(DrawBoardPageMgmt2, PageSetNameReadsBack)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    page->setName("TestPageName");
    EXPECT_EQ(page->name().toStdString(), "TestPageName");
}

TEST_F(DrawBoardPageMgmt2, PageSetNameEmpty)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);

    page->setName("");
    EXPECT_TRUE(page->name().isEmpty());
}
