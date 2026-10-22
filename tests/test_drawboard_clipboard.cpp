// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMimeData>
#include <QCursor>

#define protected public
#define private public
#include "publicApi.h"
#include "ccentralwidget.h"
#include "globaldefine.h"
#include "drawshape/cshapemimedata.h"
#include "drawshape/sitemdata.h"
#include "cattributemanagerwgt.h"

/*
 * DrawBoard clipboard / touch / auto-close / attribution tests (V-6123)
 *
 * Covers: setClipBoardShapeData / clipBoardShapeData,
 *         setTouchFeelingEnhanceValue / touchFeelingEnhanceValue,
 *         setAutoClose / isAutoClose,
 *         setAttributionWidget / attributionWidget
 */

// ---------------------------------------------------------------------------
// setClipBoardShapeData / clipBoardShapeData
// ---------------------------------------------------------------------------

TEST(DrawBoardClipBoardTest, SetAndRetrieveShapeData)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    CGroupBzItemsTreeInfo treeInfo;
    auto *mimeData = new CShapeMimeData(treeInfo);

    board->setClipBoardShapeData(mimeData);

    // clipBoardShapeData returns _pClipBordData when set.
    QMimeData *retrieved = board->clipBoardShapeData();
    EXPECT_NE(retrieved, nullptr);
}

TEST(DrawBoardClipBoardTest, SetNullClearsPreviousData)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Set a valid CShapeMimeData first.
    CGroupBzItemsTreeInfo treeInfo;
    auto *mimeData = new CShapeMimeData(treeInfo);
    board->setClipBoardShapeData(mimeData);
    EXPECT_NE(board->clipBoardShapeData(), nullptr);

    // Setting nullptr clears _pClipBordData.
    board->setClipBoardShapeData(nullptr);

    // _pClipBordData is cleared. Check the member directly:
    // clipBoardShapeData() falls back to the system clipboard, whose
    // mimeData() is non-null on platforms with clipboard support.
    EXPECT_EQ(board->_pClipBordData, nullptr);
}

TEST(DrawBoardClipBoardTest, NonShapeMimeDataIsIgnored)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // A plain QMimeData (not CShapeMimeData) should not be stored.
    board->setClipBoardShapeData(nullptr); // clear first

    auto *plainData = new QMimeData();
    plainData->setText("test");
    board->setClipBoardShapeData(plainData);

    // _pClipBordData stays null (plain QMimeData rejected). Check the
    // member directly: the system-clipboard fallback in clipBoardShapeData()
    // is non-null on platforms with clipboard support.
    EXPECT_EQ(board->_pClipBordData, nullptr);

    // Clean up the plain QMimeData that was rejected.
    delete plainData;
}

// ---------------------------------------------------------------------------
// setTouchFeelingEnhanceValue / touchFeelingEnhanceValue
// ---------------------------------------------------------------------------

TEST(DrawBoardTouchFeelingTest, NormalValueRoundTrip)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setTouchFeelingEnhanceValue(42);
    EXPECT_EQ(board->touchFeelingEnhanceValue(), 42);
}

TEST(DrawBoardTouchFeelingTest, ZeroValueRoundTrip)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setTouchFeelingEnhanceValue(0);
    EXPECT_EQ(board->touchFeelingEnhanceValue(), 0);
}

TEST(DrawBoardTouchFeelingTest, LargeValueRoundTrip)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setTouchFeelingEnhanceValue(1000000);
    EXPECT_EQ(board->touchFeelingEnhanceValue(), 1000000);
}

// ---------------------------------------------------------------------------
// setAutoClose / isAutoClose
// ---------------------------------------------------------------------------

TEST(DrawBoardAutoCloseTest, SetTrueReturnsTrue)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setAutoClose(true);
    EXPECT_TRUE(board->isAutoClose());
}

TEST(DrawBoardAutoCloseTest, SetFalseReturnsFalse)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    board->setAutoClose(false);
    EXPECT_FALSE(board->isAutoClose());
}

// ---------------------------------------------------------------------------
// setAttributionWidget / attributionWidget
// ---------------------------------------------------------------------------

TEST(DrawBoardAttributionWidgetTest, SetAndRetrieve)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // Save the original widget to restore later.
    auto *original = board->attributionWidget();

    auto *widget = new DrawAttribution::CAttributeManagerWgt();
    board->setAttributionWidget(widget);

    EXPECT_EQ(board->attributionWidget(), widget);

    // Restore original (or nullptr) to avoid side effects on other tests.
    board->setAttributionWidget(original);
    delete widget;
}

TEST(DrawBoardAttributionWidgetTest, SetNullRestoresNullptr)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    auto *original = board->attributionWidget();

    auto *widget = new DrawAttribution::CAttributeManagerWgt();
    board->setAttributionWidget(widget);
    EXPECT_NE(board->attributionWidget(), nullptr);

    // Set back to nullptr.
    board->setAttributionWidget(nullptr);
    EXPECT_EQ(board->attributionWidget(), nullptr);

    // Restore original.
    board->setAttributionWidget(original);
    delete widget;
}
