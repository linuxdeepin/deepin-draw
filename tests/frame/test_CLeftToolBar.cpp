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
#include "clefttoolbar.h"
#include "ccentralwidget.h"
#include "../getMainWindow.h"
#include "global.h"

TEST(CLeftToolBar, CLeftToolBar_switchTheme)
{
    MainWindow *w = getMainWindow();
    w->hide();

    CLeftToolBar *c = w->getCCentralwidget()->getLeftToolBar();

    c->setCurrentDrawTool(EDrawToolMode::noselected);
    c->setCurrentDrawTool(EDrawToolMode::selection);
    c->setCurrentDrawTool(EDrawToolMode::importPicture);
    c->setCurrentDrawTool(EDrawToolMode::rectangle);
    c->setCurrentDrawTool(EDrawToolMode::ellipse);
    c->setCurrentDrawTool(EDrawToolMode::triangle);
    c->setCurrentDrawTool(EDrawToolMode::polygonalStar);
    c->setCurrentDrawTool(EDrawToolMode::polygon);
    c->setCurrentDrawTool(EDrawToolMode::line);
    c->setCurrentDrawTool(EDrawToolMode::pen);
    c->setCurrentDrawTool(EDrawToolMode::text);
    c->setCurrentDrawTool(EDrawToolMode::blur);
    c->setCurrentDrawTool(EDrawToolMode::cut);
}

