// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PageView undo stack & action count
 * (src/frame/cgraphicsview.cpp).
 *
 * Covers (issue V-5981 §三):
 *   - pushUndoStack(cmd) — push a command, verify undo/redo works
 *   - pushActionCount()  — operation counter increments
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QUndoStack>
#include <QUndoCommand>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private

// A minimal QUndoCommand for testing pushUndoStack.
class DummyCmd : public QUndoCommand
{
public:
    explicit DummyCmd(const QString &text = "dummy") { setText(text); }
    void undo() override {}
    void redo() override {}
};

TEST(PageViewPushUndoStackTest, PushCommand_IncreasesStackCount)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_pUndoStack, nullptr);

    int countBefore = view->m_pUndoStack->count();
    view->pushUndoStack(new DummyCmd("test1"));
    EXPECT_EQ(view->m_pUndoStack->count(), countBefore + 1);
}

TEST(PageViewPushUndoStackTest, PushNullCommand_NoCrash)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // Pushing nullptr to QUndoStack is undefined behavior; skip to
    // avoid crash. Document as a potential robustness improvement.
    GTEST_SKIP() << "pushUndoStack(nullptr) is UB — skipped";
}

TEST(PageViewPushUndoStackTest, Undo_AfterPushReversesCommand)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_pUndoStack, nullptr);

    view->pushUndoStack(new DummyCmd("undo-test"));
    EXPECT_TRUE(view->m_pUndoStack->canUndo());

    view->m_pUndoStack->undo();
    EXPECT_FALSE(view->m_pUndoStack->canUndo());
}

TEST(PageViewPushUndoStackTest, Redo_AfterUndoReappliesCommand)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    ASSERT_NE(view->m_pUndoStack, nullptr);

    view->pushUndoStack(new DummyCmd("redo-test"));
    view->m_pUndoStack->undo();
    EXPECT_TRUE(view->m_pUndoStack->canRedo());

    view->m_pUndoStack->redo();
    EXPECT_FALSE(view->m_pUndoStack->canRedo());
}

TEST(PageViewPushActionCountTest, Increment_IncreasesCount)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    int before = view->userActionCount;
    view->pushActionCount();
    EXPECT_EQ(view->userActionCount, before + 1);
}

TEST(PageViewPushActionCountTest, MultipleIncrements_Accumulate)
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    int before = view->userActionCount;
    for (int i = 0; i < 5; ++i)
        view->pushActionCount();
    EXPECT_EQ(view->userActionCount, before + 5);
}
