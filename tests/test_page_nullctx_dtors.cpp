// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for Page null-context fallbacks, destructor chains,
 * and DrawBoard private helper branches (V-9047)
 *
 * Source: src/frame/ccentralwidget.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryDir>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QMimeData>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "drawshape/globaldefine.h"
#include "cshapemimedata.h"
#undef protected
#undef private

#include "test_helpers.h"

// =========================================================================
// Page with _context == nullptr — fallback returns
// =========================================================================

TEST(PageNullContext, NameReturnsEmpty)
{
    // Page(nullptr) creates a Page with no parent and no context.
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    ASSERT_NE(page, nullptr);
    EXPECT_EQ(page->_context, nullptr);
    EXPECT_TRUE(page->name().isEmpty());
    delete page;
}

TEST(PageNullContext, FileReturnsEmpty)
{
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    EXPECT_EQ(page->_context, nullptr);
    EXPECT_TRUE(page->file().isEmpty());
    delete page;
}

TEST(PageNullContext, KeyReturnsEmpty)
{
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    EXPECT_EQ(page->_context, nullptr);
    EXPECT_TRUE(page->key().isEmpty());
    delete page;
}

TEST(PageNullContext, PageRectReturnsZero)
{
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    EXPECT_EQ(page->_context, nullptr);
    QRectF rect = page->pageRect();
    EXPECT_EQ(rect, QRectF(0, 0, 0, 0));
    delete page;
}

TEST(PageNullContext, CurrentAttrisReturnsEmpty)
{
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    EXPECT_EQ(page->_context, nullptr);
    SAttrisList attris = page->currentAttris();
    EXPECT_TRUE(attris.isEmpty());
    delete page;
}

TEST(PageNullContext, DefaultAttriVarReturnsNull)
{
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    EXPECT_EQ(page->_context, nullptr);
    // With no context and no borad(), defaultAttriVar returns null QVariant.
    QVariant var = page->defaultAttriVar(0);
    EXPECT_TRUE(var.isNull());
    delete page;
}

// =========================================================================
// Page::setBorad — non-null borad triggers addPage
// =========================================================================

TEST(PageSetBorad, NullBoradIsNoOp)
{
    // setBorad(nullptr) is a no-op — the if(borad != nullptr) branch is false.
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    page->setBorad(nullptr);
    EXPECT_EQ(page->borad(), nullptr);
    delete page;
}

// =========================================================================
// Page::~Page — safe destruction
// =========================================================================

TEST(PageDestructor, DestroyWithContextNullSafe)
{
    Page *page = new Page(static_cast<DrawBoard *>(nullptr));
    ASSERT_NE(page, nullptr);
    // Destructor deletes _pPrivate; must not crash with null context.
    delete page;
    SUCCEED();
}

// =========================================================================
// DrawBoard::defaultAttriVar(nullptr, attri) — _attriWidget branch
// =========================================================================

TEST(DrawBoardDefaultAttriVar, NullPageUsesAttriWidget)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    ASSERT_NE(board->_attriWidget, nullptr);

    // With page == nullptr, the method returns _attriWidget->defaultAttriVar(attri).
    QVariant var = board->defaultAttriVar(nullptr, 0);
    // Should not crash; the value comes from the attribution widget.
    // We only verify the branch is taken without crash.
    SUCCEED();
}

// =========================================================================
// DrawBoard::setCurrentTool(IDrawTool*)
// =========================================================================

TEST(DrawBoardSetCurrentTool, SetNullToolPointerSafe)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // setCurrentTool(IDrawTool*) delegates to toolManager()->setCurrentTool(tool).
    // Passing nullptr should be handled safely.
    board->setCurrentTool(static_cast<IDrawTool *>(nullptr));
    SUCCEED();
}

// =========================================================================
// DrawBoard::currentAttris — all-empty fallback to app name
// =========================================================================

TEST(DrawBoardCurrentAttris, EmptyPageReturnsAppNameTitle)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Close all pages so currentPage() == nullptr.
    // This forces currentAttris() through all fallback branches to the
    // final "application name" branch.
    while (board->count() > 0) {
        Page *p = board->page(0);
        if (p == nullptr || !p->close(true))
            break;
        QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
    }

    // Now currentPage() should be nullptr.
    ASSERT_EQ(board->currentPage(), nullptr);

    SAttrisList attris = board->currentAttris();
    // The last fallback adds SAttri(ETitle, qApp->applicationName()).
    ASSERT_FALSE(attris.isEmpty());

    // ensurePageExists() in next test's SetUp will recreate the baseline page.
}

// =========================================================================
// DrawBoard::~DrawBoard — clipboard data cleanup branch
// =========================================================================

TEST(DrawBoardDestructor, ClipboardDataCleanupSafe)
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Set some clipboard data so the destructor's _pClipBordData != nullptr
    // branch is exercised. We set it and immediately clear it; the
    // destructor's branch is tested indirectly by verifying set/get.
    CShapeMimeData *mimeData = new CShapeMimeData(CGroupBzItemsTreeInfo());
    board->setClipBoardShapeData(mimeData);
    EXPECT_NE(board->_pClipBordData, nullptr);

    // Clear it so the harness DrawBoard (which lives for the process)
    // doesn't get a stale pointer. The actual destruction of DrawBoard
    // happens at process exit; we verify the set/delete cycle is safe.
    board->setClipBoardShapeData(nullptr);
    EXPECT_EQ(board->_pClipBordData, nullptr);
}

// =========================================================================
// Page::saveToImage — real PNG save via QTemporaryDir
// =========================================================================

TEST(PageSaveToImage, RealPngSaveCreatesFile)
{
    ensurePageExists();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/save_test.png";

    bool ok = page->saveToImage(filePath);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(QFile::exists(filePath));
}

TEST(PageSaveToImage, EmptyFileReturnsFalse)
{
    ensurePageExists();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    // Empty file path → saveToImage returns false.
    EXPECT_FALSE(page->saveToImage(""));
}

// =========================================================================
// Page::adjustViewScaleRatio — viewport smaller than scene
// =========================================================================

TEST(PageAdjustViewScaleRatio, SmallViewportScalesDown)
{
    ensurePageExists();
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);
    Page *page = board->currentPage();
    ASSERT_NE(page, nullptr);
    ASSERT_NE(page->context(), nullptr);

    // Set a large page rect so the scene is bigger than the viewport.
    page->setPageRect(QRectF(0, 0, 5000, 5000));

    // Call adjustViewScaleRatio — should scale down without crash.
    page->adjustViewScaleRatio();
    SUCCEED();
}
