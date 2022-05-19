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

#if TEST_HELPPANNEL_ITEM

TEST(DrawZHelpPannel, TestZDrawHelpPannelCreateView)
{
    createNewViewByShortcutKey();
}

TEST(DrawZHelpPannel, ZShowShortCutKey)
{
    QAction *ac = getMainWindow()->findChild<QAction *>("shortCutManPannel");
    ASSERT_NE(ac, nullptr);
    ac->trigger();
    QTest::qWait(300);
}
#endif
