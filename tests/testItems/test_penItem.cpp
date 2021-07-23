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
#undef protected
#undef private
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "cdrawscene.h"
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

#include <QDebug>
#include <DLineEdit>

#include "publicApi.h"

#if TEST_PEN_ITEM

TEST(PenItem, TestPenItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(PenItem, TestDrawPenItem)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    int oldCount = view->drawScene()->getBzItems().count();

    drawApp->setCurrentTool(pen);

    createItemByMouse(view);


    auto items   = view->drawScene()->getBzItems();

    int nowCount = items.count();

    ASSERT_EQ(nowCount - oldCount, 1);

    foreach (auto item, items) {
        ASSERT_EQ(item->type(), DyLayer);
    }
}


TEST(PenItem, TestPenItemProperty)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    JDynamicLayer *pen = dynamic_cast<JDynamicLayer *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(pen, nullptr);

    view->drawScene()->selectItem(pen);
    setPenWidth(pen, 1);
    ASSERT_EQ(pen->pen().width(), 1);

    // Start Type
//    DComboBox *typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("Line start style combox");
//    ASSERT_NE(typeCombox, nullptr);
//    for (int i = 0; i < typeCombox->count(); i++) {
//        ELineType defaultType = pen->getPenStartType();
//        typeCombox->setCurrentIndex(i);
//        QTest::qWait(300);
//        ASSERT_EQ(pen->getPenStartType(), i);

//        DTestEventList e;
//        e.addKeyPress(Qt::Key_Z, Qt::ControlModifier, 200);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pen->getPenStartType(), defaultType);
//        e.clear();
//        e.addKeyPress(Qt::Key_Y, Qt::ControlModifier, 200);
//        e.simulate(view->viewport());
//        ASSERT_EQ(pen->getPenStartType(), i);
//    }
}

TEST(PenItem, TestSavePenItemToFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString PenItemPath = QApplication::applicationDirPath() + "/test_pen.ddf";
    c->setFile(PenItemPath);
    c->save(true);

    QFileInfo info(PenItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(PenItem, TestOpenPenItemFromFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString PenItemPath = QApplication::applicationDirPath() + "/test_pen.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(PenItemPath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    (void)QTest::qWaitFor([ = ]() {return (view != getCurView() && getCurView()->drawScene()->getBzItems().count());});

    view = getCurView();
    ASSERT_NE(view, nullptr);

    int addedCount = view->drawScene()->getBzItems().count();
    ASSERT_EQ(addedCount, 1);
    view->page()->close(true);
}

#endif
