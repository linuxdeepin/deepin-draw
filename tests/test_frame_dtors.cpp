// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for window/graphics-item destructors (issue V-8787)
 *
 * Target destructors:
 *   - MainWindow::~MainWindow()       (src/frame/mainwindow.cpp)
 *   - CGraphicsItem::~CGraphicsItem()  (src/drawshape/drawItems/bzItems/cgraphicsitem.cpp)
 *   - DrawBoard / Page / PageView destructor paths
 *
 * MainWindow::~MainWindow deletes all registered tools and clears the
 * command stack. The harness MainWindow is a singleton that lives for
 * the entire process, so we cannot destruct it directly. Instead we
 * verify the cleanup operations it calls are safe.
 *
 * CGraphicsItem is abstract; we use CGraphicsRectItem (a concrete
 * subclass) to exercise the base-class destructor.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <memory>

#define protected public
#define private public
#include "publicApi.h"
#include "cgraphicsrectitem.h"
#include "cundoredocommand.h"
#undef protected
#undef private

#include "test_helpers.h"

// =========================================================================
// MainWindow::~MainWindow — verify cleanup code paths
//
// MainWindow::~MainWindow does:
//   1. foreach tool in CDrawToolFactory::allTools(): delete tool
//   2. CDrawToolFactory::allTools().clear()
//   3. CUndoRedoCommand::clearCommand()
//
// The harness MainWindow is a global singleton; we cannot destroy it.
// We verify the cleanup operations it calls are safe to invoke.
// =========================================================================

TEST(MainWindowDestructor, AllToolsMap_IsAccessible)
{
    // MainWindow::~MainWindow iterates allTools() and deletes each.
    // Verify the map is accessible and non-empty (tools registered at startup).
    auto &tools = CDrawToolFactory::allTools();
    EXPECT_FALSE(tools.isEmpty());
}

TEST(MainWindowDestructor, ClearCommand_IsSafe)
{
    // MainWindow::~MainWindow calls CUndoRedoCommand::clearCommand().
    // Verify calling it does not crash when the command stack is empty.
    CUndoRedoCommand::clearCommand();
    SUCCEED();
}

TEST(MainWindowDestructor, HarnessInstance_IsValid)
{
    // The harness MainWindow will be destroyed at process exit,
    // exercising ~MainWindow. Verify it is a valid widget now.
    MainWindow *mw = getMainWindow();
    ASSERT_NE(mw, nullptr);
    EXPECT_TRUE(mw->isWidgetType());

    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);
}

// =========================================================================
// CGraphicsItem::~CGraphicsItem — via concrete subclass CGraphicsRectItem
// =========================================================================

TEST(CGraphicsItemDestructor, StandaloneItem_Destruct_NoCrash)
{
    // CGraphicsItem is abstract; use CGraphicsRectItem to exercise
    // the base-class destructor which deletes _cachePixmap and _pPrivate.
    auto *item = new CGraphicsRectItem(nullptr);
    ASSERT_NE(item, nullptr);

    delete item;
    SUCCEED();
}

TEST(CGraphicsItemDestructor, ItemWithCache_Destruct_NoCrash)
{
    // Exercise the destructor with cache mode enabled.
    // setCacheMode triggers internal cache pixmap usage; destroying the
    // item after cache is active exercises the _cachePixmap delete path.
    auto *item = new CGraphicsRectItem(nullptr);
    ASSERT_NE(item, nullptr);

    // Enable device-coordinate cache so _cachePixmap may be allocated
    // during paint; the destructor must handle cleanup safely.
    item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    delete item; // should clean up _cachePixmap without crash
    SUCCEED();
}

TEST(CGraphicsItemDestructor, ItemOnScene_Destruct_NoCrash)
{
    QGraphicsScene scene;
    auto *item = new CGraphicsRectItem(nullptr);
    ASSERT_NE(item, nullptr);

    scene.addItem(item);

    scene.removeItem(item);
    delete item;
    SUCCEED();
}

TEST(CGraphicsItemDestructor, SceneDestruction_CleansItems)
{
    // When a QGraphicsScene is destroyed, it deletes all items on it.
    {
        QGraphicsScene scene;
        auto *item1 = new CGraphicsRectItem(nullptr);
        auto *item2 = new CGraphicsRectItem(nullptr);
        scene.addItem(item1);
        scene.addItem(item2);
        // scene destructor deletes item1 and item2 → ~CGraphicsItem
    }
    SUCCEED();
}

TEST(CGraphicsItemDestructor, Reconstruct_AfterDestruct_NoCrash)
{
    auto *item1 = new CGraphicsRectItem(nullptr);
    delete item1;

    auto *item2 = new CGraphicsRectItem(nullptr);
    ASSERT_NE(item2, nullptr);
    delete item2;
    SUCCEED();
}

// =========================================================================
// Page / DrawBoard / PageView destructor paths
// =========================================================================

class FrameDtorTest : public PageStateGuard
{
};

TEST_F(FrameDtorTest, ClosePage_ExercisesPageDtor)
{
    MainWindow *mw = getMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);

    // Add a page, then close it — exercises ~Page, ~PageView.
    int countBefore = board->count();
    board->addPage("dtor-test-page");
    ASSERT_EQ(board->count(), countBefore + 1);

    // Close the added page.
    Page *lastPage = board->page(board->count() - 1);
    ASSERT_NE(lastPage, nullptr);
    lastPage->close(true);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    EXPECT_EQ(board->count(), countBefore);
}

TEST_F(FrameDtorTest, DrawBoardInstance_IsValid)
{
    // Verify the DrawBoard instance is valid; its destructor (~DrawBoard)
    // runs at process exit.
    MainWindow *mw = getMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);
    EXPECT_NE(board->_pPrivate, nullptr);
}

TEST_F(FrameDtorTest, PageViewInstance_IsValid)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    EXPECT_TRUE(view->isWidgetType());
}
