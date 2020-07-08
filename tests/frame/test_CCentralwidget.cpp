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
#include "mainwindow.h"

TEST(CCentralwidget, CCentralwidget_switchTheme)
{
    MainWindow *w = new MainWindow;
    w->hide();

    CCentralwidget *c = w->getCCentralwidget();

    int theme = 1;
    c->switchTheme(theme);
    ASSERT_EQ(theme, c->getSystemTheme());

    theme = 2;
    c->switchTheme(theme);
    ASSERT_EQ(theme, c->getSystemTheme());
}

