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

#include "ctexttool.h"
#include "mainwindow.h"
#include "ccentralwidget.h"

#include <QGraphicsSceneMouseEvent>

TEST(CTextTool, CTextTool)
{
    MainWindow *w = new MainWindow;
    w->hide();

    CCentralwidget *c = w->getCCentralwidget();
    CDrawScene *scence = c->getDrawScene();

    CTextTool *tool = new CTextTool();
    QGraphicsSceneMouseEvent event;
    event.setPos(QPointF(0, 0));
    event.setScenePos(QPointF(0, 0));
    event.setScreenPos(QPoint(0, 0));
    event.setButtonDownPos(Qt::MouseButton::LeftButton, QPointF(0, 0));
    event.setButtonDownScenePos(Qt::MouseButton::LeftButton, QPointF(0, 0));
    event.setButtonDownScreenPos(Qt::MouseButton::LeftButton, QPoint(0, 0));
    event.setLastPos(QPointF(0, 0));
    event.setLastScenePos(QPointF(0, 0));
    event.setLastScreenPos(QPoint(0, 0));
    event.setButtons(Qt::MouseButton::LeftButton);
    event.setButton(Qt::MouseButton::LeftButton);
    event.setModifiers(Qt::NoModifier);
    event.setSource(Qt::MouseEventSynthesizedByApplication);
    event.setFlags(Qt::MouseEventCreatedDoubleClick);
    tool->mousePressEvent(&event, scence);

    event.setPos(QPointF(400, 400));
    event.setScenePos(QPointF(400, 400));
    event.setScreenPos(QPoint(400, 400));
    event.setButtonDownPos(Qt::MouseButton::LeftButton, QPointF(400, 400));
    event.setButtonDownScenePos(Qt::MouseButton::LeftButton, QPointF(400, 400));
    event.setButtonDownScreenPos(Qt::MouseButton::LeftButton, QPoint(400, 400));
    tool->mouseMoveEvent(&event, scence);

    tool->mouseReleaseEvent(&event, scence);
}
