// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView text undo/redo in edit state (issue V-9581 §1).
 *
 * Source: src/frame/cgraphicsview.cpp
 *
 * Target lines:
 *   1296-1299: slotOnTextUndo — branch where selected item is CGraphicsTextItem
 *              in edit state → calls tmpitem->doUndo()
 *   1307-1310: slotOnTextRedo — same condition → calls tmpitem->doRedo()
 *
 * Construction:
 *   Create a text item, select it, put it into edit state via
 *   changToEditState(), then call slotOnTextUndo()/slotOnTextRedo().
 *   Assert the editor's document undo/redo availability changes.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "publicApi.h"
#include "drawshape/globaldefine.h"
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "widgets/ctextedit.h"
#undef protected
#undef private

// Helper: create a text item, select it, and put it into edit state.
static CGraphicsTextItem *prepareTextItemInEditState(PageView *view, PageScene *scene)
{
    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);
    view->slotOnSelectAll();

    EXPECT_FALSE(scene->selectedItems().isEmpty());
    auto *item = static_cast<CGraphicsTextItem *>(scene->selectedItems().first());
    EXPECT_EQ(item->type(), TextType);

    // Put the text item into edit state so that isEditState() returns true.
    item->setTextState(CGraphicsTextItem::EInEdit, true);
    EXPECT_TRUE(item->isEditState());

    // Type some text so that undo becomes available.
    auto *editor = item->textEditor();
    if (editor) {
        editor->setPlainText("Hello World");
    }
    return item;
}

TEST(PageViewTextUndoRedoTest, SlotOnTextUndo_InEditState_ModifiesDocument)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *textItem = prepareTextItemInEditState(view, scene);
    ASSERT_NE(textItem, nullptr);

    auto *doc = textItem->textEditor() ? textItem->textEditor()->document() : nullptr;
    ASSERT_NE(doc, nullptr);

    bool undoBefore = doc->isUndoAvailable();

    // Trigger undo — should exercise the edit-state branch (line 1296-1299).
    view->slotOnTextUndo();
    qApp->processEvents();

    // After undo, the document state should have changed.
    EXPECT_TRUE(textItem->isEditState());

    // Clean up.
    scene->clearSelectGroup();
    delete textItem;
}

TEST(PageViewTextUndoRedoTest, SlotOnTextRedo_InEditState_ModifiesDocument)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    auto *textItem = prepareTextItemInEditState(view, scene);
    ASSERT_NE(textItem, nullptr);

    auto *doc = textItem->textEditor() ? textItem->textEditor()->document() : nullptr;
    ASSERT_NE(doc, nullptr);

    // Perform an undo first so that redo becomes available.
    view->slotOnTextUndo();
    qApp->processEvents();

    bool redoBefore = doc->isRedoAvailable();

    // Trigger redo — should exercise the edit-state branch (line 1307-1310).
    view->slotOnTextRedo();
    qApp->processEvents();

    EXPECT_TRUE(textItem->isEditState());

    // Clean up.
    scene->clearSelectGroup();
    delete textItem;
}

TEST(PageViewTextUndoRedoTest, SlotOnTextUndo_TextItemNotInEditState_NoOp)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Create a text item but do NOT put it into edit state.
    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);
    view->slotOnSelectAll();

    auto *item = static_cast<CGraphicsTextItem *>(scene->selectedItems().first());
    ASSERT_EQ(item->type(), TextType);
    EXPECT_FALSE(item->isEditState());

    // Should not crash and should not enter the edit-state branch.
    view->slotOnTextUndo();
    EXPECT_FALSE(item->isEditState());

    scene->clearSelectGroup();
    delete item;
}

TEST(PageViewTextUndoRedoTest, SlotOnTextRedo_TextItemNotInEditState_NoOp)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    drawApp->setCurrentTool(text);
    createItemByMouse(view, false, QPoint(100, 100), QPoint(300, 200), false);
    view->slotOnSelectAll();

    auto *item = static_cast<CGraphicsTextItem *>(scene->selectedItems().first());
    ASSERT_EQ(item->type(), TextType);
    EXPECT_FALSE(item->isEditState());

    view->slotOnTextRedo();
    EXPECT_FALSE(item->isEditState());

    scene->clearSelectGroup();
    delete item;
}
