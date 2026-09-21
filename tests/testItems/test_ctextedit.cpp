// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>

#include "ctextedit.h"

// =============================================================================
// CTextEdit::toWeight —— 静态方法，将样式名称转换为字重数值。
// 分支列表（if/else 链，链尾的隐式默认分支）：
//   B1  "Thin"            -> 0
//   B2  "ExtraLight"      -> 12
//   B3  "Light"           -> 25
//   B4  "Normal"/"Regular"-> 50
//   B5  "Medium"          -> 57
//   B6  "DemiBold"        -> 63
//   B7  "Bold"            -> 75
//   B8  "ExtraBold"       -> 81
//   B9  "Black"           -> 87
//   B10 未命名字符串（含空串、大小写不一致）-> 0（隐式默认分支）
// CTextEdit::toStyle —— 静态方法，将字重数值转换为样式名称。
//   S1  0    -> "Thin"
//   S2  12   -> "ExtraLight"
//   S3  25   -> "Light"
//   S4  50   -> "Regular"
//   S5  57   -> "Medium"
//   S6  63   -> "DemiBold"
//   S7  75   -> "Bold"
//   S8  81   -> "ExtraBold"
//   S9  87   -> "Black"
//   S10 其他数值 -> ""（默认分支）
// =============================================================================

class TestCTextEdit : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

// ---------------- CTextEdit::toWeight ----------------

TEST_F(TestCTextEdit, toWeightNamedStyles)
{
    // Arrange
    struct Case {
        const char *name;
        int expected;
    };
    const Case cases[] = {
        {"Thin", 0},
        {"ExtraLight", 12},
        {"Light", 25},
        {"Normal", 50},
        {"Regular", 50},
        {"Medium", 57},
        {"DemiBold", 63},
        {"Bold", 75},
        {"ExtraBold", 81},
        {"Black", 87},
    };

    for (const Case &c : cases) {
        // Act
        int w = CTextEdit::toWeight(QString::fromLatin1(c.name));
        // Assert
        EXPECT_EQ(w, c.expected) << "style name: " << c.name;
    }
}

TEST_F(TestCTextEdit, toWeightUnknownNames)
{
    // Arrange
    const char *unknowns[] = {"", "thin", "BOLD", "NotAStyle", "Custom"};

    for (const char *u : unknowns) {
        // Act
        int w = CTextEdit::toWeight(QString::fromLatin1(u));
        // Assert
        EXPECT_EQ(w, 0) << "input: " << u;
    }
}

TEST_F(TestCTextEdit, toWeightEmptyAndChinese)
{
    // Arrange
    const QString chinese[] = {QString::fromUtf8("常规"), QString::fromUtf8("加粗"), QString::fromUtf8("细体")};
    // Act
    int wEmpty = CTextEdit::toWeight(QString());
    // Assert
    EXPECT_EQ(wEmpty, 0);

    for (const QString &s : chinese) {
        // Act
        int w = CTextEdit::toWeight(s);
        // Assert
        EXPECT_EQ(w, 0) << "input: " << s.toStdString();
    }
}

// ---------------- CTextEdit::toStyle ----------------

TEST_F(TestCTextEdit, toStyleNamedWeights)
{
    // Arrange
    struct Case {
        int weight;
        const char *expected;
    };
    const Case cases[] = {
        {0, "Thin"},
        {12, "ExtraLight"},
        {25, "Light"},
        {50, "Regular"},
        {57, "Medium"},
        {63, "DemiBold"},
        {75, "Bold"},
        {81, "ExtraBold"},
        {87, "Black"},
    };

    for (const Case &c : cases) {
        // Act
        QString s = CTextEdit::toStyle(c.weight);
        // Assert
        EXPECT_STREQ(s.toStdString().c_str(), c.expected) << "weight: " << c.weight;
    }
}

TEST_F(TestCTextEdit, toStyleDefaultBranch)
{
    // Arrange
    const int unknowns[] = {1, 100, -5, 7};

    for (int u : unknowns) {
        // Act
        QString s = CTextEdit::toStyle(u);
        // Assert
        EXPECT_TRUE(s.isEmpty()) << "weight: " << u;
    }
}

// 对规范名（不含别名 Normal，其会被归一化为 Regular）互为逆映射：toStyle(toWeight(name)) == name
TEST_F(TestCTextEdit, roundTripMapping)
{
    // Arrange
    const char *names[] = {"Thin", "ExtraLight", "Light", "Regular",
                           "Medium", "DemiBold", "Bold", "ExtraBold", "Black"};

    for (const char *n : names) {
        // Act
        QString s = CTextEdit::toStyle(CTextEdit::toWeight(QString::fromLatin1(n)));
        // Assert
        EXPECT_STREQ(s.toStdString().c_str(), n);
    }
}
