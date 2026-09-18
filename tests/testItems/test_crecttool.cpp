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
#include "crecttool.h"
#include "globaldefine.h"
#include "cgraphicsview.h"
#include "ccentralwidget.h"
#include "application.h"
#include "drawshape/cdrawscene.h"
#include "cgraphicsrectitem.h"
#undef protected
#undef private

#include "publicApi.h"

#include <QTimer>
#include <QEvent>

#if TEST_CRECTTOOL_ITEM

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
// Helper: create a CRectTool with drawBoard set
// =========================================================================
static CRectTool *createRectTool()
{
    CRectTool *tool = new CRectTool();
    tool->setDrawBoard(getMainWindow()->drawBoard());
    return tool;
}

// =========================================================================
// Helper: create a basic ITERecordInfo for testing
// =========================================================================
static IDrawTool::ITERecordInfo makeRecordInfo(PageScene *scene,
                                                CGraphicsRectItem *item,
                                                const QPointF &startPos = QPointF(100, 100))
{
    IDrawTool::ITERecordInfo info;
    info.eventLife = IDrawTool::ENormal;
    info._startPos = startPos;
    info._prePos = startPos;
    info._scene = scene;
    info._isvaild = true;
    info.businessItem = item;
    return info;
}

// =========================================================================
// Null / wrong-type safety tests
// =========================================================================

TEST(CRectToolToolCreatItemUpdate, NullPInfo_NoCrash)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CDrawToolEvent event(QPointF(200, 200), QPointF(200, 200), QPointF(200, 200), scene);

    // Act — should early-return without crash
    tool->toolCreatItemUpdate(&event, nullptr);

    // Assert
    SUCCEED();

    delete tool;
}

TEST(CRectToolToolCreatItemUpdate, WrongBusinessItemType_NoCrash)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    // Use an ellipse item where a rect item is expected → dynamic_cast fails
    CGraphicsEllipseItem *wrongItem = new CGraphicsEllipseItem(100, 100, 50, 50);
    scene->addItem(wrongItem);

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, nullptr);
    info.businessItem = wrongItem;

    CDrawToolEvent event(QPointF(200, 200), QPointF(200, 200), QPointF(200, 200), scene);

    // Act — dynamic_cast returns nullptr, should early-return
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    SUCCEED();

    delete wrongItem;
    delete tool;
}

// =========================================================================
// No modifiers — free-form rectangle
// =========================================================================

TEST(CRectToolToolCreatItemUpdate, NoModifiers_FreeFormRect)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    CDrawToolEvent event(QPointF(300, 250), QPointF(300, 250), QPointF(300, 250), scene);

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — QRectF(100,100, 300,250).normalized() = QRectF(100,100, 200,150)
    QRectF expected(QPointF(100, 100), QPointF(300, 250));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().x(), expected.x(), 0.5);
    EXPECT_NEAR(rectItem->rect().y(), expected.y(), 0.5);
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);
    EXPECT_TRUE(event.isAccepted());

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

// =========================================================================
// Shift only — square mode (4 sub-branches)
// =========================================================================

TEST(CRectToolToolCreatItemUpdate, ShiftOnly_AbsLengthGeHPositive)
{
    // Arrange — |w| > |h|, h >= 0 → Y adjusted to |w|
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    // w = 300 - 100 = 200, h = 150 - 100 = 50 → abslength = 200 - 50 = 150 >= 0.1, h >= 0
    CDrawToolEvent event(QPointF(300, 150), QPointF(300, 150), QPointF(300, 150), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — resultPoint.setY(100 + 200) = 300 → QRectF(100,100, 300,300).normalized()
    QRectF expected(QPointF(100, 100), QPointF(300, 300));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

TEST(CRectToolToolCreatItemUpdate, ShiftOnly_AbsLengthGeHNegative)
{
    // Arrange — |w| > |h|, h < 0 → Y adjusted to -|w|
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    // w = 300 - 100 = 200, h = 50 - 100 = -50 → abslength = 150 >= 0.1, h < 0
    CDrawToolEvent event(QPointF(300, 50), QPointF(300, 50), QPointF(300, 50), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — resultPoint.setY(100 - 200) = -100 → QRectF(100,100, 300,-100).normalized()
    QRectF expected(QPointF(100, 100), QPointF(300, -100));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

TEST(CRectToolToolCreatItemUpdate, ShiftOnly_AbsLengthLtWPositive)
{
    // Arrange — |w| < |h|, w >= 0 → X adjusted to |h|
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    // w = 150 - 100 = 50, h = 300 - 100 = 200 → abslength = 50 - 200 = -150 < 0.1, w >= 0
    CDrawToolEvent event(QPointF(150, 300), QPointF(150, 300), QPointF(150, 300), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — resultPoint.setX(100 + 200) = 300 → QRectF(100,100, 300,300).normalized()
    QRectF expected(QPointF(100, 100), QPointF(300, 300));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

TEST(CRectToolToolCreatItemUpdate, ShiftOnly_AbsLengthLtWNegative)
{
    // Arrange — |w| < |h|, w < 0 → X adjusted to -|h|
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    // w = 50 - 100 = -50, h = 300 - 100 = 200 → abslength = -150 < 0.1, w < 0
    CDrawToolEvent event(QPointF(50, 300), QPointF(50, 300), QPointF(50, 300), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — resultPoint.setX(100 - 200) = -100 → QRectF(100,100, -100,300).normalized()
    QRectF expected(QPointF(100, 100), QPointF(-100, 300));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

// =========================================================================
// Alt only — centered rectangle
// =========================================================================

TEST(CRectToolToolCreatItemUpdate, AltOnly_CenteredRect)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100); // center
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    // point1 = (300, 250), center = (100, 100), point2 = 2*(100,100) - (300,250) = (-100, -50)
    CDrawToolEvent event(QPointF(300, 250), QPointF(300, 250), QPointF(300, 250), scene);
    event._kbMods = Qt::AltModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — QRectF(300,250, -100,-50).normalized() = QRectF(-100,-50, 400,300)
    QRectF expected(QPointF(300, 250), QPointF(-100, -50));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().x(), expected.x(), 0.5);
    EXPECT_NEAR(rectItem->rect().y(), expected.y(), 0.5);
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

// =========================================================================
// Shift+Alt — centered square
// =========================================================================

TEST(CRectToolToolCreatItemUpdate, ShiftAlt_CenteredSquare)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CRectTool *tool = createRectTool();

    CGraphicsRectItem *rectItem = new CGraphicsRectItem(QRectF(0, 0, 1, 1));
    scene->addItem(rectItem);

    QPointF startPos(100, 100); // center
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, rectItem, startPos);

    // w = 200, h = 50 → abslength = 150 >= 0.1, h >= 0 → resultPoint.setY(100+200)=300
    // point1 = (300, 300), center = (100, 100), point2 = (200-300, 200-300) = (-100, -100)
    CDrawToolEvent event(QPointF(300, 150), QPointF(300, 150), QPointF(300, 150), scene);
    event._kbMods = Qt::ShiftModifier | Qt::AltModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert — QRectF(300,300, -100,-100).normalized() = QRectF(-100,-100, 400,400)
    QRectF expected(QPointF(300, 300), QPointF(-100, -100));
    expected = expected.normalized();
    EXPECT_NEAR(rectItem->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(rectItem->rect().height(), expected.height(), 0.5);
    // Should be square
    EXPECT_NEAR(rectItem->rect().width(), rectItem->rect().height(), 0.5);

    scene->removeItem(rectItem);
    delete rectItem;
    delete tool;
}

#endif // TEST_CRECTTOOL_ITEM
