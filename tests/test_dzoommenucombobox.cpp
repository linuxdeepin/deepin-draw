// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QAction>
#include <QEvent>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>

// publicApi.h already includes dzoommenucombobox.h with
// protected/private remapped to public (and restores the macros).
#include "publicApi.h"
#include "dzoommenucombobox.h"

/*
 * DZoomMenuComboBox unit tests (V-5185)
 *
 * Exercises the eight key methods:
 *   initUI, setCurrentIndex, setCurrentText, removeItem,
 *   setMenuButtonTextAndIcon, setSizeMode, slotActionToggled,
 *   eventFilter
 *
 * The combo box is reachable through the application-managed
 * TopTilte widget (drawApp->topMainWindow()->topTitle()).
 * TopTilte::initComboBox() creates the combo box and populates it
 * with five zoom-level items: "200%"(0), "100%"(1), "75%"(2),
 * "50%"(3), "25%"(4).
 *
 * setSizeMode, slotActionToggled and eventFilter are declared
 * protected in DZoomMenuComboBox.  We expose them via a test
 * subclass that lifts them into the public section with using-
 * declarations (standard C++ access-adjustment technique).
 */

// Test subclass: exposes protected members for unit testing.
class TestDZoomMenuComboBox : public DZoomMenuComboBox
{
public:
    using DZoomMenuComboBox::setSizeMode;
    using DZoomMenuComboBox::slotActionToggled;
    using DZoomMenuComboBox::eventFilter;

    explicit TestDZoomMenuComboBox(DWidget *parent = nullptr)
        : DZoomMenuComboBox(parent) {}
};

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxConstructorTest, ConstructsWithNullParent)
{
    DZoomMenuComboBox combo(nullptr);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// initUI
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxInitUITest, InitUIDoesNotCrash)
{
    DZoomMenuComboBox combo(nullptr);
    combo.initUI();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// setCurrentIndex
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxSetCurrentIndexTest, UpdatesCurrentIndex)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);
    tt->initComboBox();
    DZoomMenuComboBox *combo = tt->m_zoomMenuComboBox;
    ASSERT_NE(combo, nullptr);

    combo->setCurrentIndex(3);
    EXPECT_EQ(combo->m_currentIndex, 3);

    combo->setCurrentIndex(0);
    EXPECT_EQ(combo->m_currentIndex, 0);
}

// ---------------------------------------------------------------------------
// setCurrentText
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxSetCurrentTextTest, FindsMatchingItemByText)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);
    tt->initComboBox();
    DZoomMenuComboBox *combo = tt->m_zoomMenuComboBox;
    ASSERT_NE(combo, nullptr);

    // "25%" is at index 4 in the initComboBox item list.
    combo->setCurrentText("25%");
    EXPECT_EQ(combo->m_currentIndex, 4);

    // "200%" is at index 0.
    combo->setCurrentText("200%");
    EXPECT_EQ(combo->m_currentIndex, 0);
}

// ---------------------------------------------------------------------------
// removeItem
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxRemoveItemTest, RemovesActionFromMenuAndList)
{
    DZoomMenuComboBox combo(nullptr);
    combo.initUI();

    QAction *action = new QAction("test-item", &combo);
    combo.addItem(action);
    ASSERT_EQ(combo.m_actions.count(), 1);
    ASSERT_TRUE(combo.m_actions.contains(action));

    combo.removeItem(action);

    // The action must be removed from the internal list and the menu,
    // and must be detached from the combo (but not deleted).
    EXPECT_FALSE(combo.m_actions.contains(action));
    EXPECT_FALSE(combo.m_menu->actions().contains(action));
    EXPECT_EQ(action->parent(), nullptr);
}

// ---------------------------------------------------------------------------
// setMenuButtonTextAndIcon
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxSetMenuButtonTextAndIconTest, DoesNotCrash)
{
    DZoomMenuComboBox combo(nullptr);
    combo.initUI();

    combo.setMenuButtonTextAndIcon("Zoom", QIcon());
    SUCCEED();
}

// ---------------------------------------------------------------------------
// setSizeMode  (protected — exposed via TestDZoomMenuComboBox)
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxSetSizeModeTest, TogglesCompactMode)
{
    TestDZoomMenuComboBox combo(nullptr);
    combo.initUI();

    EXPECT_FALSE(combo.m_isCompact);
    combo.setSizeMode(true);
    EXPECT_TRUE(combo.m_isCompact);
    combo.setSizeMode(false);
    EXPECT_FALSE(combo.m_isCompact);
}

// ---------------------------------------------------------------------------
// slotActionToggled  (protected — exposed via TestDZoomMenuComboBox)
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxSlotActionToggledTest, SelectsMatchingItem)
{
    TestDZoomMenuComboBox combo(nullptr);
    combo.initUI();

    // The action must be part of the combo, otherwise
    // slotActionToggled cannot match it and would be a no-op.
    QAction *first = new QAction("first-item", &combo);
    QAction *second = new QAction("second-item", &combo);
    combo.addItem(first);
    combo.addItem(second);
    ASSERT_EQ(combo.m_actions.count(), 2);
    ASSERT_EQ(combo.m_currentIndex, 0);

    combo.slotActionToggled(second);
    EXPECT_EQ(combo.m_currentIndex, 1);

    combo.slotActionToggled(first);
    EXPECT_EQ(combo.m_currentIndex, 0);
}

// ---------------------------------------------------------------------------
// eventFilter  (protected — exposed via TestDZoomMenuComboBox)
// ---------------------------------------------------------------------------

TEST(DZoomMenuComboBoxEventFilterTest, HandlesMenuKeyEvents)
{
    TestDZoomMenuComboBox combo(nullptr);
    combo.initUI();

    // eventFilter watches m_menu: a plain single-character key press
    // without modifiers must be consumed (returns true).
    QKeyEvent singleChar(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "a");
    EXPECT_TRUE(combo.eventFilter(combo.m_menu, &singleChar));

    // Events the filter does not handle are forwarded to the base class.
    QEvent noneEvent(QEvent::None);
    (void)combo.eventFilter(combo.m_menu, &noneEvent);
}
