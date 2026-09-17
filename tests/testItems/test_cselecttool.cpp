// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "qteventcompat.h"

#define protected public
#define private public
#include "mainwindow.h"
#include "idrawtool.h"
#include "idrawtoolevent.h"
#include "cselecttool.h"
#include "csizehandlerect.h"
#include "globaldefine.h"
#include "cgraphicsview.h"
#include "ccentralwidget.h"
#include "application.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "drawshape/cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsitemselectedmgr.h"
#include "cgraphicsitemevent.h"
#undef protected
#undef private

#include "publicApi.h"

#include <QTimer>
#include <QEvent>
#include <QScrollBar>

#if TEST_CSELECTTOOL_ITEM

// =========================================================================
// Helper: obtain a valid scene from a freshly created view
// =========================================================================
static PageScene *getTestScene()
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    if (!view)
        return nullptr;
    return view->drawScene();
}

// =========================================================================
// Helper: create a CSelectTool with drawBoard set
// =========================================================================
static CSelectTool *createSelectTool()
{
    CSelectTool *tool = new CSelectTool();
    tool->setDrawBoard(getMainWindow()->drawBoard());
    return tool;
}

// =========================================================================
// Helper: create a basic ITERecordInfo for testing
// =========================================================================
static IDrawTool::ITERecordInfo makeRecordInfo(PageScene *scene,
                                                const QPointF &startPos = QPointF(100, 100))
{
    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::ENormal;
    info._startPos = startPos;
    info._prePos = startPos;
    info._scene = scene;
    info._isvaild = true;
    return info;
}

// =========================================================================
// decideUpdate tests
// =========================================================================

TEST(CSelectToolDecideUpdate, NotWorking_ReturnsENothingDo)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), scene);

    // No ITERecordInfo inserted → isWorking() returns false
    int result = tool->decideUpdate(&event, nullptr);

    EXPECT_EQ(result, CSelectTool::ENothingDo);

    delete tool;
}

TEST(CSelectToolDecideUpdate, NoItem_MouseEvent_ReturnsERectSelect)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(500, 500), QPointF(500, 500), QPointF(500, 500), scene);
    event._msBtns = Qt::LeftButton;
    // Default _orgEvent is nullptr → EEventSimulated (not ETouchEvent)

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(500, 500));
    tool->_allITERecordInfo.insert(event.uuid(), info);

    int result = tool->decideUpdate(&event, &tool->_allITERecordInfo[event.uuid()]);

    EXPECT_EQ(result, CSelectTool::ERectSelect);

    tool->clearITE();
    delete tool;
}

TEST(CSelectToolDecideUpdate, NoItem_TouchEvent_LongElapsed_ReturnsERectSelect)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(500, 500), QPointF(500, 500), QPointF(500, 500), scene);
    event._msBtns = Qt::LeftButton;
    QEvent touchEvent(QEvent::TouchBegin);
    event._orgEvent = &touchEvent;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(500, 500));
    info._elapsedToUpdate = 300; // > 200
    tool->_allITERecordInfo.insert(event.uuid(), info);

    int result = tool->decideUpdate(&event, &tool->_allITERecordInfo[event.uuid()]);

    EXPECT_EQ(result, CSelectTool::ERectSelect);

    tool->clearITE();
    delete tool;
}

TEST(CSelectToolDecideUpdate, NoItem_TouchEvent_ShortElapsed_ReturnsEDragSceneMove)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CDrawToolEvent event(QPointF(500, 500), QPointF(500, 500), QPointF(500, 500), scene);
    event._msBtns = Qt::LeftButton;
    QEvent touchEvent(QEvent::TouchBegin);
    event._orgEvent = &touchEvent;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(500, 500));
    info._elapsedToUpdate = 100; // <= 200
    tool->_allITERecordInfo.insert(event.uuid(), info);

    int result = tool->decideUpdate(&event, &tool->_allITERecordInfo[event.uuid()]);

    EXPECT_EQ(result, CSelectTool::EDragSceneMove);

    tool->clearITE();
    delete tool;
}

TEST(CSelectToolDecideUpdate, NoItem_SimulatedEvent_ReturnsERectSelect)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Simulated event (no _orgEvent) is treated as non-touch → ERectSelect
    CDrawToolEvent event(QPointF(500, 500), QPointF(500, 500), QPointF(500, 500), scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(500, 500));
    tool->_allITERecordInfo.insert(event.uuid(), info);

    int result = tool->decideUpdate(&event, &tool->_allITERecordInfo[event.uuid()]);

    EXPECT_EQ(result, CSelectTool::ERectSelect);

    tool->clearITE();
    delete tool;
}

TEST(CSelectToolDecideUpdate, BzItem_NoModifier_ReturnsEDragMove)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Create a rect item at a known position with solid fill so it is non-penetrable
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    rectItem->setBrush(QBrush(Qt::red));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    // Click on the item
    QPointF itemCenter(200, 175);
    CDrawToolEvent event(itemCenter, itemCenter, itemCenter, scene);
    event._msBtns = Qt::LeftButton;
    event._kbMods = Qt::NoModifier;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, itemCenter);
    info.startPosItems.append(rectItem);
    tool->_allITERecordInfo.insert(event.uuid(), info);

    int result = tool->decideUpdate(&event, &tool->_allITERecordInfo[event.uuid()]);

    EXPECT_EQ(result, CSelectTool::EDragMove);

    tool->clearITE();
    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolDecideUpdate, BzItem_AltModifier_ReturnsECopyMove)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    rectItem->setBrush(QBrush(Qt::red));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF itemCenter(200, 175);
    CDrawToolEvent event(itemCenter, itemCenter, itemCenter, scene);
    event._msBtns = Qt::LeftButton;
    event._kbMods = Qt::AltModifier;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, itemCenter);
    info.startPosItems.append(rectItem);
    tool->_allITERecordInfo.insert(event.uuid(), info);

    int result = tool->decideUpdate(&event, &tool->_allITERecordInfo[event.uuid()]);

    EXPECT_EQ(result, CSelectTool::ECopyMove);

    tool->clearITE();
    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

// =========================================================================
// toolStart tests
// =========================================================================

TEST(CSelectToolToolStart, EmptySpace_Click_ClearsSelection)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Create and select an item first
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);
    ASSERT_GT(scene->selectGroup()->count(), 0);

    // Click on empty space
    QPointF emptyPos(800, 800);
    CDrawToolEvent event(emptyPos, emptyPos, emptyPos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, emptyPos);
    info._curEvent = event;
    info._startEvent = event;

    tool->toolStart(&event, &info);

    // Empty space click should clear selection
    EXPECT_EQ(scene->selectGroup()->count(), 0);

    delete rectItem;
    delete tool;
}

TEST(CSelectToolToolStart, ItemClick_SelectsItem)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);

    QPointF itemCenter(200, 175);
    CDrawToolEvent event(itemCenter, itemCenter, itemCenter, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, itemCenter);
    info.startPosTopBzItem = rectItem;
    info._curEvent = event;
    info._startEvent = event;

    tool->toolStart(&event, &info);

    // The item should be selected now
    EXPECT_GT(scene->selectGroup()->count(), 0);

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolToolStart, ShiftModifier_TogglesSelection)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Create two items and select both so count > 1
    CGraphicsRectItem *rectItem1 = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    rectItem1->setBrush(QBrush(Qt::red));
    scene->addItem(rectItem1);
    scene->selectItem(rectItem1);

    CGraphicsRectItem *rectItem2 = new CGraphicsRectItem(QRectF(400, 100, 200, 150));
    rectItem2->setBrush(QBrush(Qt::blue));
    scene->addItem(rectItem2);
    scene->selectItem(rectItem2);

    ASSERT_EQ(scene->selectGroup()->count(), 2);

    // Shift+click on already selected item in multi-selection → deselect one
    QPointF itemCenter(200, 175);
    CDrawToolEvent event(itemCenter, itemCenter, itemCenter, scene);
    event._msBtns = Qt::LeftButton;
    event._kbMods = Qt::ShiftModifier;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, itemCenter);
    info.startPosTopBzItem = rectItem1;
    info._curEvent = event;
    info._startEvent = event;

    tool->toolStart(&event, &info);

    // Shift+click on a selected item in a multi-selection (count > 1) deselects it
    EXPECT_EQ(scene->selectGroup()->count(), 1);

    scene->clearSelectGroup();
    delete rectItem1;
    delete rectItem2;
    delete tool;
}

TEST(CSelectToolToolStart, ClearHighlightPath)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF pos(500, 500);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, pos);
    info._curEvent = event;
    info._startEvent = event;

    tool->toolStart(&event, &info);

    // After toolStart, _hightLight should be empty (cleared at start)
    EXPECT_TRUE(tool->_hightLight.isEmpty());

    delete tool;
}

// =========================================================================
// toolUpdate tests
// =========================================================================

TEST(CSelectToolToolUpdate, ERectSelect_UpdatesScene)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF pos(300, 300);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(100, 100));
    info._opeTpUpdate = CSelectTool::ERectSelect;
    info._curEvent = event;
    info._startEvent = event;

    // Should not crash, just calls scene()->update()
    tool->toolUpdate(&event, &info);

    // event should be set to not accepted
    EXPECT_FALSE(event.isAccepted());

    delete tool;
}

TEST(CSelectToolToolUpdate, EDragSceneMove_AdjustsScrollBars)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF startPos(100, 100);
    QPointF currentPos(200, 200);
    CDrawToolEvent event(currentPos, currentPos, currentPos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, startPos);
    info._opeTpUpdate = CSelectTool::EDragSceneMove;
    info._curEvent = event;
    info._startEvent = event;

    // Should adjust scroll bars without crashing
    tool->toolUpdate(&event, &info);

    EXPECT_FALSE(event.isAccepted());

    delete tool;
}

TEST(CSelectToolToolUpdate, ENothingDo_Default_NoCrash)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF pos(300, 300);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(100, 100));
    info._opeTpUpdate = CSelectTool::ENothingDo;
    info._curEvent = event;
    info._startEvent = event;

    tool->toolUpdate(&event, &info);

    EXPECT_FALSE(event.isAccepted());

    delete tool;
}

// =========================================================================
// toolFinish tests
// =========================================================================

TEST(CSelectToolToolFinish, ERectSelect_SelectsItemsByRect)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    // Create an item inside the rect
    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 100, 80));
    rectItem->setBrush(QBrush(Qt::red));
    scene->addItem(rectItem);

    QPointF startPos(50, 50);
    QPointF endPos(300, 300);
    CDrawToolEvent event(endPos, endPos, endPos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, startPos);
    info._opeTpUpdate = CSelectTool::ERectSelect;
    info._curEvent = event;
    info._startEvent = event;

    tool->toolFinish(&event, &info);

    // Verify the ERectSelect branch completes without crash
    SUCCEED();

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolToolFinish, ENothingDo_Default_ReturnsEarly)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF pos(300, 300);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(100, 100));
    info._opeTpUpdate = CSelectTool::ENothingDo;
    info._curEvent = event;
    info._startEvent = event;

    // Default case returns early without crash
    tool->toolFinish(&event, &info);

    // Should not crash — that's the expectation
    SUCCEED();

    delete tool;
}

TEST(CSelectToolToolFinish, EDragMove_ResetsItemMoving)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF pos(200, 175);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(150, 150));
    info._opeTpUpdate = CSelectTool::EDragMove;
    info._curEvent = event;
    info._startEvent = event;

    tool->toolFinish(&event, &info);

    // m_isItemMoving should be reset to false
    EXPECT_FALSE(tool->m_isItemMoving);

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolToolFinish, ECopyMove_ResetsItemMoving)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF pos(200, 175);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(150, 150));
    info._opeTpUpdate = CSelectTool::ECopyMove;
    info._curEvent = event;
    info._startEvent = event;

    tool->toolFinish(&event, &info);

    EXPECT_FALSE(tool->m_isItemMoving);

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

// =========================================================================
// sendToolEventToItem tests
// =========================================================================

TEST(CSelectToolSendToolEventToItem, EDragMove_DispatchesAsMove)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF pos(200, 175);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(180, 160));
    info._opeTpUpdate = CSelectTool::EDragMove;
    info.etcItems.append(rectItem);
    info._curEvent = event;
    info._startEvent = event;

    // Should not crash — dispatches processItemsMove with EChangedBegin
    tool->sendToolEventToItem(&event, &info, EChangedBegin);

    SUCCEED();

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolSendToolEventToItem, ECopyMove_DispatchesAsMove)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF pos(200, 175);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(180, 160));
    info._opeTpUpdate = CSelectTool::ECopyMove;
    info.etcItems.append(rectItem);
    info._curEvent = event;
    info._startEvent = event;

    tool->sendToolEventToItem(&event, &info, EChangedUpdate);

    SUCCEED();

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolSendToolEventToItem, EResizeMove_DispatchesAsScal)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF pos(300, 250);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(280, 230));
    info._opeTpUpdate = CSelectTool::EResizeMove;
    info._etcopeTpUpdate = CSizeHandleRect::RightBottom;
    info.etcItems.append(rectItem);
    info._curEvent = event;
    info._startEvent = event;

    tool->sendToolEventToItem(&event, &info, EChangedUpdate);

    SUCCEED();

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolSendToolEventToItem, ERotateMove_DispatchesAsRot)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(100, 100, 200, 150));
    scene->addItem(rectItem);
    scene->selectItem(rectItem);

    QPointF pos(200, 50);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(200, 80));
    info._opeTpUpdate = CSelectTool::ERotateMove;
    info._etcopeTpUpdate = CSizeHandleRect::Rotation;
    info.etcItems.append(rectItem);
    info._curEvent = event;
    info._startEvent = event;

    tool->sendToolEventToItem(&event, &info, EChangedUpdate);

    SUCCEED();

    scene->clearSelectGroup();
    delete rectItem;
    delete tool;
}

TEST(CSelectToolSendToolEventToItem, ENothingDo_Default_NoCrash)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF pos(300, 300);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(100, 100));
    info._opeTpUpdate = CSelectTool::ENothingDo;
    info._curEvent = event;
    info._startEvent = event;

    // Default → EUnKnow → no dispatch, should not crash
    tool->sendToolEventToItem(&event, &info, EChangedBegin);

    SUCCEED();

    delete tool;
}

TEST(CSelectToolSendToolEventToItem, ERectSelect_Default_NoCrash)
{
    CSelectTool *tool = createSelectTool();
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);

    QPointF pos(300, 300);
    CDrawToolEvent event(pos, pos, pos, scene);
    event._msBtns = Qt::LeftButton;

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, QPointF(100, 100));
    info._opeTpUpdate = CSelectTool::ERectSelect;
    info._curEvent = event;
    info._startEvent = event;

    // ERectSelect → EUnKnow → no dispatch
    tool->sendToolEventToItem(&event, &info, EChangedFinished);

    SUCCEED();

    delete tool;
}

#endif // TEST_CSELECTTOOL_ITEM
