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
#include "cellipsetool.h"
#include "globaldefine.h"
#include "cgraphicsview.h"
#include "ccentralwidget.h"
#include "application.h"
#include "drawshape/cdrawscene.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicsrectitem.h"
#undef protected
#undef private

#include "publicApi.h"

#include <QTimer>
#include <QEvent>

#if TEST_CELLIPSETOOL_ITEM

static PageScene *getTestScene()
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    if (!view)
        return nullptr;
    return view->drawScene();
}

static CEllipseTool *createEllipseTool()
{
    CEllipseTool *tool = new CEllipseTool();
    tool->setDrawBoard(getMainWindow()->drawBoard());
    return tool;
}

static IDrawTool::ITERecordInfo makeRecordInfo(PageScene *scene,
                                                CGraphicsItem *item,
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

TEST(CEllipseToolToolCreatItemUpdate, NullPInfo_NoCrash)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CDrawToolEvent event(QPointF(200, 200), QPointF(200, 200), QPointF(200, 200), scene);

    // Act
    tool->toolCreatItemUpdate(&event, nullptr);

    // Assert
    SUCCEED();

    delete tool;
}

TEST(CEllipseToolToolCreatItemUpdate, WrongBusinessItemType_NoCrash)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsRectItem *wrongItem = new CGraphicsRectItem(QRectF(100, 100, 50, 50));
    scene->addItem(wrongItem);

    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, wrongItem);

    CDrawToolEvent event(QPointF(200, 200), QPointF(200, 200), QPointF(200, 200), scene);

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    SUCCEED();

    delete wrongItem;
    delete tool;
}

// =========================================================================
// No modifiers — free-form
// =========================================================================

TEST(CEllipseToolToolCreatItemUpdate, NoModifiers_FreeFormRect)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(300, 250), QPointF(300, 250), QPointF(300, 250), scene);

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(100, 100), QPointF(300, 250));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().x(), expected.x(), 0.5);
    EXPECT_NEAR(item->rect().y(), expected.y(), 0.5);
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

// =========================================================================
// Shift only — square mode
// =========================================================================

TEST(CEllipseToolToolCreatItemUpdate, ShiftOnly_AbsLengthGeHPositive)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(300, 150), QPointF(300, 150), QPointF(300, 150), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(100, 100), QPointF(300, 300));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

TEST(CEllipseToolToolCreatItemUpdate, ShiftOnly_AbsLengthGeHNegative)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(300, 50), QPointF(300, 50), QPointF(300, 50), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(100, 100), QPointF(300, -100));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

TEST(CEllipseToolToolCreatItemUpdate, ShiftOnly_AbsLengthLtWPositive)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(150, 300), QPointF(150, 300), QPointF(150, 300), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(100, 100), QPointF(300, 300));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

TEST(CEllipseToolToolCreatItemUpdate, ShiftOnly_AbsLengthLtWNegative)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(50, 300), QPointF(50, 300), QPointF(50, 300), scene);
    event._kbMods = Qt::ShiftModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(100, 100), QPointF(-100, 300));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

// =========================================================================
// Alt only — centered
// =========================================================================

TEST(CEllipseToolToolCreatItemUpdate, AltOnly_CenteredRect)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(300, 250), QPointF(300, 250), QPointF(300, 250), scene);
    event._kbMods = Qt::AltModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(300, 250), QPointF(-100, -50));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().x(), expected.x(), 0.5);
    EXPECT_NEAR(item->rect().y(), expected.y(), 0.5);
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

// =========================================================================
// Shift+Alt — centered square
// =========================================================================

TEST(CEllipseToolToolCreatItemUpdate, ShiftAlt_CenteredSquare)
{
    // Arrange
    PageScene *scene = getTestScene();
    ASSERT_NE(scene, nullptr);
    CEllipseTool *tool = createEllipseTool();

    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(0, 0, 1, 1);
    scene->addItem(item);

    QPointF startPos(100, 100);
    IDrawTool::ITERecordInfo info = makeRecordInfo(scene, item, startPos);

    CDrawToolEvent event(QPointF(300, 150), QPointF(300, 150), QPointF(300, 150), scene);
    event._kbMods = Qt::ShiftModifier | Qt::AltModifier;

    // Act
    tool->toolCreatItemUpdate(&event, &info);

    // Assert
    QRectF expected(QPointF(300, 300), QPointF(-100, -100));
    expected = expected.normalized();
    EXPECT_NEAR(item->rect().width(), expected.width(), 0.5);
    EXPECT_NEAR(item->rect().height(), expected.height(), 0.5);
    EXPECT_NEAR(item->rect().width(), item->rect().height(), 0.5);

    scene->removeItem(item);
    delete item;
    delete tool;
}

#endif // TEST_CELLIPSETOOL_ITEM
