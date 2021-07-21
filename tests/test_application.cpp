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
#include <QTimer>

#include "dialog.h"
#include "publicApi.h"
#include "application.h"
#include "mainwindow.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"

TEST(isFileNameLegal, ins1)
{
    ASSERT_EQ(false, drawApp->isFileNameLegal("/sadhgasdjhasdg/sahdkjahskdjhaskd"));
}

TEST(isFileNameLegal, ins2)
{
    ASSERT_EQ(false, drawApp->isFileNameLegal("./"));
}

TEST(isFileNameLegal, ins3)
{
    ASSERT_EQ(true, drawApp->isFileNameLegal("./myfile.txt"));
}

TEST(isFileNameLegal, ins4)
{
    ASSERT_EQ(false, drawApp->isFileNameLegal(""));
}

//TEST(isFileNameLegal, ins5)
//{
//    QTimer::singleShot(1000, drawApp->topMainWindowWidget(), [ = ]() {
//        auto dial = qobject_cast<Dialog *>(qApp->activeModalWidget());
//        if (dial != nullptr) {
//            dial->done(1);
//        }
//    });
//    getMainWindow()->close();
//}
