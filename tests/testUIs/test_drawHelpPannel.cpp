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

#include "../testItems/publicApi.h"

#include "application.h"
#include "mainwindow.h"
#include "shortcut.h"

#include "QTestEventList"

#if TEST_DRAWHELP_ITEM

TEST(DrawHelpPannel, TestDrawHelpPannelCreateView)
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

    DTestEventList e;
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

TEST(DrawHelpPannel, ShowShortCutKey)
{
    QAction *ac = getMainWindow()->findChild<QAction *>("shortCutManPannel");
    ASSERT_NE(ac, nullptr);
    ac->trigger();
    QTest::qWait(300);
}

#endif
