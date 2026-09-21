// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QColor>

#include "colorlabel.h"

// =============================================================================
// ColorLabel::getColor —— HSV 转 RGB。
// 分支列表（按 hi = int(h / 60) % 6 的六分支 + 注释缺陷）：
//   H0  hi==0 -> (p, q, v)
//   H1  hi==1 -> (t, p, v)
//   H2  hi==2 -> (v, p, q)   [注：第三通道 int(255*q) 未做 min(...,255) 夹紧]
//   H3  hi==3 -> (v, t, p)
//   H4  hi==4 -> (q, v, p)
//   H5  else(hi==5) -> (p, v, t)
// *****************************************************************************
// !!! 已确认源码缺陷（不修复，仅特征化测试）!!!：
//   该实现并不是标准的 HSV->RGB 转换：输出色相约为 (h + 180)°（即 h=0 输出青色
//   (0,255,255) 而非标准红色 (255,0,0)），且 hi==2 分支的第三个通道
//   int(255*q) 未使用 std::min(...,255) 夹紧（与 hi==0/1/3/4/5 不一致）。
//   以下断言语义为"断言实际实现行为的特征化测试"（characterization test），
//   用例期望值由实现公式逐字复算得出，而非标准色相公式。
// *****************************************************************************
// =============================================================================

class TestColorLabel : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

static QColor calcGetColor(qreal h, qreal s, qreal v)
{
    return ColorLabel().getColor(h, s, v);
}

// 六个色相扇区（全饱和全亮度），特征化精确色值——覆盖 H0..H5 全部分支
TEST_F(TestColorLabel, fullSaturationAllSectors)
{
    // Arrange
    struct Case {
        qreal h;
        int r, g, b;
    };
    const Case cases[] = {
        {0, 0, 255, 255},     // H0
        {30, 0, 127, 255},    // H0
        {60, 0, 0, 255},      // H1
        {90, 127, 0, 255},    // H1
        {120, 255, 0, 255},   // H2
        {150, 255, 0, 127},   // H2
        {180, 255, 0, 0},     // H3
        {210, 255, 127, 0},   // H3
        {240, 255, 255, 0},   // H4
        {270, 127, 255, 0},   // H4
        {300, 0, 255, 0},     // H5
        {330, 0, 255, 127},   // H5
        {359, 0, 255, 250},   // H5（边界）
    };

    for (const Case &c : cases) {
        // Act
        QColor got = calcGetColor(c.h, 1.0, 1.0);
        // Assert
        EXPECT_EQ(got.red(), c.r) << "h=" << c.h;
        EXPECT_EQ(got.green(), c.g) << "h=" << c.h;
        EXPECT_EQ(got.blue(), c.b) << "h=" << c.h;
        // 同时校验 QColor 名（#RRGGBB）与分量一致
        EXPECT_EQ(got.name(QColor::HexRgb),
                  QColor(c.r, c.g, c.b).name(QColor::HexRgb)) << "h=" << c.h;
    }
}

// 饱和度=0：任何 h 都应是灰色（特征化：同值灰，s=0 时 p=q=t=v）
TEST_F(TestColorLabel, zeroSaturationGray)
{
    // Arrange
    const qreal hs[] = {0, 120, 240, 359};
    const qreal vs[] = {1.0, 0.5};
    // Act + Assert（覆盖 H0/H1/H2/H5 分支的 s=0 退化路径）
    for (qreal h : hs) {
        for (qreal v : vs) {
            QColor got = calcGetColor(h, 0, v);
            EXPECT_EQ(got.red(), got.green()) << "h=" << h << " v=" << v;
            EXPECT_EQ(got.green(), got.blue()) << "h=" << h << " v=" << v;
        }
    }
}

// 亮度=0：全黑
TEST_F(TestColorLabel, zeroValueBlack)
{
    // Arrange
    // Act
    QColor got = calcGetColor(60, 1.0, 0); // H1, v=0
    // Assert
    EXPECT_EQ(got.red(), 0);
    EXPECT_EQ(got.green(), 0);
    EXPECT_EQ(got.blue(), 0);
}

// 部分饱和度/亮度组合——特征化精确色值
TEST_F(TestColorLabel, partialSaturationCombos)
{
    // Arrange
    struct Case {
        qreal h, s, v;
        int r, g, b;
    };
    const Case cases[] = {
        {0, 0, 1, 255, 255, 255},       // H0（白色）
        {0, 0.5, 1, 127, 255, 255},     // H0（浅青）
        {120, 0, 0.5, 127, 127, 127},   // H2（灰）
        {60, 1, 0, 0, 0, 0},            // H1（黑）
        {0, 0.5, 0.5, 63, 127, 127},    // H0（中灰青）
        {40, 0.5, 0.25, 31, 42, 63},    // H0（暗青）
        {210, 0.5, 0.5, 127, 95, 63},   // H3
        {90, 1, 0.5, 63, 0, 127},       // H1
        {180, 0.75, 0.6, 153, 38, 38},  // H3
    };

    for (const Case &c : cases) {
        // Act
        QColor got = calcGetColor(c.h, c.s, c.v);
        // Assert
        EXPECT_EQ(got.red(), c.r) << "h=" << c.h << " s=" << c.s << " v=" << c.v;
        EXPECT_EQ(got.green(), c.g) << "h=" << c.h << " s=" << c.s << " v=" << c.v;
        EXPECT_EQ(got.blue(), c.b) << "h=" << c.h << " s=" << c.s << " v=" << c.v;
    }
}

// 相临扇区边界（h 恰为 60 的整数倍）——覆盖 hi 取整数除法向上翻进位
TEST_F(TestColorLabel, sectorBoundaries)
{
    // Arrange
    const qreal hs[] = {0, 60, 120, 180, 240, 300, 360};
    for (qreal h : hs) {
        // Act
        QColor got = calcGetColor(h, 1.0, 1.0);
        // Assert（边界不崩溃且各通道在 [0,255] 内）
        EXPECT_GE(got.red(), 0);
        EXPECT_LE(got.red(), 255);
        EXPECT_GE(got.green(), 0);
        EXPECT_LE(got.green(), 255);
        EXPECT_GE(got.blue(), 0);
        EXPECT_LE(got.blue(), 255);
    }
}
