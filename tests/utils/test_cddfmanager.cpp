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
#include "cddfmanager.h"
#include "../getMainWindow.h"
#include "ccentralwidget.h"
#include "frame/cgraphicsview.h"
#include "frame/cviewmanagement.h"
#include "cdrawparamsigleton.h"
#include "QApplication"
#include <QTest>

//TEST(cddfmanager, cddfmanager)
//{
//    MainWindow *w = getMainWindow();

//    CCentralwidget *c = w->getCCentralwidget();
//    CGraphicsView *view = c->getGraphicsView();

//    QFile cacheddf(":/test.ddf");
//    qDebug() << "++++++++++++++:" << cacheddf.open(QIODevice::ReadWrite);
//    QString ddfpath = QApplication::applicationDirPath() + "/test_save.ddf";
//    QByteArray data = cacheddf.readAll();
//    qDebug() << "++++++++++++++ size:" << data.size();
//    cacheddf.setFileName(ddfpath);
//    qDebug() << "++++++++++++++ write:" << cacheddf.write(data);
//    cacheddf.close();

//    view->signalLoadDragOrPasteFile(ddfpath);

//    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath(ddfpath);
//    c->slotSaveToDDF(false);
//    c->slotSaveToDDF(true);

//    int i = 0;
//    while (i++ < 10) {
//        QTest::qWait(200);
//    }
//}

