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

#if TEST_CUT_ITEM

TEST(CutItem, TestCutItemCreateView)
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

TEST(CutItem, TestCutItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // [2] 裁剪图元需要单独进行处理才可以
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);
    int addedCount = view->drawScene()->getBzItems().count();

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("CropTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();
    QTest::qWait(100);

    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), CutType);

    // 获取确认裁剪按钮
    DPushButton  *cutDoneBtn = dApp->topToolbar()->findChild<DPushButton *>("CutDoneBtn");
    ASSERT_NE(cutDoneBtn, nullptr);

    // [2.1] 手动设置裁剪大小(自由模式)
    DLineEdit *widthLineEdit = dApp->topToolbar()->findChild<DLineEdit *>("CutWidthLineEdit");
    ASSERT_NE(widthLineEdit, nullptr);
    widthLineEdit->setText("1000");
    widthLineEdit->editingFinished();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());

    tool->clicked();
    QTest::qWait(100);

    DLineEdit *heightLineEdit = dApp->topToolbar()->findChild<DLineEdit *>("CutHeightLineEdit");
    ASSERT_NE(heightLineEdit, nullptr);
    heightLineEdit->setText("500");
    heightLineEdit->editingFinished();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.2] 1:1 模式
    tool->clicked();
    QTest::qWait(100);
    DPushButton *btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate1_1Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.3] 2:3 模式
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate2_3Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(int(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.4] 8:5 模式
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate8_5Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().height()), heightLineEdit->text().toInt());

    // [2.5] 16:9 模式
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate16_9Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().height()), heightLineEdit->text().toInt());

    // [2.6] 原始恢复
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRateOriginalBtn");
    ASSERT_NE(btn, nullptr);
    view->drawScene()->setSceneRect(QRectF(0, 0, 400, 400));
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());
}

TEST(CutItem, TestResizeCutItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("CropTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();
    QTest::qWait(100);

    EDrawToolMode model = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
    CCutTool *pTool = dynamic_cast<CCutTool *>(CDrawToolManagerSigleton::GetInstance()->getDrawTool(model));
    ASSERT_NE(pTool, nullptr);

    QVector<CSizeHandleRect *> handles = pTool->getCutItem(view->drawScene())->nodes();
    for (int i = 0; i < handles.size(); ++i) {
        CSizeHandleRect *pNode = handles[i];
        QPoint posInView = view->mapFromScene(pNode->mapToScene(pNode->boundingRect().center()));
        QTestEventList e;
        e.addMouseMove(posInView, 100);
        e.addMousePress(Qt::LeftButton, Qt::ShiftModifier, posInView, 100);
        e.addMouseMove(posInView + QPoint(50, 50), 100);
        e.addMouseRelease(Qt::LeftButton, Qt::ShiftModifier, posInView + QPoint(50, 50), 100);
        e.simulate(view->viewport());
    }
}

TEST(CutItem, TestChangeView)
{

}

TEST(CutItem, TestSaveCutItemToFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString CutItemPath = QApplication::applicationDirPath() + "/test_cut.ddf";
    QFile file(CutItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(CutItemPath);
    QTestEventList e;
    e.addKeyPress(Qt::Key_S, Qt::ControlModifier, 100);

    QTimer::singleShot(1000, c, [&]() {
        DDialog  *dialog = c->findChild<DDialog *>("CutDialog");
        ASSERT_NE(dialog, nullptr);
        dialog->buttonClicked(1, "");
    });

    e.simulate(view->viewport());

    QFileInfo info(CutItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(CutItem, TestOpenCutItemFromFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString CutItemPath = QApplication::applicationDirPath() + "/test_cut.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(CutItemPath));
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
