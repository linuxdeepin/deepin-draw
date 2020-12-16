/*

* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.

*

* Author: Wang Yicun <wangyicun@uniontech.com>

*

* Maintainer: Wang Yicun <wangyicun@uniontech.com>

*

* This program is free software: you can redistribute it and/or modify

* it under the terms of the GNU General Public License as published by

* the Free Software Foundation, either version 3 of the License, or

* any later version.

*

* This program is distributed in the hope that it will be useful,

* but WITHOUT ANY WARRANTY; without even the implied warranty of

* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the

* GNU General Public License for more details.

*

* You should have received a copy of the GNU General Public License

* along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

#define protected public
#define private public
#include "cgraphicsitem.h"
#include "cspinbox.h"
#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicscutitem.h"
#include "csizehandlerect.h"
#include "qgraphicssceneevent.h"
#include "cviewmanagement.h"
#include "cmultiptabbarwidget.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "clefttoolbar.h"
#include "globaldefine.h"
#include "ccuttool.h"
#include "idrawtool.h"
#include "cellipsetool.h"
#include "clinetool.h"
#include "cmasicotool.h"
#undef protected
#undef private

#include <QTimer>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>


#include "toptoolbar.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"
#include "qevent.h"

#include "crecttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"
#include "publicApi.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>

#include "sitemdata.h"


#include <QDebug>
#include <QtTest>
#include <QTestEventList>
#include <DLineEdit>

#if TEST_FUNCTION

TEST(TestFunction, TestCgraphicsItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsItem *grap = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());
    grap->getCenter(CSizeHandleRect::LeftTop);
    grap->getCenter(CSizeHandleRect::RightTop);
    grap->getCenter(CSizeHandleRect::Top);
    grap->getCenter(CSizeHandleRect::Right);
    grap->getCenter(CSizeHandleRect::RightBottom);
    grap->getCenter(CSizeHandleRect::Bottom);
    grap->getCenter(CSizeHandleRect::LeftBottom);
    grap->getCenter(CSizeHandleRect::Left);
    grap->getCenter(CSizeHandleRect::Rotation);
    grap->getCenter(CSizeHandleRect::InRect);
    grap->getCenter(CSizeHandleRect::None);

    grap->getSelfOrgShape();

    QGraphicsSceneMouseEvent Event(QEvent::None);
    grap->mousePressEvent(&Event);
    grap->mouseMoveEvent(&Event);
    grap->mouseReleaseEvent(&Event);
    grap->mouseDoubleClickEvent(&Event);

    grap->blurBegin(QPointF(1, 1));
    grap->blurUpdate(QPointF(1, 1));
    grap->blurEnd();

    struct SBlurInfo sblurinfo;
    grap->addBlur(sblurinfo);
    QPainter *painter = nullptr;
    grap->paintBlur(painter, sblurinfo);
    grap->resetCachePixmap();

    grap->scenRect();
    grap->type();

    bool b = true;
    bool c = false;
    grap->setFilpBaseOrg(CPictureItem::EFilpHor, b);
    grap->setFilpBaseOrg(CPictureItem::EFilpVer, b);

    grap->setCacheEnable(b);
    grap->setCacheEnable(c);

    int a = 1;
    grap->setAutoCache(b, a);
    grap->isAutoCache();

    QRectF rect;
    grap->isRectPenetrable(rect);
    qreal angle = true;
    grap->rotatAngle(angle);
    grap->setBzZValue(angle);

    grap->creatSameItem();
    CGraphicsUnit data;
    grap->loadGraphicsUnit(data);
    grap->hitTest(QPointF(1, 1));

    QStyleOptionGraphicsItem *style = nullptr;
    grap->paintSelf(painter, style);

    grap->isGrabToolEvent();

    CGraphicsUnit un;
    grap->getGraphicsUnit(ENormal);
    grap->loadGraphicsUnit(un);
}

TEST(TestFunction, TestCgraphicsCutItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsCutItem cutitem;
    cutitem.move(QPointF(0, 0), QPointF(0, 0));
    cutitem.hitTest(QPointF(0, 0));
    cutitem.getHighLightPath();
    cutitem.isPosPenetrable(QPointF(0, 0));
    QPointF point(1, 1);
    cutitem.resizeCutSize(CSizeHandleRect::Left, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::LeftTop, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::Top, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::RightTop, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::Right, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::RightBottom, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::Bottom, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::LeftBottom, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::Rotation, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::InRect, QPointF(1, 1), QPointF(1, 1), &point);
    cutitem.resizeCutSize(CSizeHandleRect::None, QPointF(1, 1), QPointF(1, 1), &point);

    cutitem.getWHRadio();
    cutitem.nodes();
}

TEST(TestFunction, TestCgraphicsTextItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsTextItem textitem;
    textitem.initTextEditWidget();
    textitem.updateSelectAllTextProperty();
    textitem.beginPreview();
    bool loadorg = true;
    textitem.endPreview(loadorg);
    bool loador = false;
    textitem.endPreview(loador);
    textitem.isPreview();
    QRectF qrect;
    textitem.setRect(qrect);
    textitem.getTextFontStyle();
    QString str = "a";
    textitem.setTextFontStyle(str);
    CGraphicsUnit data;
    data.reson = ENormal;
    textitem.loadGraphicsUnit(data);
//    CGraphItemEvent *event = nullptr;
//    textitem.operatingEnd(event);
    textitem.setSelectTextBlockAlign(Qt::AlignTop);
    textitem.setSelectTextBlockAlign(Qt::AlignBottom);
    textitem.setSelectTextBlockAlign(Qt::AlignVCenter);
    textitem.setSelectTextBlockAlign(Qt::AlignCenter);
    textitem.doUndo();
    textitem.doRedo();
    textitem.doDelete();
}

TEST(TestFunction, TestSitemdata)
{
    CGraphicsUnit unit;
    CGraphicsUnit cs;
    CGraphicsUnit a;
    cs.data.pRect = nullptr;
    a.head.dataType = RectType;
    unit.deepCopy(cs, a);
    cs.data.pCircle = nullptr;
    a.head.dataType = EllipseType;
    unit.deepCopy(cs, a);
    cs.data.pTriangle = nullptr;
    a.head.dataType = TriangleType;
    unit.deepCopy(cs, a);
    cs.data.pPolygonStar = nullptr;
    a.head.dataType = PolygonalStarType;
    unit.deepCopy(cs, a);
    cs.data.pPolygon = nullptr;
    a.head.dataType = PolygonType;
    unit.deepCopy(cs, a);
    cs.data.pLine = nullptr;
    a.head.dataType = LineType;
    unit.deepCopy(cs, a);
    cs.data.pPen = nullptr;
    a.head.dataType = PenType;
    unit.deepCopy(cs, a);
    cs.data.pText = nullptr;
    a.head.dataType = TextType;
    unit.deepCopy(cs, a);
    cs.data.pPic = nullptr;
    a.head.dataType = PictureType;
    unit.deepCopy(cs, a);
    cs.data.pBlur = nullptr;
    a.head.dataType = BlurType;
    unit.deepCopy(cs, a);
    cs.data.pGroup = nullptr;
    a.head.dataType = MgrType;
    unit.deepCopy(cs, a);
}

TEST(TestFunction, TestCviewmanagement)
{
    CManageViewSigleton sigleton;
    int type = 0;
    sigleton.setThemeType(type);
    sigleton.updateTheme();
    QString ddfFile = "rect";
    sigleton.onDDfFileChanged(ddfFile);
    sigleton.onDdfFileChanged(ddfFile, type);
    sigleton.getNoticeFileDialog(ddfFile);
    QWidget parent;
    sigleton.creatOneNoticeFileDialog(ddfFile, &parent);
    DDialog dialog;
    sigleton.removeNoticeFileDialog(&dialog);
    sigleton.getFileSrcData(ddfFile);
}

TEST(TestFunction, TestMultiptabbarwidget)
{
    CMultipTabBarWidget tabbar;
    tabbar.getAllTabBarUUID();
    tabbar.clearHoverState();
    QObject o;
    QMouseEvent *mouseevent = new QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(1, 1), Qt::MouseButton::LeftButton,
                                              Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
    tabbar.eventFilter(&o, mouseevent);
}

TEST(TestFunction, TestCentralWidget)
{
    CGraphicsView *view = getCurView();
    QString filepath = "Unnamed";
    QStringList str;
    str << "rect" << "ellipse";
    DWidget parent;
    CCentralwidget *tralwidget = new CCentralwidget(str, &parent);
    tralwidget->getLeftToolBar();
    tralwidget->getGraphicsView();
    int type = 1;
    tralwidget->switchTheme(type);
    tralwidget->getAllTabBarUUID();

//    tralwidget->skipOpenedTab(filepath);
    bool isimagesize = true;
    bool a = true;
//    tralwidget->loadFilesByCreateTag(str, isimagesize);
    tralwidget->closeSceneView(view, isimagesize, a);
//    tralwidget->slotNew();
    tralwidget->slotShowCutItem();
//    tralwidget->slotLoadDragOrPasteFile(filepath);
    tralwidget->slotShowExportDialog();
    qreal scale = true;
    tralwidget->slotSetScale(scale);
    tralwidget->getSceneImage(type);
    tralwidget->initConnect();
}

TEST(TestFunction, TestGraphicsview)
{
    CGraphicsView *view = getCurView();
    view->slotDoCutScene();
    view->slotViewZoomIn();
    view->slotViewZoomOut();
    view->slotViewOrignal();
    view->slotSetTextAlignment(Qt::AlignmentFlag::AlignLeft);
    bool type = true;
    bool finishClose = false;
    QString save = "sad";
    QTimer::singleShot(3000, view, [ = ]() {
        auto dial = qobject_cast<QFileDialog *>(qApp->activeModalWidget());
        if (dial != nullptr) {
            dial->done(0);
        }
    });
    view->setSaveDialogMoreOption(QFileDialog::DontUseNativeDialog);
    view->showSaveDDFDialog(type, finishClose, save);
    view->setSaveDialogMoreOption(QFileDialog::DontResolveSymlinks);

    view->getSelectedValidItems();
    view->getCouldPaste();
    QPanGesture gesture;
    view->panTriggered(&gesture);
    QPinchGesture gesture2;
    view->pinchTriggered(&gesture2);
    QSwipeGesture gesture3;
    view->swipeTriggered(&gesture3);
}

TEST(TestFunction, TestLefttoolbar)
{
    CLeftToolBar lefttoolbar;
    lefttoolbar.toolButton(EDrawToolMode::importPicture);
    lefttoolbar.toolButton(EDrawToolMode::rectangle);
    lefttoolbar.toolButton(EDrawToolMode::ellipse);
    lefttoolbar.toolButton(EDrawToolMode::triangle);
    lefttoolbar.toolButton(EDrawToolMode::polygonalStar);
    lefttoolbar.toolButton(EDrawToolMode::polygon);
    lefttoolbar.toolButton(EDrawToolMode::line);
    lefttoolbar.toolButton(EDrawToolMode::pen);
    lefttoolbar.toolButton(EDrawToolMode::text);
    lefttoolbar.toolButton(EDrawToolMode::blur);
    lefttoolbar.toolButton(EDrawToolMode::cut);
    QMouseEvent mouseevent(QEvent::None, QPointF(1, 1), Qt::MouseButton::NoButton,
                           Qt::MouseButton::NoButton, Qt::KeyboardModifier::NoModifier);
    lefttoolbar.mouseMoveEvent(&mouseevent);
    lefttoolbar.enterEvent(&mouseevent);
    lefttoolbar.slotShortCutPictrue();
    lefttoolbar.slotShortCutRect();
    lefttoolbar.slotShortCutRound();
    lefttoolbar.slotShortCutTriangle();
    lefttoolbar.slotShortCutPolygonalStar();
    lefttoolbar.slotShortCutPolygon();
    lefttoolbar.slotShortCutLine();
    lefttoolbar.slotShortCutPen();
    lefttoolbar.slotShortCutText();
    lefttoolbar.slotShortCutBlur();
    lefttoolbar.slotShortCutCut();
    lefttoolbar.initShortCut();

    QObject o;
    CMultipTabBarWidget tiptabar;
    tiptabar.getAllTabBarUUID();
    tiptabar.getAllTabBarName();
    tiptabar.eventFilter(&o, &mouseevent);
}

TEST(TestFunction, TestDrawTools)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    QString uuid = "123";
    bool isModified = false;
    CDrawScene *scence = new CDrawScene(view, uuid, isModified);
    IDrawTool::CDrawToolEvent event(QPointF(1, 1), QPointF(1, 1), QPointF(1, 1), scence);
    IDrawTool::ITERecordInfo info;
    CCutTool cuttool;
    cuttool.toolStart(&event, &info);
    cuttool.toolUpdate(&event, &info);
    cuttool.toolFinish(&event, &info);
    cuttool.mouseHoverEvent(&event);
    cuttool.getCutRect(scence);

    CEllipseTool ellipsetool;
    ellipsetool.toolCreatItemUpdate(&event, &info);

    CLineTool linetool;
    linetool.toolCreatItemUpdate(&event, &info);
    linetool.toolCreatItemFinish(&event, &info);

    CMasicoTool masicotool;
    masicotool.toolCreatItemUpdate(&event, &info);
    masicotool.toolCreatItemFinish(&event, &info);
    masicotool.decideUpdate(&event, &info);
    masicotool.updateRealTimePixmap(scence);
}

#endif
