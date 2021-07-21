/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao <zhanghao@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#define protected public
#define private public
#include "cgraphicsview.h"
#include <qaction.h>

#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>
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
#include "cgraphicscutitem.h"
#include "blurwidget.h"

#include <QDebug>
#include <DLineEdit>

#include "publicApi.h"

#undef protected
#undef private

#if TEST_PICTURE_ITEM

TEST(PictureItem, TestPictureItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(PictureItem, TestDrawPictureItem)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    int addedCount = view->drawScene()->getBzItems().count();
    QString path = QApplication::applicationDirPath() + "/test.png";
    QPixmap pix(":/test.png");
    ASSERT_EQ(true, pix.save(path, "PNG"));
    QTest::qWait(300);

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(path));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(300);

    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), DyLayer);
}

TEST(PictureItem, TestBlurPictureItem)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    Page *c = getMainWindow()->drawBoard()->currentPage();

    ASSERT_NE(c, nullptr);

    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), DyLayer);

    auto pPictureItem = dynamic_cast<JDynamicLayer *>(view->drawScene()->getBzItems().first());

    view->drawScene()->selectItem(pPictureItem);

    //1.切换到模糊工具下
    drawApp->setCurrentTool(blur);
    QTest::qWait(200);

    //2.设置模糊参数
    BlurWidget *pBLurAttriWidget = drawApp->topToolbar()->findChild<BlurWidget *>("BlurWidget");

    ASSERT_NE(pBLurAttriWidget, nullptr);

    pBLurAttriWidget->setBlurType(BlurEffect);
    QTest::qWait(100);

    // Blur width
    pBLurAttriWidget->setBlurWidth(30);
    QTest::qWait(1000);

    //3.对图片进行模糊
    QPoint picturePointInGloble = view->mapFromScene(pPictureItem->sceneBoundingRect().topLeft());
    QSize pictureSize = pPictureItem->rect().size().toSize();
    QRect rectInView(picturePointInGloble, pictureSize);

    DTestEventList blurEvent;
    blurEvent.clear();
    blurEvent.addMouseMove(rectInView.topLeft(), 100);

    blurEvent.addMousePress(Qt::LeftButton, Qt::NoModifier, rectInView.topLeft(), 100);
    blurEvent.addMouseMove(rectInView.center(), 100);
    blurEvent.addMouseMove(rectInView.bottomRight(), 100);

    blurEvent.addMouseRelease(Qt::LeftButton, Qt::NoModifier, rectInView.bottomRight(), 100);
    blurEvent.simulate(view->viewport());

    //ASSERT_EQ(pPictureItem->_blurInfos.count(), 1);

    //切换模糊类型再测试一次
    {
        // Blur Masic Type测试马赛克式模糊
        pBLurAttriWidget->setBlurType(MasicoEffect);
        QTest::qWait(100);

        DTestEventList blurEvent;
        blurEvent.clear();
        blurEvent.addMouseMove(rectInView.bottomLeft(), 100);

        blurEvent.addMousePress(Qt::LeftButton, Qt::NoModifier, rectInView.bottomLeft(), 100);
        blurEvent.addMouseMove(rectInView.center(), 100);
        blurEvent.addMouseMove(rectInView.topRight(), 100);

        blurEvent.addMouseRelease(Qt::LeftButton, Qt::NoModifier, rectInView.topRight(), 100);
        blurEvent.addMouseClick(Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);
        blurEvent.simulate(view->viewport());

        //ASSERT_EQ(pPictureItem->_blurInfos.count(), 2);
    }

    view->drawScene()->selectItem(pPictureItem);
}


TEST(PictureItem, TestCopyPictureItem)
{
    keyShortCutCopyItem();

    PageView *view = getCurView();

    ASSERT_NE(view, nullptr);

    auto allItems = view->drawScene()->getBzItems();

    ASSERT_EQ(allItems.count(), 2);
}

TEST(PictureItem, TestPictureItemProperty)
{
    PageView *view = getCurView();

    ASSERT_NE(view, nullptr);

    auto allItems = view->drawScene()->getBzItems();

    ASSERT_EQ(allItems.count(), 2);

    view->drawScene()->clearSelectGroup();

    view->drawScene()->selectItem(allItems.last());

    auto selectionsItems0 = view->drawScene()->selectGroup()->items();

    ASSERT_EQ(selectionsItems0.count(), 1);

    ASSERT_EQ(selectionsItems0.first()->type(), /*PictureType*/DyLayer);

    auto fDoOperate = [ = ]() {
        // 左旋转
        QPushButton *btn = drawApp->topToolbar()->findChild<QPushButton *>("PicLeftRotateBtn");
        ASSERT_NE(btn, nullptr);
        emit btn->clicked();
        QTest::qWait(100);

        // 右旋转
        btn = drawApp->topToolbar()->findChild<QPushButton *>("PicRightRotateBtn");
        ASSERT_NE(btn, nullptr);
        emit btn->clicked();
        QTest::qWait(100);

        // 水平翻转
        btn = drawApp->topToolbar()->findChild<QPushButton *>("PicFlipHBtn");
        ASSERT_NE(btn, nullptr);
        emit btn->clicked();
        QTest::qWait(100);

        // 水平翻转
        btn = drawApp->topToolbar()->findChild<QPushButton *>("PicFlipVBtn");
        ASSERT_NE(btn, nullptr);
        emit btn->clicked();
        QTest::qWait(100);

        // 自适应scence
        btn = drawApp->topToolbar()->findChild<QPushButton *>("PicFlipAdjustmentBtn");
        ASSERT_NE(btn, nullptr);
        emit btn->clicked();
        QTest::qWait(100);
    };

    fDoOperate();

    view->drawScene()->clearSelectGroup();

    view->drawScene()->selectItem(allItems.first());

    view->drawScene()->selectItem(allItems.last());

    auto selectionsItems1 = view->drawScene()->selectGroup()->items();

    ASSERT_EQ(selectionsItems1.count(), 2);

    fDoOperate();
}

TEST(PictureItem, TestPictureItemPenetrable)
{
    PageView *view = getCurView();

    ASSERT_NE(view, nullptr);

    auto allItems = view->drawScene()->getBzItems();

    ASSERT_EQ(allItems.count(), 2);

    //allItems.first()->rasterToSelfLayer(false);

    allItems = view->drawScene()->getBzItems();

    ASSERT_EQ(allItems.count(), 2);

    auto pictureItem = dynamic_cast</*CPictureItem*/JDynamicLayer *>(allItems.first());

    ASSERT_NE(pictureItem, nullptr);

    ASSERT_EQ(pictureItem->isPosPenetrable(QPointF(0, 0)), false);
    ASSERT_EQ(pictureItem->isRectPenetrable(QRectF()), false);

    //ASSERT_EQ(pictureItem->rasterSelf().isNull(), false);
}

TEST(PictureItem, TestResizePictureItem)
{
    resizeItem();
}

TEST(PictureItem, TestSelectAllPictureItem)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 全选图元
    DTestEventList e;
    e.addMouseMove(QPoint(20, 20), 100);
    e.addMousePress(Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 100);
    e.addMouseMove(QPoint(1800, 900), 100);
    e.addMouseRelease(Qt::LeftButton, Qt::NoModifier, QPoint(1000, 1000), 100);
    e.addKeyPress(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyRelease(Qt::Key_A, Qt::ControlModifier, 100);
    e.simulate(view->viewport());

    // 水平等间距对齐
    view->m_itemsVEqulSpaceAlign->triggered(true);
    // 垂直等间距对齐
    view->m_itemsHEqulSpaceAlign->triggered(true);

    //滚轮事件
    QWheelEvent wheelevent(QPointF(1000, 1000), 100, Qt::MouseButton::NoButton, Qt::KeyboardModifier::ControlModifier);
    view->wheelEvent(&wheelevent);
    QWheelEvent wheelevent2(QPointF(1000, 1000), 100, Qt::MouseButton::NoButton, Qt::KeyboardModifier::NoModifier);
    view->wheelEvent(&wheelevent2);
    QWheelEvent wheelevent3(QPointF(1000, 1000), 100, Qt::MouseButton::NoButton, Qt::KeyboardModifier::ShiftModifier);
    view->wheelEvent(&wheelevent3);
}

TEST(PictureItem, TestSavePictureItemToFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString PictureItemPath = QApplication::applicationDirPath() + "/test_picture.ddf";
    c->setFile(PictureItemPath);
    c->save(true);

    QFileInfo info(PictureItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(PictureItem, TestOpenPictureItemFromFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString PictureItemPath = QApplication::applicationDirPath() + "/test_picture.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(PictureItemPath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    (void)QTest::qWaitFor([ = ]() {return (view != getCurView() && getCurView()->drawScene()->getBzItems().count());});

    view = getCurView();
    ASSERT_NE(view, nullptr);
    int addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    ASSERT_EQ(addedCount, 2);
}

#endif
