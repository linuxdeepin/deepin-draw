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
#include "ccutwidget.h"
#include "cgraphicsview.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "cgraphicsitemselectedmgr.h"
#include "application.h"
#include "cviewmanagement.h"
#include "cdrawtoolmanagersigleton.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"
#include "ccutdialog.h"

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

#if TEST_CUT_ITEM

TEST(CutItem, TestCutItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(CutItem, TestCutItemProperty)
{
    PageView *view = getCurView();

    QRectF originRect = view->page()->pageRect();

    ASSERT_NE(view, nullptr);

    // [2] 裁剪图元需要单独进行处理才可以
    int addedCount = view->drawScene()->getBzItems().count();

    drawApp->setCurrentTool(cut);
    QTest::qWait(300);

    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), CutType);

    // 获取确认裁剪按钮
    DPushButton  *cutDoneBtn = drawApp->topMainWindow()->findChild<DPushButton *>("Cut done pushbutton");

    ASSERT_NE(cutDoneBtn, nullptr);

    // [2.1] 手动设置裁剪大小(自由模式)
    DLineEdit *widthLineEdit = drawApp->topMainWindow()->findChild<DLineEdit *>("CutWidthLineEdit");
    ASSERT_NE(widthLineEdit, nullptr);
    widthLineEdit->setText("1000");
    emit widthLineEdit->editingFinished();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());

    drawApp->setCurrentTool(cut);
    QTest::qWait(100);

    auto topToolBar = drawApp->topToolbar();
    DLineEdit *heightLineEdit = topToolBar->findChild<DLineEdit *>("CutHeightLineEdit");
    ASSERT_NE(heightLineEdit, nullptr);
    heightLineEdit->setText("500");
    emit heightLineEdit->editingFinished();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.2] 1:1 模式
    drawApp->setCurrentTool(cut);
    QTest::qWait(100);
    DPushButton *btn = topToolBar->findChild<DPushButton *>("Cut ratio(1:1) pushbutton");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.3] 2:3 模式
    drawApp->setCurrentTool(cut);
    QTest::qWait(100);
    btn = topToolBar->findChild<DPushButton *>("Cut ratio(2:3) pushbutton");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(int(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.4] 8:5 模式
    drawApp->setCurrentTool(cut);
    QTest::qWait(100);
    btn = topToolBar->findChild<DPushButton *>("Cut ratio(8:5) pushbutton");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().height()), heightLineEdit->text().toInt());

    // [2.5] 16:9 模式
    drawApp->setCurrentTool(cut);
    QTest::qWait(100);
    btn = topToolBar->findChild<DPushButton *>("Cut ratio(16:9) pushbutton");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().height()), heightLineEdit->text().toInt());

    // [2.6] 原始恢复
    drawApp->setCurrentTool(cut);
    QTest::qWait(100);
    btn = topToolBar->findChild<DPushButton *>("Cut ratio(Original) pushbutton");
    ASSERT_NE(btn, nullptr);
    view->drawScene()->setSceneRect(QRectF(0, 0, 400, 400));
    btn->toggle();
    emit cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), originRect.width());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), originRect.height());
}

TEST(CutItem, TestResizeCutItem)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    drawApp->setCurrentTool(cut);
    QTest::qWait(100);

    CCutTool *pTool = qobject_cast<CCutTool *>(drawApp->drawBoard()->currentTool_p());
    ASSERT_NE(pTool, nullptr);

    auto cutAttriWidget  = drawApp->topToolbar()->findChild<CCutWidget *>("scene cut attribution widget");
    auto fDoOperate = [ = ]() {
        QVector<CSizeHandleRect *> handles = pTool->getCutItem(view->drawScene())->nodes();
        for (int i = 0; i < handles.size(); ++i) {
            CSizeHandleRect *pNode = handles[i];
            QPoint startPosInView    = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
            QPoint desPosInView      =  startPosInView + QPoint(50, 50);
            DTestEventList e;
            e.addMouseMove(startPosInView, 100);
            e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, startPosInView, 100);
            e.addMouseMove(desPosInView, 100);
            e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, desPosInView, 100);

            e.simulate(view->viewport());

            e.clear();

            e.addMouseMove(desPosInView, 100);
            e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, desPosInView, 100);
            e.addMouseMove(startPosInView, 100);
            e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, startPosInView, 100);

            e.simulate(view->viewport());
        }
    };

    auto cutItem = pTool->getCutItem(view->drawScene());
    auto size    = view->drawScene()->sceneRect().size().toSize();
    for (int i = cut_1_1; i < cut_count; ++i) {
        cutAttriWidget->setCutType(ECutType(i));
        QTest::qWait(100);
        fDoOperate();
        QTest::qWait(100);
        ASSERT_EQ(cutItem->rect().size().toSize(), cutAttriWidget->cutSize());
    }
    cutAttriWidget->setCutType(cut_free);
    cutAttriWidget->setCutSize(QSize(10, 10));
    ASSERT_EQ(cutItem->rect().size().toSize(), QSize(10, 10));
}

TEST(CutItem, TestSaveCutItemToFile)
{
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString CutItemPath = QApplication::applicationDirPath() + "/test_cut.ddf";
    c->setFile(CutItemPath);
    setQuitDialogResult(CCutDialog::Save);
    c->save();

    setQuitDialogResult(2);
    view->page()->close();

    setQuitDialogResult(-1);
    QFileInfo info(CutItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(CutItem, TestOpenCutItemFromFile)
{
    PageView *view = getCurView();

    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString CutItemPath = QApplication::applicationDirPath() + "/test_cut.ddf";

    view->page()->borad()->load(CutItemPath);

    qMyWaitFor([ = ]() {return (view != getCurView());});

    view = getCurView();

    ASSERT_NE(view, nullptr);

    view->page()->close(true);
}

#endif
