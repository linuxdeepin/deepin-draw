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
#include "application.h"
#include "../getMainWindow.h"
#include <QDebug>

//TEST(MainWindow, MainWindow_openFiles)
//{
//    MainWindow *w = getMainWindow();

// test case true
//    QString path = QApplication::applicationDirPath() + "/test.png";
//    QPixmap pix(":/test.png");
//    ASSERT_EQ(true, pix.save(path, "PNG"));
//    QStringList filePaths;
//    filePaths.append(path);
//    bool openFlag = w->openFiles(QStringList(":/test.png"));
//    ASSERT_EQ(true, openFlag);

//    // test case false
//    filePaths.clear();
//    filePaths.append("asdasdajihhasidhaida");
//    openFlag = w->openFiles(filePaths);
//    ASSERT_EQ(false, openFlag);
//}

