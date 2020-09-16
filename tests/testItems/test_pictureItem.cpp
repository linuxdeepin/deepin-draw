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
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "frame/cgraphicsview.h"
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
#include "cgraphicsmasicoitem.h"
#include "cgraphicscutitem.h"

#include <QDebug>
#include <QtTest>
#include <QTestEventList>
#include <DLineEdit>

#include "publicApi.h"

#if TEST_PICTURE_ITEM

TEST(PictureItem, TestPictureItemCreateView)
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

TEST(PictureItem, TestDrawPictureItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
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
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), PictureType);
}


TEST(PictureItem, TestCopyPictureItem)
{
    keyShortCutCopyItem();
}

TEST(PictureItem, TestPictureItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
//    CGraphicsItem *item = dynamic_cast<CGraphicsItem *>(view->drawScene()->getBzItems().first());

    // 左旋转
    QPushButton *btn = dApp->topToolbar()->findChild<QPushButton *>("PicLeftRotateBtn");
    ASSERT_NE(btn, nullptr);
    btn->released();
    QTest::qWait(100);

    // 右旋转
    btn = dApp->topToolbar()->findChild<QPushButton *>("PicRightRotateBtn");
    ASSERT_NE(btn, nullptr);
    btn->released();
    QTest::qWait(100);

    // 水平翻转
    btn = dApp->topToolbar()->findChild<QPushButton *>("PicFlipHBtn");
    ASSERT_NE(btn, nullptr);
    btn->released();
    QTest::qWait(100);

    // 水平翻转
    btn = dApp->topToolbar()->findChild<QPushButton *>("PicFlipVBtn");
    ASSERT_NE(btn, nullptr);
    btn->released();
    QTest::qWait(100);

    // 自适应scence
    btn = dApp->topToolbar()->findChild<QPushButton *>("PicFlipAdjustmentBtn");
    ASSERT_NE(btn, nullptr);
    btn->released();
    QTest::qWait(100);
}

TEST(PictureItem, TestResizePictureItem)
{
    resizeItem();
}

TEST(PictureItem, TestSelectAllPictureItem)
{
    selectAllItem();
}

TEST(PictureItem, TestSavePictureItemToFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString PictureItemPath = QApplication::applicationDirPath() + "/test_picture.ddf";
    QFile file(PictureItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(PictureItemPath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);

    QFileInfo info(PictureItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(PictureItem, TestOpenPictureItemFromFile)
{
    CGraphicsView *view = getCurView();
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
    QTest::qWait(100);

    view = getCurView();
    ASSERT_NE(view, nullptr);
    int addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    ASSERT_EQ(true, addedCount == 2 ? true : false);
}

#endif
