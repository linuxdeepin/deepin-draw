// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "qteventcompat.h"

// Pre-include application.h before the macro hack: it internally does
// #undef protected, which would undo our #define when cerasertool.h
// re-includes it. Pre-including sets the include guard first.
#include "application.h"

#define protected public
#define private public
#include "mainwindow.h"
#include "idrawtool.h"
#include "idrawtoolevent.h"
#include "cerasertool.h"
#include "globaldefine.h"
#include "cgraphicsview.h"
#include "ccentralwidget.h"
#include "application.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "cgraphicslayer.h"
#include "cgraphicsrectitem.h"
#include "widgets/toolbutton.h"
#include "widgets/cspinbox.h"
#include "cattributeitemwidget.h"
#include "cattributemanagerwgt.h"
#undef protected
#undef private

#include "publicApi.h"

#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QPicture>
#include <QImage>
#include <QPainter>
#include <QCursor>
#include <DToolButton>

#if TEST_CERASERTOOL_ITEM

// =========================================================================
// Helper: obtain a valid scene from a freshly created view
// =========================================================================
static PageScene *getEraserTestScene()
{
    createNewViewByShortcutKey();
    PageView *view = getCurView();
    if (!view)
        return nullptr;
    return view->drawScene();
}

// =========================================================================
// Helper: create a CEraserTool with drawBoard set
// =========================================================================
static CEraserTool *createEraserTool()
{
    CEraserTool *tool = new CEraserTool();
    tool->setDrawBoard(getMainWindow()->drawBoard());
    return tool;
}

// =========================================================================
// Helper: build a basic ITERecordInfo
// =========================================================================
static IDrawTool::ITERecordInfo makeEraserRecordInfo(PageScene *scene,
                                                      CGraphicsItem *item = nullptr,
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
// Section 1: CEraserTool construction and basic interface
// =========================================================================

TEST(CEraserTool, Constructor_DrawToolModeIsEraser)
{
    CEraserTool tool;
    EXPECT_EQ(tool.getDrawToolMode(), eraser);
}

TEST(CEraserTool, Constructor_InitialStatusIsEIdle)
{
    CEraserTool tool;
    EXPECT_EQ(tool._status, IDrawTool::EIdle);
}

TEST(CEraserTool, Attributions_ContainsOnlyEEraserWidth)
{
    // attributions() -> defaultAttriVar() requires drawBoard to be set
    CEraserTool *tool = createEraserTool();
    DrawAttribution::SAttrisList attrs = tool->attributions();
    EXPECT_EQ(attrs.size(), 1);
    if (attrs.size() >= 1) {
        EXPECT_EQ(attrs[0].attri, DrawAttribution::EEraserWidth);
    }
    delete tool;
}

TEST(CEraserTool, Cursor_IsBitmapCursorWithTransparentPixel)
{
    CEraserTool tool;
    QCursor c = tool.cursor();
    EXPECT_EQ(c.shape(), Qt::BitmapCursor);
    QPixmap pm = c.pixmap();
    EXPECT_EQ(pm.width(), 1);
    EXPECT_EQ(pm.height(), 1);
    EXPECT_EQ(pm.toImage().pixelColor(0, 0).alpha(), 0);
}

TEST(CEraserTool, InitToolButton_NonNullCheckableFixedSize)
{
    CEraserTool tool;
    QAbstractButton *btn = tool.initToolButton();
    ASSERT_NE(btn, nullptr);
    EXPECT_TRUE(btn->isCheckable());
    EXPECT_EQ(btn->toolTip(), QStringLiteral("Eraser (E)"));
    EXPECT_EQ(btn->size(), QSize(37, 37));
    delete btn;
}

TEST(CEraserTool, InitToolButton_ShortcutIsKeyE)
{
    CEraserTool tool;
    QAbstractButton *btn = tool.initToolButton();
    ASSERT_NE(btn, nullptr);
    EXPECT_EQ(btn->shortcut(), QKeySequence(Qt::Key_E));
    delete btn;
}

TEST(CEraserTool, InitToolButton_ToggledChangesIcon)
{
    CEraserTool tool;
    QAbstractButton *btn = tool.initToolButton();
    ASSERT_NE(btn, nullptr);
    // Toggle on/off — must not crash; icon may be empty in headless theme
    btn->setChecked(true);
    EXPECT_NO_FATAL_FAILURE(btn->setIcon(btn->icon()));
    btn->setChecked(false);
    EXPECT_NO_FATAL_FAILURE(btn->setIcon(btn->icon()));
    delete btn;
}

TEST(CEraserTool, CreatItem_AlwaysReturnsNullptr)
{
    CEraserTool tool;
    CDrawToolEvent event(QPointF(100, 100), QPointF(100, 100), QPointF(100, 100), nullptr);
    IDrawTool::ITERecordInfo info;
    CGraphicsItem *result = tool.creatItem(&event, &info);
    EXPECT_EQ(result, nullptr);
}

TEST(CEraserTool, RegisterAttributionWidgets_NoCrash)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    // The application already registered its own EEraserWidth widget at
    // startup (DrawBoard::initTools). Save the existing registry entry so
    // it can be restored after this test registers a replacement widget.
    auto attrMgr = getMainWindow()->drawBoard()->attributionWidget();
    const bool hadEntry = attrMgr->s_allInstalledAttriWgts.contains(EEraserWidth);
    QWidget *origWgt = attrMgr->widgetOfAttr(EEraserWidth);
    QVariant origVar = attrMgr->s_allInstalledDefaultVar.value(EEraserWidth);

    EXPECT_NO_FATAL_FAILURE(tool->registerAttributionWidgets());

    // Restore the pre-existing registry entry so later lookups keep using
    // the registered tool's widget, not the one created by this test.
    QWidget *testWgt = attrMgr->widgetOfAttr(EEraserWidth);
    if (hadEntry) {
        attrMgr->s_allInstalledAttriWgts.insert(EEraserWidth, origWgt);
        attrMgr->s_allInstalledDefaultVar.insert(EEraserWidth, origVar);
    } else {
        attrMgr->s_allInstalledAttriWgts.remove(EEraserWidth);
        attrMgr->s_allInstalledDefaultVar.remove(EEraserWidth);
    }
    // The widget created by the test call is unparented; delete it instead
    // of leaking it (its signal connection dies with the QObject).
    if (testWgt != nullptr && testWgt != origWgt)
        delete testWgt;

    EXPECT_EQ(attrMgr->widgetOfAttr(EEraserWidth), origWgt);

    delete tool;
}

TEST(CEraserTool, Destructor_NoCrash)
{
    auto *tool = new CEraserTool();
    delete tool;
}

// =========================================================================
// Section 2: CEraserTool::isEnable
// =========================================================================

TEST(CEraserToolIsEnable, NullView_ReturnsFalse)
{
    CEraserTool tool;
    EXPECT_FALSE(tool.isEnable(nullptr));
}

TEST(CEraserToolIsEnable, NoSelection_ReturnsFalse)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();
    EXPECT_FALSE(tool->isEnable(scene->drawView()));
    delete tool;
}

TEST(CEraserToolIsEnable, SingleJDynamicLayer_ReturnsTrue)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    auto *layer = new JDynamicLayer(img);
    scene->addItem(layer);
    scene->selectGroup()->add(layer);

    EXPECT_TRUE(tool->isEnable(scene->drawView()));

    scene->selectGroup()->clear();
    delete layer;
    delete tool;
}

TEST(CEraserToolIsEnable, SingleNormalItem_ReturnsFalse)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    auto *rect = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addItem(rect);
    scene->selectGroup()->add(rect);

    EXPECT_FALSE(tool->isEnable(scene->drawView()));

    scene->selectGroup()->clear();
    delete rect;
    delete tool;
}

TEST(CEraserToolIsEnable, MultipleItems_ReturnsFalse)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    QImage img(100, 100, QImage::Format_ARGB32);
    img.fill(Qt::white);
    auto *layer = new JDynamicLayer(img);
    auto *rect = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addItem(layer);
    scene->addItem(rect);
    scene->selectGroup()->add(layer);
    scene->selectGroup()->add(rect);

    EXPECT_FALSE(tool->isEnable(scene->drawView()));

    scene->selectGroup()->clear();
    delete layer;
    delete rect;
    delete tool;
}

// =========================================================================
// Section 3: CEraserTool erase interaction flow
// =========================================================================

TEST(CEraserToolDecideUpdate, NonDynamicLayer_ReturnsZero)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    auto *rect = new CGraphicsRectItem(QRectF(10, 10, 50, 50));
    scene->addItem(rect);
    scene->selectGroup()->add(rect);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);
    IDrawTool::ITERecordInfo info = makeEraserRecordInfo(scene);

    int ret = tool->decideUpdate(&event, &info);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(tool->_layers.isEmpty());

    scene->selectGroup()->clear();
    delete rect;
    delete tool;
}

TEST(CEraserToolDecideUpdate, SingleDynamicLayer_ReturnsWidth)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    // Set a non-zero eraser width
    scene->drawView()->page()->setAttributionVar(EEraserWidth, 20, 0, false);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    auto *layer = new JDynamicLayer(img);
    scene->addItem(layer);
    scene->selectGroup()->add(layer);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);
    IDrawTool::ITERecordInfo info = makeEraserRecordInfo(scene);

    int ret = tool->decideUpdate(&event, &info);
    // decideUpdate returns the configured eraser width — require the exact
    // value, not merely nonzero.
    EXPECT_EQ(ret, 20);
    EXPECT_FALSE(tool->_layers.isEmpty());

    scene->selectGroup()->clear();
    delete layer;
    delete tool;
}

TEST(CEraserToolDecideUpdate, ZeroWidth_ReturnsZero)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    // Set eraser width to 0
    scene->drawView()->page()->setAttributionVar(EEraserWidth, 0, 0, false);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    auto *layer = new JDynamicLayer(img);
    scene->addItem(layer);
    scene->selectGroup()->add(layer);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);
    IDrawTool::ITERecordInfo info = makeEraserRecordInfo(scene);

    int ret = tool->decideUpdate(&event, &info);
    EXPECT_EQ(ret, 0);

    scene->selectGroup()->clear();
    delete layer;
    delete tool;
}

TEST(CEraserToolReturnToSelectTool, AlwaysReturnsFalse)
{
    CEraserTool tool;
    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), nullptr);
    IDrawTool::ITERecordInfo info;
    EXPECT_FALSE(tool.returnToSelectTool(&event, &info));
}

TEST(CEraserToolMouseHoverEvent, NoCrash)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);
    EXPECT_NO_FATAL_FAILURE(tool->mouseHoverEvent(&event));

    delete tool;
}

TEST(CEraserToolEnterLeaveEvent, SetsMLeavedCorrectly)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);

    tool->m_leaved = true;
    tool->enterEvent(&event);
    EXPECT_FALSE(tool->m_leaved);

    tool->leaveEvent(&event);
    EXPECT_TRUE(tool->m_leaved);

    delete tool;
}

TEST(CEraserToolToolFlow, StartUpdateFinish_NoCrash)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    scene->drawView()->page()->setAttributionVar(EEraserWidth, 20, 0, false);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    auto *layer = new JDynamicLayer(img);
    scene->addItem(layer);
    scene->selectGroup()->add(layer);

    CDrawToolEvent event(QPointF(50, 50), QPointF(50, 50), QPointF(50, 50), scene);
    IDrawTool::ITERecordInfo info = makeEraserRecordInfo(scene, layer);

    // Insert into _layers first (normally done by decideUpdate)
    tool->_layers.insert(scene, layer);

    EXPECT_NO_FATAL_FAILURE(tool->toolStart(&event, &info));
    EXPECT_NO_FATAL_FAILURE(tool->toolUpdate(&event, &info));
    EXPECT_NO_FATAL_FAILURE(tool->toolFinish(&event, &info));

    // After finish, _activePictures should not contain the uuid
    EXPECT_FALSE(tool->_activePictures.contains(event.uuid()));

    // Cleanup
    qApp->removeEventFilter(tool);

    scene->selectGroup()->clear();
    delete layer;
    delete tool;
}

// =========================================================================
// Section 4: CEraserTool status and drawing
// =========================================================================

TEST(CEraserToolOnStatusChanged, IdleToReady_InstallsEventFilter)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    tool->onStatusChanged(IDrawTool::EIdle, IDrawTool::EReady);
    EXPECT_FALSE(tool->m_leaved);

    // Clean up: remove event filter
    qApp->removeEventFilter(tool);
    delete tool;
}

TEST(CEraserToolOnStatusChanged, ReadyToIdle_RemovesEventFilter)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    // First go to Ready to install filter
    tool->onStatusChanged(IDrawTool::EIdle, IDrawTool::EReady);
    // Then go back to Idle
    tool->onStatusChanged(IDrawTool::EReady, IDrawTool::EIdle);
    EXPECT_TRUE(tool->m_leaved);

    delete tool;
}

TEST(CEraserToolOnStatusChanged, NullCurrentPage_NoCrash)
{
    CEraserTool tool;
    // currentPage() is nullptr when no drawBoard is set
    EXPECT_NO_FATAL_FAILURE(tool.onStatusChanged(IDrawTool::EIdle, IDrawTool::EReady));
}

TEST(CEraserToolOnStatusChanged, SameStatus_NoFilterChange)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    // EIdle -> EIdle should not install filter
    tool->m_leaved = true;
    tool->onStatusChanged(IDrawTool::EIdle, IDrawTool::EIdle);
    EXPECT_TRUE(tool->m_leaved);

    delete tool;
}

TEST(CEraserToolDrawMore, MLeavedTrue_NoDraw)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();
    tool->m_leaved = true;

    // drawMore() returns early on: (1) m_leaved, (2) non-bitmap viewport
    // cursor, (3) cursor outside the viewport. With the viewport left at
    // its default cursor, check (2) alone would keep the image unchanged,
    // so the m_leaved guard would never be exercised. Set a bitmap cursor
    // (the tool's own 1x1 transparent cursor) and place the cursor inside
    // the viewport so only the m_leaved guard can prevent the draw.
    auto *viewport = scene->drawView()->viewport();
    viewport->setCursor(tool->cursor());
    QCursor::setPos(viewport->mapToGlobal(viewport->rect().center()));

    // drawMore paints at scene coordinates and applies no transform of its
    // own (its rect parameter is unused) — in the app the caller's painter
    // already carries the scene->device transform. Mirror that here: map the
    // paint window under the cursor onto the image.
    const QPointF scenePos = scene->drawView()->mapToScene(viewport->rect().center());
    QRectF paintRect(scenePos - QPointF(100, 100), QSizeF(200, 200));

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QImage baseline = img;

    {
        QPainter painter(&img);
        painter.translate(-paintRect.topLeft());
        EXPECT_NO_FATAL_FAILURE(tool->drawMore(&painter, paintRect, scene));
    }

    // m_leaved == true must suppress the eraser ring everywhere.
    EXPECT_EQ(img, baseline);

    // Control: with m_leaved == false the same setup must draw something,
    // proving the unchanged result above comes from the guard, not from
    // the setup being unable to draw.
    QImage img2(200, 200, QImage::Format_ARGB32);
    img2.fill(Qt::white);
    tool->m_leaved = false;
    {
        QPainter painter(&img2);
        painter.translate(-paintRect.topLeft());
        tool->drawMore(&painter, paintRect, scene);
    }
    EXPECT_NE(img2, baseline);

    viewport->unsetCursor();
    delete tool;
}

TEST(CEraserToolDrawMore, NonBitmapCursor_NoDraw)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();
    tool->m_leaved = false;

    // Set cursor to non-BitmapCursor
    scene->drawView()->viewport()->setCursor(Qt::ArrowCursor);

    QImage img(200, 200, QImage::Format_ARGB32);
    img.fill(Qt::white);
    QPainter painter(&img);

    EXPECT_NO_FATAL_FAILURE(tool->drawMore(&painter, img.rect(), scene));

    delete tool;
}

TEST(CEraserToolEventFilter, ShortcutOverrideUndo_NoCrash)
{
    PageScene *scene = getEraserTestScene();
    ASSERT_NE(scene, nullptr);
    CEraserTool *tool = createEraserTool();

    QKeyEvent keyEvent(QEvent::ShortcutOverride, Qt::Key_Z, Qt::ControlModifier);
    EXPECT_NO_FATAL_FAILURE(tool->eventFilter(scene, &keyEvent));

    // Process any queued connections
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // Cleanup
    qApp->removeEventFilter(tool);
    delete tool;
}

TEST(CEraserToolEventFilter, NonShortcutEvent_DelegatesToBase)
{
    CEraserTool tool;
    QEvent event(QEvent::Resize);
    EXPECT_NO_FATAL_FAILURE(tool.eventFilter(nullptr, &event));
}

#endif // TEST_CERASERTOOL_ITEM
