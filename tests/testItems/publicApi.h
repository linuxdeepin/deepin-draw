/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zhanghao<zhanghao@uniontech.com>
* Maintainer: zhanghao<zhanghao@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MIANWINDOW_H
#define MIANWINDOW_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "mainwindow.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "application.h"
#include "cgraphicsview.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "cdrawscene.h"

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
#include "cgraphicsitemselectedmgr.h"

#include <DComboBox>
#include <DLineEdit>
#include <DWidget>
#include <DSlider>

#include "calphacontrolwidget.h"
#include <dzoommenucombobox.h>
#include "cspinbox.h"
#include "bordercolorbutton.h"
#include "bigcolorbutton.h"
#include "ccolorpickwidget.h"
#include "ciconbutton.h"
#include "cgraphicsitem.h"
#include "pickcolorwidget.h"

#include <QBrush>
#include <QtTest>
#include <QTestEventList>

#define ON true
#define OFF false
#define TEST_PICTURE_ITEM ON
#define TEST_RECT_ITEM ON
#define TEST_ELLIPSE_ITEM ON
#define TEST_TRIANGLE_ITEM ON
#define TEST_START_ITEM ON
#define TEST_POLYGON_ITEM ON
#define TEST_LINE_ITEM ON
#define TEST_PEN_ITEM ON
#define TEST_TEXT_ITEM ON
#define TEST_BLUR_ITEM ON
#define TEST_CUT_ITEM ON
#define TEST_SCANLE_SCENCE ON
#define TEST_DELETE_ITEM ON

DWIDGET_USE_NAMESPACE

static MainWindow *getMainWindow()
{
    if (drawApp->topMainWindow() == nullptr) {
        drawApp->showMainWindow(QStringList());
        drawApp->topMainWindow()->showMaximized();
    }
    return drawApp->topMainWindow();
}

static CGraphicsView *getCurView()
{
    if (getMainWindow() != nullptr) {
        return drawApp->topMainWindow()->getCCentralwidget()->getGraphicsView();
    }
    return nullptr;
}

static void  createNewViewByShortcutKey()
{
    int i = 0;
    while (i++ < 50) {
        QTest::qWait(200);
        if (getCurView() != nullptr) {
            break;
        }
    }
    if (getCurView() == nullptr) {
        qDebug() << __FILE__ << __LINE__ << "get CGraphicsView is nullptr.";
    }
    ASSERT_NE(getCurView(), nullptr);

    QTestEventList e;
    e.addKeyClick(Qt::Key_N, Qt::ControlModifier);
    e.simulate(getCurView());
    QTest::qWait(200);

    i = 0;
    while (i++ < 50) {
        QTest::qWait(200);
        if (getCurView() != nullptr) {
            break;
        }
    }
    if (getCurView() == nullptr) {
        qDebug() << __FILE__ << __LINE__ << "get CGraphicsView is nullptr.";
    }
    ASSERT_NE(getCurView(), nullptr);
}

//升序排列用
static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() < info2->zValue();
}

inline void setPenWidth(CGraphicsItem *item, int width)
{
    int defaultWidth = item->pen().width();

    DComboBox *sideComBox = drawApp->topToolbar()->findChild<DComboBox *>("Line width combox");
    // pen width 0 1 2 4 8 10 px
    if (width == 0 || width == 1 || width == 2) {
        sideComBox->setCurrentIndex(width);
    } else if (width == 4) {
        sideComBox->setCurrentIndex(3);
    } else if (width == 8 || width == 10) {
        sideComBox->setCurrentIndex(width / 2);
    }
    QTest::qWait(100);

    QTestEventList e;
    e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
    e.simulate(item->drawScene()->drawView()->viewport());
    ASSERT_EQ(item->pen().width(), defaultWidth);
    e.clear();
    e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
    e.simulate(item->drawScene()->drawView()->viewport());
    ASSERT_EQ(item->pen().width(), width);
}

inline void setStrokeColor(CGraphicsItem *item, QColor color)
{
    QColor defaultColor = item->pen().color();
    BorderColorButton *stroke = drawApp->topToolbar()->findChild<BorderColorButton *>("stroken color button");
    stroke->setColor(color);
    QTest::qWait(100);

    QTestEventList e;
    e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
    e.simulate(item->drawScene()->drawView()->viewport());
    ASSERT_EQ(item->pen().color(), defaultColor);
    e.clear();
    e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
    e.simulate(item->drawScene()->drawView()->viewport());
    ASSERT_EQ(item->pen().color(), color);
}

inline void setBrushColor(CGraphicsItem *item, QColor color)
{
    QColor defaultColor = item->brush().color();
    BigColorButton *brush = drawApp->topToolbar()->findChild<BigColorButton *>("fill color button");
    brush->setColor(color);
    QTest::qWait(100);

    QTestEventList e;
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
    e.simulate(item->drawScene()->drawView()->viewport());
    ASSERT_EQ(item->brush().color(), defaultColor);
    e.clear();
    e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
    e.simulate(item->drawScene()->drawView()->viewport());
    ASSERT_EQ(item->brush().color(), color);

    item = dynamic_cast<CGraphicsItem *>(item->drawScene()->getBzItems().first());
    //   [0]  show colorPanel
    QMouseEvent mousePressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(brush, &mousePressEvent);
    QTest::qWait(100);
    CColorPickWidget *pickColor = drawApp->colorPickWidget();
    ASSERT_NE(pickColor, nullptr);

    //  [1]  Color  LineEdit
    DLineEdit *colorLineEdit = pickColor->findChild<DLineEdit *>("ColorLineEdit");
    QTest::qWait(100);
    ASSERT_NE(colorLineEdit, nullptr);
    colorLineEdit->setText("8fc31f");
    QTest::qWait(200);
    ASSERT_EQ(item->brush().color(), QColor("#8fc31f"));

    //  [2]  Color  Alpha
    CAlphaControlWidget *alphaControlWidget = pickColor->findChild<CAlphaControlWidget *>("CAlphaControlWidget");
    DSlider *slider = alphaControlWidget->findChild<DSlider *>("Color Alpha slider");
    ASSERT_NE(slider, nullptr);
    slider->setValue(155);
    ASSERT_EQ(item->paintBrush().color().alpha(), slider->value());

    //  [3]  show expand Color  Panel
    CIconButton *iconbutton = pickColor->findChild<CIconButton *>("CIconButton");
    ASSERT_NE(iconbutton, nullptr);
    QEvent event(QEvent::Enter);
    dApp->sendEvent(iconbutton, &event);
    QTest::qWait(100);
    dApp->sendEvent(iconbutton, &mousePressEvent);
    QTest::qWait(100);
    QMouseEvent mouseReleaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(iconbutton, &mouseReleaseEvent);
    QTest::qWait(100);
    event = QEvent(QEvent::Leave);
    dApp->sendEvent(iconbutton, &event);
    QTest::qWait(100);

    //  [4]  picker color
    PickColorWidget *picker = pickColor->findChild<PickColorWidget *>("PickColorWidget");
    color.setAlpha(100);
    picker->setColor(color);
    QTest::qWait(100);
    ASSERT_EQ(item->paintBrush().color(), color);
}

inline void resizeItem()
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pItem, nullptr);

    view->drawScene()->clearSelectGroup();
    view->drawScene()->selectItem(pItem);

    QVector<CSizeHandleRect *> handles = view->drawScene()->selectGroup()->handleNodes();

    int delay = 50;

    // 普通拉伸
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QTestEventList e;
        e.addMouseMove(posInView, delay);
        e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, posInView, delay);
        e.addMouseMove(posInView + QPoint(20, 20), delay);
        e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, posInView + QPoint(20, 20), delay);
        e.simulate(view->viewport());

        //  Undo Redo
        e.clear();
        e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, delay);
        e.addKeyRelease(Qt::Key_Z, Qt::ControlModifier, delay);
        e.addDelay(100);
        e.clear();
        e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, delay);
        e.addKeyRelease(Qt::Key_Y, Qt::ControlModifier, delay);
        e.simulate(view->viewport());
    }

    // SHIFT   ALT拉伸:  QTestEvent mouseMove 中移动鼠标的实现是直接设置全局鼠标位置 5.15中解决了此问题
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QMouseEvent mouseEvent(QEvent::MouseButtonPress, posInView, Qt::LeftButton, Qt::LeftButton, Qt::ShiftModifier);
        QApplication::sendEvent(view->viewport(), &mouseEvent);
        QTest::qWait(delay);
        QMouseEvent mouseEvent1(QEvent::MouseMove, posInView + QPoint(20, 20), Qt::LeftButton, Qt::LeftButton, Qt::ShiftModifier);
        QApplication::sendEvent(view->viewport(), &mouseEvent1);
        QTest::qWait(delay);
    }
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QMouseEvent mouseEvent(QEvent::MouseButtonPress, posInView, Qt::LeftButton, Qt::LeftButton, Qt::AltModifier);
        QApplication::sendEvent(view->viewport(), &mouseEvent);
        QTest::qWait(delay);
        QMouseEvent mouseEvent1(QEvent::MouseMove, posInView - QPoint(20, 20), Qt::LeftButton, Qt::LeftButton, Qt::AltModifier);
        QApplication::sendEvent(view->viewport(), &mouseEvent1);
        QTest::qWait(delay);
    }
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QMouseEvent mouseEvent(QEvent::MouseButtonPress, posInView, Qt::LeftButton, Qt::LeftButton, Qt::ShiftModifier | Qt::AltModifier);
        QApplication::sendEvent(view->viewport(), &mouseEvent);
        QTest::qWait(delay);
        QMouseEvent mouseEvent1(QEvent::MouseMove, posInView + QPoint(20, 20), Qt::LeftButton, Qt::LeftButton, Qt::ShiftModifier | Qt::AltModifier);
        QApplication::sendEvent(view->viewport(), &mouseEvent1);
        QTest::qWait(delay);
    }

    // 全选拉伸
    view->slotOnSelectAll();
    handles = view->drawScene()->selectGroup()->handleNodes();
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QTestEventList e;
        e.addMouseMove(posInView, 100);
        e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, posInView, 100);
        e.addMouseMove(posInView - QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, posInView - QPoint(50, 50), 100);
        e.simulate(view->viewport());
    }
}

inline void createItemByMouse(CGraphicsView *view, bool altCopyItem = false, QPoint topLeft = QPoint(500, 300)
                                                                                              , QPoint bottomRight = QPoint(600, 400), bool doUndoRedo = true)
{
    QTestEventList e;
    e.clear();
    e.addMouseMove(topLeft, 100);
    e.addKeyPress(Qt::Key_Shift, Qt::NoModifier, 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, topLeft, 100);
    e.addMouseMove(bottomRight / 2, 100);
    e.addKeyRelease(Qt::Key_Shift, Qt::NoModifier, 100);
    e.addMouseMove(bottomRight, 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, bottomRight, 100);
    e.addMouseClick(Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 100);
    e.simulate(view->viewport());

    // alt move copy item will not sucess,because move event has no modifier
    if (altCopyItem) {
        e.clear();
        QPoint p = bottomRight - QPoint(30, 30);
        e.addMouseMove(p, 100);
        e.addKeyPress(Qt::Key_Alt, Qt::AltModifier, 100);
        e.addMousePress(Qt::LeftButton, Qt::AltModifier, p, 100);
        e.addMouseMove(bottomRight + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::AltModifier, bottomRight + QPoint(50, 50), 100);
        e.addKeyRelease(Qt::Key_Alt, Qt::AltModifier, 100);
        e.simulate(view->viewport());
    }

    if (doUndoRedo) {
        int addedCount = view->drawScene()->getBzItems().count();
        QTestEventList e;
        e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
        e.simulate(view->viewport());
        ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount - 1);

        e.clear();
        addedCount = view->drawScene()->getBzItems().count();
        e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
        e.simulate(view->viewport());
        ASSERT_EQ(view->drawScene()->getBzItems().count(),  addedCount + 1);
    }
}

inline void createItemWithkeyShift(CGraphicsView *view, bool altCopyItem = true, QPoint startPos = QPoint(200, 100)
                                                                                                   , QPoint endPos = QPoint(400, 300))
{
    QPoint topRight = startPos + QPoint(100, -50);
    QPoint topLeft = startPos + QPoint(-100, -50);
    QPoint bottomLeft = startPos + QPoint(-100, 50);
    QTestEventList e;
    e.clear();
    e.addMouseMove(topLeft, 100);
    e.addKeyPress(Qt::Key_Shift, Qt::NoModifier, 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, topLeft, 100);
    e.addMouseMove(topRight, 300);
    e.addMouseMove(topLeft, 300);
    e.addMouseMove(bottomLeft, 300);
    e.addMouseMove(endPos, 300);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, endPos, 100);
    e.addKeyRelease(Qt::Key_Shift, Qt::NoModifier, 100);
    e.simulate(view->viewport());

    if (altCopyItem) {
        e.clear();
        e.addMouseMove(endPos / 2, 100);
        e.addMousePress(Qt::LeftButton, Qt::AltModifier, endPos / 2, 100);
        e.addMouseMove(endPos + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::AltModifier, endPos + QPoint(50, 50), 100);
        e.simulate(view->viewport());
    }
}
inline void keyShortCutCopyItem()
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pItem, nullptr);

    view->drawScene()->clearSelectGroup();
    view->drawScene()->selectItem(pItem);

    int addedCount = view->drawScene()->getBzItems().count();
    QTestEventList e;
    e.addKeyPress(Qt::Key_C, Qt::ControlModifier, 100);
    e.addKeyPress(Qt::Key_V, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
}

inline void selectAllItem()
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    QTestEventList e;
    e.addMouseMove(QPoint(20, 20), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);
    e.addMouseMove(QPoint(1800, 900), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(1000, 1000), 100);
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_A, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
}

inline void itemAlignment(CGraphicsView *view)
{
    QTestEventList e;
    e.addKeyPress(Qt::Key_L, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addKeyRelease(Qt::Key_L, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_R, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addKeyRelease(Qt::Key_R, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_T, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addKeyRelease(Qt::Key_T, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_B, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addKeyRelease(Qt::Key_B, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_V, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addKeyRelease(Qt::Key_V, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_H, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addKeyRelease(Qt::Key_H, Qt::ControlModifier | Qt::ShiftModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_Z, Qt::ControlModifier, 100);
    e.addDelay(300);
    e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_Y, Qt::ControlModifier, 100);
    e.simulate(view->viewport());
}

#endif // MIANWINDOW_H
