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

#include "cpicturetool.h"
#include "mainwindow.h"
#include "ccentralwidget.h"

#include <QGraphicsSceneMouseEvent>

TEST(CPictureTool, CPictureTool)
{
    MainWindow *w = new MainWindow;
    w->hide();

    CCentralwidget *c = w->getCCentralwidget();
    CDrawScene *scence = c->getDrawScene();

    CPictureTool *tool = new CPictureTool();

    QPixmap pix(":/test.png");

    tool->addImages(pix, 0, scence, nullptr, QByteArray(":/test.png"), false);

    tool->addImages(pix, 0, scence, nullptr, QByteArray(":/test.png"), true);

    tool->addImages(pix, 31, scence, nullptr, QByteArray(":/test.png"), true);
}
