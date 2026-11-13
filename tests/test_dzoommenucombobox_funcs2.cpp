// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DZoomMenuComboBox API (issue V-7429)
 *
 * Source: src/widgets/dzoommenucombobox.h / .cpp
 *
 * NOTE: setArrowICon(QIcon) is declared in the header but has NO
 * implementation in the source code (neither dzoommenucombobox.cpp nor
 * dmenucombobox.cpp contains a definition). Calling it would cause a linker
 * error. DMenuComboBox::setArrowICon has the same issue — dmenucombobox.cpp
 * is entirely wrapped in #if 0.
 *
 * The remaining API is therefore exercised directly: the constructor's
 * members, setMenuFlat() and setItemICon().
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QIcon>
#include <QPixmap>
#include <QString>

#define protected public
#define private public
#include "dzoommenucombobox.h"
#include "publicApi.h"
#undef protected
#undef private

namespace {

// Side length of the throwaway pixmaps used as item icons.
constexpr int kTestIconSize = 8;

QIcon solidIcon(Qt::GlobalColor color)
{
    QPixmap pm(kTestIconSize, kTestIconSize);
    pm.fill(color);
    return QIcon(pm);
}

} // namespace

TEST(DZoomMenuComboBoxArrowIcon, ConstructsWithoutArrowIcon)
{
    DZoomMenuComboBox combo(nullptr);

    // The constructor's initUI() builds the menu button and the popup menu;
    // no items exist until addItem() is called.
    EXPECT_NE(combo.m_btn, nullptr);
    EXPECT_NE(combo.m_menu, nullptr);
    EXPECT_TRUE(combo.m_actions.isEmpty());
}

TEST(DZoomMenuComboBoxArrowIcon, SetMenuFlatTogglesMenuButtonFlatState)
{
    DZoomMenuComboBox combo(nullptr);
    ASSERT_NE(combo.m_btn, nullptr);

    combo.setMenuFlat(true);
    EXPECT_TRUE(combo.m_btn->isFlat());

    combo.setMenuFlat(false);
    EXPECT_FALSE(combo.m_btn->isFlat());
}

TEST(DZoomMenuComboBoxArrowIcon, SetItemIConAppliesIconToExistingItem)
{
    DZoomMenuComboBox combo(nullptr);
    combo.addItem("item1");
    ASSERT_EQ(combo.m_actions.count(), 1);

    const QIcon icon = solidIcon(Qt::green);
    combo.setItemICon(0, icon);

    // The icon lands on the action backing item 0.
    EXPECT_FALSE(combo.m_actions.at(0)->icon().isNull());
    EXPECT_EQ(combo.m_actions.at(0)->icon().pixmap(kTestIconSize,
                                                   kTestIconSize).toImage(),
              icon.pixmap(kTestIconSize, kTestIconSize).toImage());
}
