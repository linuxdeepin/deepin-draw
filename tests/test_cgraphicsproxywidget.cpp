// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsProxyWidget (issue V-7767)
 *
 * Source: src/drawshape/drawItems/cgraphicsproxywidget.cpp
 *
 * Target methods:
 *   - Constructor (sets IBeam cursor)
 *   - parentDrawItem() (dynamic_cast to CGraphicsItem)
 *   - paint() (delegates to QGraphicsProxyWidget::paint)
 *   - itemChange() (ItemFlagsHaveChanged handling)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QCursor>
#include <QUndoStack>

#define protected public
#define private public
#include "drawshape/drawItems/cgraphicsproxywidget.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "frame/cundoredocommand.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// Helper: create a CGraphicsProxyWidget with a parent CGraphicsItem

static PageScene *getTestScene()
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    auto *page = board->currentPage();
    return page ? page->scene() : nullptr;
}
// =========================================================================
static CGraphicsProxyWidget *createProxyWithParent(CGraphicsItem *parent)
{
    return new CGraphicsProxyWidget(parent);
}

// =========================================================================
// Constructor — sets IBeam cursor
// =========================================================================

TEST(CGraphicsProxyWidgetConstructor, SetsIBeamCursor)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    EXPECT_EQ(proxy->cursor().shape(), Qt::IBeamCursor);

    delete proxy;
}

TEST(CGraphicsProxyWidgetConstructor, AcceptsNullParent)
{
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget(nullptr);
    ASSERT_NE(proxy, nullptr);
    EXPECT_EQ(proxy->cursor().shape(), Qt::IBeamCursor);
    EXPECT_EQ(proxy->parentItem(), nullptr);

    delete proxy;
}

// =========================================================================
// parentDrawItem — returns parent CGraphicsItem or nullptr
// =========================================================================

TEST(CGraphicsProxyWidgetParentDrawItem, ReturnsNullWhenNoParent)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    // No parent → should return nullptr
    EXPECT_EQ(proxy->parentDrawItem(), nullptr);

    delete proxy;
}

TEST(CGraphicsProxyWidgetParentDrawItem, ReturnsParentWhenParentIsCGraphicsItem)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Create a CGraphicsTextItem as parent
    CGraphicsTextItem *textItem = new CGraphicsTextItem();
    scene->addCItem(textItem);

    // Create proxy with textItem as parent
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget(textItem);

    CGraphicsItem *parent = proxy->parentDrawItem();
    EXPECT_NE(parent, nullptr);
    EXPECT_EQ(parent, textItem);

    delete proxy;
    delete textItem;
}

// =========================================================================
// paint — does not crash with offscreen rendering
// =========================================================================

TEST(CGraphicsProxyWidgetPaint, DoesNotCrashOffscreen)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    QStyleOptionGraphicsItem option;
    option.rect = QRect(0, 0, 200, 200);

    // Should not crash
    proxy->paint(&painter, &option, nullptr);

    delete proxy;
}

// =========================================================================
// itemChange — flag change is safe (no crash without parent/scene)
// =========================================================================

TEST(CGraphicsProxyWidgetItemChange, FlagChangeSafeWithoutParent)
{
    QGraphicsScene scene;
    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget();
    scene.addItem(proxy);

    // ItemFlagsHaveChanged with no parentDrawItem → should not crash
    // (parentDrawItem returns nullptr, the inner if-guard skips)
    proxy->setFlag(QGraphicsItem::ItemIsMovable, true);

    // If we reach here without crash, the test passes
    SUCCEED();

    delete proxy;
}

// =========================================================================
// itemChange — flag change on a parented proxy records undo/redo info
//
// With a CGraphicsItem parent living in a scene, parentDrawItem() and its
// drawScene() both resolve, so the ItemFlagsHaveChanged handling executes:
//   - clearing ItemHasNoContents ("show", i.e. entering text editing)
//     records the parent item's state as the undo half of a command
//     (recordItemsInfoToCmd with UndoVar, EItemChangedCmd/EAllChanged);
//   - setting ItemHasNoContents again ("hide", i.e. leaving editing)
//     records the redo half when the parent changed meanwhile and
//     finishRecord() pushes the completed couple onto the undo stack.
// =========================================================================

TEST(CGraphicsProxyWidgetItemChange, FlagChangeWithParentRecordsUndoRedoInfo)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Parent must be a CGraphicsItem inside the scene so parentDrawItem()
    // resolves and parentDrawItem->drawScene() is valid.
    CGraphicsTextItem *textItem = new CGraphicsTextItem();
    scene->addCItem(textItem);

    CGraphicsProxyWidget *proxy = new CGraphicsProxyWidget(textItem);
    ASSERT_EQ(proxy->parentDrawItem(), textItem);

    // Wipe records produced by the item's own construction so the
    // assertions below observe only this test's flag change.
    CUndoRedoCommand::clearCommand();
    ASSERT_TRUE(CUndoRedoCommand::isRecordEmpty());

    // Enter editing: any flag change while ItemHasNoContents is unset must
    // run the "show" branch and record the parent's state for undo.
    proxy->setFlag(QGraphicsItem::ItemIsMovable, true);

    EXPECT_FALSE(CUndoRedoCommand::isRecordEmpty());
    ASSERT_EQ(CUndoRedoCommand::s_recordedCmdInfoList.size(), 1);
    const CUndoRedoCommand::SCommandInfoCouple &couple =
        CUndoRedoCommand::s_recordedCmdInfoList.first();
    EXPECT_EQ(couple.undoInfo.tp, CUndoRedoCommand::EItemChangedCmd);
    EXPECT_EQ(couple.undoInfo.expTp, CItemUndoRedoCommand::EAllChanged);
    EXPECT_EQ(couple.undoInfo.urFlag, UndoVar);
    ASSERT_EQ(couple.undoInfo.vars.size(), 2);
    EXPECT_EQ(couple.undoInfo.vars.first().toLongLong(),
              reinterpret_cast<long long>(textItem));
    EXPECT_FALSE(couple.noneedRedoVars);

    // Change the parent while "editing" so the "hide" branch below sees a
    // size difference and completes the redo half of the command.
    textItem->setRect(QRectF(0, 0, 456, 123));

    QUndoStack *stack = CUndoRedoCommand::getUndoRedoStack();
    ASSERT_NE(stack, nullptr);
    const int stackCountBefore = stack->count();

    // Leave editing: setting ItemHasNoContents runs the "hide" branch —
    // the redo info is recorded and finishRecord() pushes the grouped
    // command onto the view's undo stack.
    proxy->setFlag(QGraphicsItem::ItemHasNoContents, true);

    EXPECT_EQ(stack->count(), stackCountBefore + 1);
    EXPECT_TRUE(stack->canUndo());

    stack->clear();
    delete proxy;
    delete textItem;
}
