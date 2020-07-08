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
#include "bordercolorbutton.h"

TEST(setColor, setColor)
{
    BorderColorButton btn;
    QColor color("red");
    btn.setColor(color);
    ASSERT_EQ(color, btn.getColor());
}

TEST(setColorIndex, setColorIndex)
{
    BorderColorButton btn;
    QColor color = QColor("#ff0c0c");
    btn.setColorIndex(1);
    ASSERT_EQ(color, btn.getColorByIndex());
}

TEST(setIsMultColorSame, setIsMultColorSame)
{
    BorderColorButton btn;
    btn.setIsMultColorSame(true);
    ASSERT_EQ(true, btn.getIsMultColorSame());
}

TEST(setButtonText, setButtonText)
{
    BorderColorButton btn;
    QString str = "test string";
    btn.setButtonText(str);
    ASSERT_EQ(str, btn.getButtonText());
}
