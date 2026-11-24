// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CTextTool eventFilter font keypress and
 * QComboxMenuDelegate::getStyleOption remaining branches (issue V-9255 area 3)
 *
 * Source: src/drawshape/drawTools/ctexttool.cpp
 *
 * Target:
 *   CTextTool::eventFilter — m_fontSize KeyPress: Up/PageUp, Down/PageDown, clamp 8~500
 *   QComboxMenuDelegate::getStyleOption — ForegroundRole, Disabled, CheckStateRole,
 *     AccessibleDescriptionRole separator, DecorationRole QIcon/QColor/default,
 *     BackgroundRole, FontRole valid/invalid
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QComboBox>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QStyleOptionViewItem>
#include <QApplication>
#include <QLineEdit>

#define protected public
#define private public
#include "drawshape/drawTools/ctexttool.h"
#include "drawshape/drawTools/cdrawtoolfactory.h"
#include "drawshape/drawTools/idrawtool.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cdrawscene.h"
#include "publicApi.h"
#undef protected
#undef private

#include "test_helpers.h"

#ifdef ENABLE_FSANITIZE
#include <sanitizer/lsan_interface.h>
#endif

namespace {

void releaseTextTool(CTextTool *tool)
{
#ifdef ENABLE_FSANITIZE
    if (tool != nullptr)
        __lsan_ignore_object(tool);
#else
    Q_UNUSED(tool)
#endif
}

CTextTool *createTextToolForFontKbTest()
{
    ensurePageExists();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    CTextTool *tool = qobject_cast<CTextTool *>(CDrawToolFactory::Create(text));
    if (tool) {
        tool->setDrawBoard(board);
        tool->initFontWeightWidget();
        tool->initFontFamilyWidget(tool->m_fontHeavy);
        tool->initFontFontSizeWidget();
    }
    return tool;
}

// Send a key event through the tool's eventFilter targeting m_fontSize.
bool sendFontKeyToTool(CTextTool *tool, int key)
{
    QKeyEvent keyEvent(QEvent::KeyPress, key, Qt::NoModifier);
    return tool->eventFilter(tool->m_fontSize, &keyEvent);
}

} // namespace

// =========================================================================
// CTextTool::eventFilter — m_fontSize KeyPress: Up/PageUp increases
// =========================================================================

TEST(CTextToolFontKb, KeyUp_IncreasesFontSize)
{
    CTextTool *tool = createTextToolForFontKbTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    tool->m_fontSize->setCurrentText("12px");
    bool handled = sendFontKeyToTool(tool, Qt::Key_Up);

    EXPECT_TRUE(handled);
    EXPECT_EQ(tool->m_fontSize->currentText(), "13px");

    releaseTextTool(tool);
}

TEST(CTextToolFontKb, KeyPageUp_IncreasesFontSize)
{
    CTextTool *tool = createTextToolForFontKbTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    tool->m_fontSize->setCurrentText("100px");
    bool handled = sendFontKeyToTool(tool, Qt::Key_PageUp);

    EXPECT_TRUE(handled);
    EXPECT_EQ(tool->m_fontSize->currentText(), "101px");

    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::eventFilter — m_fontSize KeyPress: Down/PageDown decreases
// =========================================================================

TEST(CTextToolFontKb, KeyDown_DecreasesFontSize)
{
    CTextTool *tool = createTextToolForFontKbTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    tool->m_fontSize->setCurrentText("100px");
    bool handled = sendFontKeyToTool(tool, Qt::Key_Down);

    EXPECT_TRUE(handled);
    EXPECT_EQ(tool->m_fontSize->currentText(), "99px");

    releaseTextTool(tool);
}

TEST(CTextToolFontKb, KeyPageDown_DecreasesFontSize)
{
    CTextTool *tool = createTextToolForFontKbTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    tool->m_fontSize->setCurrentText("50px");
    bool handled = sendFontKeyToTool(tool, Qt::Key_PageDown);

    EXPECT_TRUE(handled);
    EXPECT_EQ(tool->m_fontSize->currentText(), "49px");

    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::eventFilter — clamp to 8 (minimum)
// =========================================================================

TEST(CTextToolFontKb, KeyDown_AtMin_ClampedTo8)
{
    CTextTool *tool = createTextToolForFontKbTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    tool->m_fontSize->setCurrentText("8px");
    bool handled = sendFontKeyToTool(tool, Qt::Key_Down);

    EXPECT_TRUE(handled);
    // 8 - 1 = 7, clamped to 8
    EXPECT_EQ(tool->m_fontSize->currentText(), "8px");

    releaseTextTool(tool);
}

// =========================================================================
// CTextTool::eventFilter — clamp to 500 (maximum)
// =========================================================================

TEST(CTextToolFontKb, KeyUp_AtMax_ClampedTo500)
{
    CTextTool *tool = createTextToolForFontKbTest();
    ASSERT_NE(tool, nullptr);
    ASSERT_NE(tool->m_fontSize, nullptr);

    tool->m_fontSize->setCurrentText("500px");
    bool handled = sendFontKeyToTool(tool, Qt::Key_Up);

    EXPECT_TRUE(handled);
    // 500 + 1 = 501, clamped to 500
    EXPECT_EQ(tool->m_fontSize->currentText(), "500px");

    releaseTextTool(tool);
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — ForegroundRole → QBrush
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, ForegroundRole_QBrush_Applied)
{
    // Build a combo box so the delegate's mCombo is valid
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("test");
    item.setForeground(QBrush(QColor(255, 0, 0)));
    model.appendRow(&item);
    combo.setModel(&model);

    auto *delegate = qobject_cast<QComboxMenuDelegate *>(combo.itemDelegate());
    // The combo's default delegate is not QComboxMenuDelegate; create one
    QComboxMenuDelegate customDelegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = customDelegate.getStyleOption(option, idx);

    // The foreground brush should be applied to the palette
    EXPECT_EQ(menuOption.palette.brush(QPalette::WindowText), QBrush(QColor(255, 0, 0)));
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — Disabled item
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, DisabledItem_PaletteDisabled)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("disabled");
    item.setEnabled(false);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.state = QStyle::State_Enabled;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    // Item is disabled → palette should be set to Disabled color group
    EXPECT_EQ(menuOption.palette.currentColorGroup(), QPalette::Disabled);
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — CheckStateRole valid (Checked)
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, CheckStateRole_Checked_SetsStateOn)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("checked");
    item.setCheckable(true);
    item.setCheckState(Qt::Checked);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_TRUE(menuOption.checked);
    EXPECT_TRUE(menuOption.state & QStyle::State_On);
}

TEST(QComboxMenuDelegateGetStyleOption, CheckStateRole_Unchecked_SetsStateOff)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("unchecked");
    item.setCheckable(true);
    item.setCheckState(Qt::Unchecked);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_FALSE(menuOption.checked);
    EXPECT_TRUE(menuOption.state & QStyle::State_Off);
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — AccessibleDescriptionRole "separator"
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, Separator_SetsMenuItemTypeSeparator)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item;
    item.setData("separator", Qt::AccessibleDescriptionRole);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_EQ(menuOption.menuItemType, QStyleOptionMenuItem::Separator);
}

TEST(QComboxMenuDelegateGetStyleOption, NonSeparator_SetsMenuItemTypeNormal)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("normal");
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_EQ(menuOption.menuItemType, QStyleOptionMenuItem::Normal);
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — DecorationRole: QIcon
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, DecorationRole_QIcon_Applied)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("icon");
    // Build the icon from a pixmap: a theme icon is null when the icon theme
    // lacks "dialog-ok" (CI / offscreen), which would fail the assertion.
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    QIcon icon(pixmap);
    item.setIcon(icon);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    // The same QIcon must pass through the QMetaType::QIcon branch (the
    // default branch would cast it to a null QPixmap).
    EXPECT_FALSE(menuOption.icon.isNull());
    EXPECT_EQ(menuOption.icon.cacheKey(), icon.cacheKey());
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — DecorationRole: QColor
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, DecorationRole_QColor_Applied)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("color");
    item.setData(QColor(0, 255, 0), Qt::DecorationRole);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.decorationSize = QSize(16, 16);
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_FALSE(menuOption.icon.isNull());
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — DecorationRole: default (QPixmap)
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, DecorationRole_Default_NoCrash)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("default-decoration");
    // No decoration role data → falls into default case
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    // No crash, icon may be null but the method completes
    SUCCEED();
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — BackgroundRole → QBrush
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, BackgroundRole_QBrush_Applied)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("bg");
    item.setBackground(QBrush(QColor(0, 0, 255)));
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_EQ(menuOption.palette.brush(QPalette::Window), QBrush(QColor(0, 0, 255)));
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — FontRole valid
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, FontRole_Valid_UsesItemFont)
{
    QComboBox combo;
    QStandardItemModel model;
    QStandardItem item("font-role");
    QFont customFont("Monospace", 14);
    item.setFont(customFont);
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    EXPECT_EQ(menuOption.font.family(), customFont.family());
    EXPECT_EQ(menuOption.font.pointSize(), 14);
}

// =========================================================================
// QComboxMenuDelegate::getStyleOption — FontRole invalid (falls to combo font)
// =========================================================================

TEST(QComboxMenuDelegateGetStyleOption, FontRole_Invalid_FallsToComboFont)
{
    QComboBox combo;
    QFont comboFont("Sans Serif", 10);
    combo.setFont(comboFont);

    QStandardItemModel model;
    QStandardItem item("no-font-role");
    model.appendRow(&item);
    combo.setModel(&model);

    QComboxMenuDelegate delegate(nullptr, &combo);

    QModelIndex idx = model.index(0, 0);
    QStyleOptionViewItem option;
    option.rect = QRect(0, 0, 100, 20);

    QStyleOptionMenuItem menuOption = delegate.getStyleOption(option, idx);

    // With WA_SetFont, the combo font is used
    EXPECT_EQ(menuOption.font.family(), comboFont.family());
}
