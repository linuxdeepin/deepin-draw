// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextTool::eventFilter and QComboxMenuDelegate::getStyleOption
 *
 * Target methods (from issue V-5046):
 *   - CTextTool::eventFilter              (level=high, complexity:15, lines:76)
 *   - QComboxMenuDelegate::getStyleOption (level=high)
 *
 * Source: src/drawshape/drawTools/ctexttool.cpp
 *
 * Minimum case counts:
 *   eventFilter:   high → 3 cases
 *   getStyleOption: high → 3 cases
 *
 * | Method         | level | min | actual |
 * |----------------|-------|-----|--------|
 * | eventFilter    | high  | 3   | 3      |
 * | getStyleOption | high  | 3   | 3      |
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

// Include Qt headers BEFORE #define private to avoid breaking Qt internals.
#include <QKeyEvent>
#include <QEvent>
#include <QComboBox>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>
#include <QStyleOptionMenuItem>
#include <QApplication>

#define protected public
#define private public
#include "drawshape/drawTools/ctexttool.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CTextTool::eventFilter tests
// =========================================================================

TEST(CTextToolEventFilter, FontSizeShowEvent_NoCrash)
{
    // Arrange
    createNewViewByShortcutKey();

    drawApp->setCurrentTool(text);
    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);
    ASSERT_NE(textTool->m_fontSize, nullptr);

    // Get the view object that the eventFilter checks against
    QObject *fontSizeView = textTool->m_fontSize->view();
    ASSERT_NE(fontSizeView, nullptr);

    QEvent showEvent(QEvent::Show);

    // Act - should not crash
    textTool->eventFilter(fontSizeView, &showEvent);

    // Assert - no crash
    SUCCEED();
}

TEST(CTextToolEventFilter, FontSizeKeyPress_Down_NoCrash)
{
    // Arrange
    createNewViewByShortcutKey();

    drawApp->setCurrentTool(text);
    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);
    ASSERT_NE(textTool->m_fontSize, nullptr);

    QObject *fontSizeView = textTool->m_fontSize->view();
    ASSERT_NE(fontSizeView, nullptr);

    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);

    // Act - should not crash even if the event is not handled
    textTool->eventFilter(fontSizeView, &keyEvent);

    // Assert - no crash
    SUCCEED();
}

TEST(CTextToolEventFilter, FontComBoxHideEvent_ReturnsTrue)
{
    // Arrange
    createNewViewByShortcutKey();

    drawApp->setCurrentTool(text);
    CTextTool *textTool = dynamic_cast<CTextTool *>(
        CDrawToolFactory::tool(text));
    ASSERT_NE(textTool, nullptr);
    ASSERT_NE(textTool->m_fontComBox, nullptr);

    QEvent hideEvent(QEvent::Hide);

    // Act
    bool result = textTool->eventFilter(textTool->m_fontComBox->view(), &hideEvent);

    // Assert - eventFilter returns true for Hide event on fontComBox view
    EXPECT_TRUE(result);
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption tests
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, ValidIndex_ReturnsCorrectCheckType)
{
    // Arrange
    QComboBox combo;
    combo.addItem("Item1");
    combo.addItem("Item2");

    QComboxMenuDelegate delegate(nullptr, &combo);
    QModelIndex index = combo.model()->index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);
    option.state = QStyle::State_Enabled | QStyle::State_Selected;

    // Act
    QStyleOptionMenuItem result = delegate.getStyleOption(option, index);

    // Assert
    EXPECT_EQ(result.checkType, QStyleOptionMenuItem::NonExclusive);
    EXPECT_EQ(result.menuItemType, QStyleOptionMenuItem::Normal);
}

TEST(QComboxMenuDelegateGetStyleOption, SelectedIndex_SetsSelectedState)
{
    // Arrange
    QComboBox combo;
    combo.addItem("Item1");
    combo.addItem("Item2");
    combo.setCurrentIndex(0);

    QComboxMenuDelegate delegate(nullptr, &combo);
    QModelIndex index = combo.model()->index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);
    option.state = QStyle::State_Enabled | QStyle::State_Selected;

    // Act
    QStyleOptionMenuItem result = delegate.getStyleOption(option, index);

    // Assert
    EXPECT_TRUE(result.state & QStyle::State_Selected);
    EXPECT_TRUE(result.checked);  // currentIndex == index.row()
}

TEST(QComboxMenuDelegateGetStyleOption, NonSelectedIndex_NotChecked)
{
    // Arrange
    QComboBox combo;
    combo.addItem("Item1");
    combo.addItem("Item2");
    combo.setCurrentIndex(0);

    QComboxMenuDelegate delegate(nullptr, &combo);
    QModelIndex index = combo.model()->index(1, 0);  // Different from currentIndex
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);
    option.state = QStyle::State_Enabled;

    // Act
    QStyleOptionMenuItem result = delegate.getStyleOption(option, index);

    // Assert
    EXPECT_FALSE(result.checked);  // currentIndex(0) != index.row(1)
    EXPECT_EQ(result.text, QString("Item2"));
}
