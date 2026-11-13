// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for miscellaneous zero-coverage interfaces (V-8216)
 *
 * Target methods:
 *   - DZoomMenuComboBox: ~destructor, setItemICon(QString, QIcon), initUI() lambda
 *   - IDrawTool: ~destructor, mouseDoubleClickEvent, ITERecordInfo::isVaild()
 *   - PageScene: mouseDoubleClickEvent, blockMouseMoveEvent, getGroupTreeInfo
 *   - DrawBoard: closeEvent, loadFiles
 *
 * Source files:
 *   src/widgets/dzoommenucombobox.cpp
 *   src/drawshape/drawTools/idrawtool.cpp
 *   src/drawshape/cdrawscene.cpp
 *   src/frame/ccentralwidget.cpp
 */

#include <gtest/gtest.h>

#include <QIcon>
#include <QString>
#include <QCloseEvent>
#include <QStackedWidget>
#include <QStringList>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QPixmap>
#include <QPointer>

#define protected public
#define private public
#include "dzoommenucombobox.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/cdrawscene.h"
#include "ccentralwidget.h"
#include "publicApi.h"
#undef protected
#undef private

#include "test_helpers.h"

// =========================================================================
// DZoomMenuComboBox
// =========================================================================

TEST(DZoomMenuComboBoxZeroCov, SetItemIConByText_MatchingAction_SetsIcon)
{
    DZoomMenuComboBox combo(nullptr);
    combo.addItem("item1");
    combo.addItem("item2");

    QPixmap pm(8, 8);
    pm.fill(Qt::red);
    combo.setItemICon("item1", QIcon(pm));

    // The icon must land on the action whose text matches.
    ASSERT_FALSE(combo.m_actions.isEmpty());
    EXPECT_EQ(combo.m_actions.at(0)->text(), QString("item1"));
    EXPECT_FALSE(combo.m_actions.at(0)->icon().isNull());
    EXPECT_EQ(combo.m_actions.at(0)->icon().pixmap(8, 8).toImage(), pm.toImage());
    // The non-matching item keeps its null icon.
    EXPECT_TRUE(combo.m_actions.at(1)->icon().isNull());
}

TEST(DZoomMenuComboBoxZeroCov, SetItemIConByText_NoMatch_NoCrash)
{
    DZoomMenuComboBox combo(nullptr);
    combo.addItem("item1");

    QPixmap pm(8, 8);
    pm.fill(Qt::red);
    combo.setItemICon("nonexistent", QIcon(pm));

    // No action may be touched when the text does not match.
    ASSERT_FALSE(combo.m_actions.isEmpty());
    EXPECT_TRUE(combo.m_actions.at(0)->icon().isNull());
}

TEST(DZoomMenuComboBoxZeroCov, SetItemIConByIndex_Valid_SetsIcon)
{
    DZoomMenuComboBox combo(nullptr);
    combo.addItem("item1");

    QPixmap pm(8, 8);
    pm.fill(Qt::blue);
    combo.setItemICon(0, QIcon(pm));
    ASSERT_FALSE(combo.m_actions.isEmpty());
    EXPECT_EQ(combo.m_actions.at(0)->icon().pixmap(8, 8).toImage(), pm.toImage());
}

TEST(DZoomMenuComboBoxZeroCov, SetItemIConByIndex_Invalid_NoCrash)
{
    DZoomMenuComboBox combo(nullptr);
    combo.addItem("item1");

    QPixmap pm(8, 8);
    pm.fill(Qt::red);
    combo.setItemICon(99, QIcon(pm)); // out of range

    // Out-of-range index must leave every action untouched.
    ASSERT_FALSE(combo.m_actions.isEmpty());
    EXPECT_TRUE(combo.m_actions.at(0)->icon().isNull());
}

TEST(DZoomMenuComboBoxZeroCov, Destructor_HeapInstance_DestroysWidget)
{
    QPointer<DZoomMenuComboBox> guard;
    {
        auto *combo = new DZoomMenuComboBox(nullptr);
        combo->addItem("item1");
        guard = combo;
        EXPECT_FALSE(guard.isNull());
        delete combo;
    }
    EXPECT_TRUE(guard.isNull());
}

TEST(DZoomMenuComboBoxZeroCov, InitUI_ButtonClickedLambda_RequiresPopup)
{
    // m_btn->click() opens a popup menu which hangs in offscreen mode.
    // The lambda is exercised indirectly via the constructor's initUI().
    GTEST_SKIP() << "Button click opens popup; cannot test in offscreen mode";
}

// =========================================================================
// IDrawTool::ITERecordInfo::isVaild
// =========================================================================

TEST(IDrawToolZeroCov, ITERecordInfo_IsVaild_DefaultFalse)
{
    IDrawTool::ITERecordInfo info;
    EXPECT_FALSE(info.isVaild());
}

TEST(IDrawToolZeroCov, ITERecordInfo_IsVaild_SetTrue_ReturnsTrue)
{
    IDrawTool::ITERecordInfo info;
    info._isvaild = true;
    EXPECT_TRUE(info.isVaild());
}

TEST(IDrawToolZeroCov, ITERecordInfo_IsVaild_ToggleReturnsCorrect)
{
    IDrawTool::ITERecordInfo info;
    info._isvaild = false;
    EXPECT_FALSE(info.isVaild());
    info._isvaild = true;
    EXPECT_TRUE(info.isVaild());
    info._isvaild = false;
    EXPECT_FALSE(info.isVaild());
}

// =========================================================================
// PageScene: blockMouseMoveEvent
// =========================================================================

TEST(PageSceneZeroCov, BlockMouseMoveEvent_SetTrue_FlagIsTrue)
{
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->blockMouseMoveEvent(true);
    EXPECT_TRUE(scene->isBlockMouseMoveEvent());
}

TEST(PageSceneZeroCov, BlockMouseMoveEvent_SetFalse_FlagIsFalse)
{
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    scene->blockMouseMoveEvent(false);
    EXPECT_FALSE(scene->isBlockMouseMoveEvent());
}

// =========================================================================
// PageScene: mouseDoubleClickEvent
// =========================================================================

TEST(PageSceneZeroCov, MouseDoubleClickEvent_NoCrash)
{
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    QGraphicsSceneMouseEvent event;
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);
    event.setPos(QPointF(10, 10));
    event.setScenePos(QPointF(10, 10));

    // Double-clicking an empty canvas must not select or create anything.
    const int itemCountBefore = scene->items().count();
    ASSERT_TRUE(scene->selectedItems().isEmpty());
    scene->mouseDoubleClickEvent(&event);
    EXPECT_TRUE(scene->selectedItems().isEmpty());
    EXPECT_EQ(scene->items().count(), itemCountBefore);
}

// =========================================================================
// PageScene: getGroupTreeInfo
// =========================================================================

TEST(PageSceneZeroCov, GetGroupTreeInfo_NullGroup_ProcessEvent_NoCrash)
{
    ensurePageExists();
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);

    // Single consolidated test: a null group yields the virtual root group;
    // processEvent=true additionally exercises the qApp->processEvents() branches.
    auto info = scene->getGroupTreeInfo(nullptr, ESaveToDDf, true);
    EXPECT_EQ(info.groupType(), CGraphicsItemGroup::EVirRootGroup);
}

// =========================================================================
// DrawBoard: closeEvent
// =========================================================================

TEST(DrawBoardZeroCov, CloseEvent_AcceptsWhenNoPages)
{
    MainWindow *mw = getMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);

    // Make the precondition explicit: no pages on the board. Force-close
    // so a page dirtied by an earlier test cannot open the save prompt
    // and refuse.
    //
    // Walk the board's QStackedWidget directly instead of
    // DrawBoard::page(count - 1): the latter indexes the parallel tab bar
    // (page(i) == page(_topTabs->key(i))), and after a long series of rapid
    // force-closes that lookup can transiently miss a live stack page
    // (out-of-range/empty key -> page("") == nullptr) while count() is
    // still positive. The stack is the authoritative page list.
    while (board->count() > 0) {
        QStackedWidget *stack = nullptr;
        const QList<QStackedWidget *> stacks = board->findChildren<QStackedWidget *>();
        for (QStackedWidget *s : stacks) {
            if (s->parentWidget() == board) {
                stack = s;
                break;
            }
        }
        ASSERT_NE(stack, nullptr);
        Page *pg = qobject_cast<Page *>(stack->widget(stack->count() - 1));
        ASSERT_NE(pg, nullptr);
        ASSERT_TRUE(pg->close(true));
        // Run the board's queued autoClose MetaCall now, under our control,
        // instead of letting it land inside a later iteration's close.
        QCoreApplication::sendPostedEvents(board, QEvent::MetaCall);
    }

    QCloseEvent event;
    board->closeEvent(&event);
    // With no pages left to iterate, the event must be accepted.
    EXPECT_TRUE(event.isAccepted());
    EXPECT_EQ(board->count(), 0);
}

// =========================================================================
// DrawBoard: loadFiles
// =========================================================================

TEST(DrawBoardZeroCov, LoadFiles_EmptyList_NoCrash)
{
    MainWindow *mw = getMainWindow();
    ASSERT_NE(mw, nullptr);
    DrawBoard *board = mw->drawBoard();
    ASSERT_NE(board, nullptr);

    const int pageCountBefore = board->count();
    board->loadFiles(QStringList(), false, 0, false);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    // An empty file list must not add any page.
    EXPECT_EQ(board->count(), pageCountBefore);
}
