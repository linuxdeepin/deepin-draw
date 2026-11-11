// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsTextItem zero-coverage methods (V-8216)
 *
 * Target methods:
 *   - currentCharFormat()
 *   - setCurrentFormat(const QTextCharFormat&, bool)
 *   - mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)
 *
 * Source: src/drawshape/drawItems/bzItems/cgraphicstextitem.cpp
 */

#include <gtest/gtest.h>

#include <QTextCharFormat>
#include <QFont>
#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QMouseEvent>

#define protected public
#define private public
#include "drawshape/drawItems/bzItems/cgraphicstextitem.h"
#include "widgets/ctextedit.h"
#undef protected
#undef private

// =========================================================================
// currentCharFormat
// =========================================================================

TEST(CGraphicsTextItemExt2, CurrentCharFormat_WithTextEdit_ReturnsFormat)
{
    CGraphicsTextItem item("Hello draw");
    // m_pTextEdit is auto-created in constructor
    ASSERT_NE(item.m_pTextEdit, nullptr);

    QTextCharFormat fmt = item.currentCharFormat();
    // Default format should be valid
    EXPECT_TRUE(fmt.isValid());
}

TEST(CGraphicsTextItemExt2, CurrentCharFormat_AfterSetBold_ReturnsBold)
{
    CGraphicsTextItem item("Hello draw");
    ASSERT_NE(item.m_pTextEdit, nullptr);

    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Bold);
    item.setCurrentFormat(fmt, true);

    QTextCharFormat result = item.currentCharFormat();
    EXPECT_EQ(result.fontWeight(), QFont::Bold);
}

TEST(CGraphicsTextItemExt2, CurrentCharFormat_AfterSetColor_ReturnsColor)
{
    CGraphicsTextItem item("Hello draw");
    ASSERT_NE(item.m_pTextEdit, nullptr);

    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor(255, 0, 0)));
    item.setCurrentFormat(fmt, false);

    QTextCharFormat result = item.currentCharFormat();
    EXPECT_EQ(result.foreground().color(), QColor(255, 0, 0));
}

// =========================================================================
// setCurrentFormat
// =========================================================================

TEST(CGraphicsTextItemExt2, SetCurrentFormat_MergeTrue_NoCrash)
{
    CGraphicsTextItem item("Hello draw");
    ASSERT_NE(item.m_pTextEdit, nullptr);

    QTextCharFormat fmt;
    fmt.setFontPointSize(16.0);
    item.setCurrentFormat(fmt, true);

    // Read back to verify the format was actually applied
    EXPECT_DOUBLE_EQ(item.currentCharFormat().fontPointSize(), 16.0);
}

TEST(CGraphicsTextItemExt2, SetCurrentFormat_MergeFalse_NoCrash)
{
    CGraphicsTextItem item("Hello draw");
    ASSERT_NE(item.m_pTextEdit, nullptr);

    QTextCharFormat fmt;
    fmt.setFontPointSize(20.0);
    fmt.setFontItalic(true);
    item.setCurrentFormat(fmt, false);

    // Read back to verify the format was actually applied
    EXPECT_DOUBLE_EQ(item.currentCharFormat().fontPointSize(), 20.0);
    EXPECT_TRUE(item.currentCharFormat().fontItalic());
}

// =========================================================================
// mouseDoubleClickEvent
// =========================================================================

TEST(CGraphicsTextItemExt2, MouseDoubleClickEvent_NoCrash)
{
    CGraphicsTextItem item("Hello draw");

    QGraphicsSceneMouseEvent event;
    event.setPos(QPointF(10, 10));
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);

    item.mouseDoubleClickEvent(&event);
    // mouseDoubleClickEvent is a no-op (Q_UNUSED), should not crash
    SUCCEED();
}
