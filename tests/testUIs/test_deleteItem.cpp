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
#include <QTimer>
#include <DFileDialog>
#include <QDialogButtonBox>

#include "../testItems/publicApi.h"

#ifdef TEST_DELETE_ITEM

TEST(DeleteItem, TestDeleteItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(DeleteItem, TestDeleteItem)
{
    // Draw Triangle
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("Triangle tool button");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    int addedCount = view->drawScene()->getBzItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), TriangleType);


    QTestEventList e;
    e.addKeyClick(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Delete, Qt::NoModifier, 100);
    e.simulate(getCurView());

    addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    ASSERT_EQ(true, addedCount == 0 ? true : false);
}

TEST(DeleteItem, TestDeleteItemSaveDDF)
{
//    CGraphicsView *view = getCurView();
//    ASSERT_NE(view, nullptr);
//    view->setSaveDialogMoreOption(QFileDialog::DontUseNativeDialog);

//    QTestEventList e;
//    e.addKeyPress(Qt::Key_S, Qt::ControlModifier, 100);

//    QTimer::singleShot(2000, [&]() {
//        // get popup filedialog
//        DFileDialog *saveDialog  =  view->findChild<DFileDialog *>("DDFSaveDialog");
//        ASSERT_NE(saveDialog, nullptr);
//        saveDialog->setFileMode(QFileDialog::AnyFile);
//        //saveDialog->setOptions(QFileDialog::DontUseNativeDialog);

//        QDialog *dia = static_cast<QDialog *>(saveDialog);
//        ASSERT_NE(dia, nullptr);
//        QTest::qWait(100);

//        // delete exist file
//        QString path = QCoreApplication::applicationDirPath() + "/" + view->getDrawParam()->viewName() + ".ddf";
//        QFileInfo info(path);
//        if (info.exists()) {
//            QFile::remove(path);
//        }

////        QTimer::singleShot(2000, [&]() {
////            saveDialog->setDirectory(QCoreApplication::applicationDirPath());
////            QTest::qWait(300);

////            // 设置默认的文件名
////            saveDialog->selectFile(path);
////            dia->done(QDialog::Accepted);
////            QTest::qWait(1000);

////            ASSERT_EQ(info.exists(), true);
////        });
//        //saveDialog->selectFile(path);
//        //QTest::qWait(100);
//        QMetaObject::invokeMethod(view, [ = ]() {
//            QDialog *dia = qobject_cast<QDialog *>(dApp->activeModalWidget());
//            if (dia != nullptr) {
//                dia->accept();
//            }
//        }, Qt::QueuedConnection);
//        dia->accept();
//        QTest::qWait(100);

//        view->doSaveDDF();
//    });
//    e.simulate(view->viewport());
}

#endif
