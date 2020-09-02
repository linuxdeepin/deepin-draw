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
#include "ccheckbutton.h"

//TEST(setChecked, setChecked_Active)
//{
//    QMap<int, QMap<CCheckButton::EButtonSattus, QString>> data;
//    QMap<CCheckButton::EButtonSattus, QString> picInfo;

//    picInfo.insert(CCheckButton::Active, "pic_ActivePath");
//    data.insert(0, picInfo);

//    CCheckButton c(data, QSize(40, 40));
//    bool flag = true;
//    c.setChecked(flag);
//    ASSERT_EQ(flag, c.isChecked());
//    ASSERT_EQ(CCheckButton::EButtonSattus::Active, c.getCurrentStatus());
//}

//TEST(setChecked, setChecked_Normal)
//{
//    QMap<int, QMap<CCheckButton::EButtonSattus, QString>> data;
//    QMap<CCheckButton::EButtonSattus, QString> picInfo;

//    picInfo.insert(CCheckButton::Normal, "pic_NormalPath");
//    data.insert(0, picInfo);

//    CCheckButton c(data, QSize(40, 40));
//    bool flag = false;
//    c.setChecked(flag);
//    ASSERT_EQ(flag, c.isChecked());
//    ASSERT_EQ(CCheckButton::EButtonSattus::Normal, c.getCurrentStatus());
//}

//TEST(setCurrentTheme, CCheckButton_setCurrentTheme)
//{
//    QMap<int, QMap<CCheckButton::EButtonSattus, QString>> data;
//    QMap<CCheckButton::EButtonSattus, QString> picInfo;

//    picInfo.insert(CCheckButton::Press, "pic_pressPath");
//    data.insert(0, picInfo);

//    CCheckButton c(data, QSize(40, 40));
//    int theme = 1;
//    c.setCurrentTheme(theme);
//    ASSERT_EQ(theme, c.getCurrentTheme());
//}
