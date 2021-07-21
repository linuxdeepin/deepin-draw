/*
 *  Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
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
#include "cgraphicscutitem.h"

#include <QDebug>
#include <DLineEdit>

#include "../testItems/publicApi.h"

#if TEST_SCANLE_ITEM

TEST(ScanleScence, TestScanleScenceCreateView)
{
    createNewViewByShortcutKey();
}

TEST(ScanleScence, TestScanleScence)
{
    MainWindow *w = getMainWindow();
    ASSERT_NE(w, nullptr);
    TopTilte *toptoolbar = w->topTitle();
    ASSERT_NE(toptoolbar, nullptr);
    /**
     * @brief 缩放view以及缩放菜单测试
     */
    DIconButton *fbtn;
    fbtn = toptoolbar->findChild<DIconButton *>("Zoom reduce button");
    ASSERT_NE(fbtn, nullptr);
    fbtn->clicked();
    fbtn->clicked();

    fbtn = toptoolbar->findChild<DIconButton *>("Zoom increase button");
    ASSERT_NE(fbtn, nullptr);
    fbtn->clicked();
    fbtn->clicked();

    DZoomMenuComboBox *box = toptoolbar->findChild<DZoomMenuComboBox *>("zoomMenuComboBox");
    ASSERT_NE(box, nullptr);
    box->setCurrentText("75%");

    DTestEventList e;
    e.addKeyClick(Qt::Key_Minus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Minus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Plus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Equal, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Equal, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Plus, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Plus, Qt::ControlModifier, 100);
    e.simulate(getCurView());
}

TEST(ScanleScence, TestSaveScanleScenceToFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString ScanleScencePath = QApplication::applicationDirPath() + "/test_scanle.ddf";
    c->setFile(ScanleScencePath);
    c->save(true);
    c->context()->setDirty(false);
    //QFileInfo info(ScanleScencePath);
    //ASSERT_TRUE(info.exists());
}

#if 0
TEST(ScanleScence, TestOpenScanleScenceFromFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString ScanleScencePath = QApplication::applicationDirPath() + "/test_scanle.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(ScanleScencePath));
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
    ASSERT_EQ(true, addedCount == 0 ? true : false);
}
#endif

#endif
