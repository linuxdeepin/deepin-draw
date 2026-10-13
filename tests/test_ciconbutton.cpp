// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CIconButton methods (issue V-5645).
 *
 * Source: src/widgets/ciconbutton.cpp
 *
 * Target methods: setChecked, isChecked, setIconMode
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMap>
#include <QSize>

#define protected public
#define private public
#include "ciconbutton.h"
#undef protected
#undef private

static CIconButton *createButton(bool isCheckLock = true)
{
    QMap<int, QMap<CIconButton::EIconButtonSattus, QString>> pictureMap;
    return new CIconButton(pictureMap, QSize(55, 36), nullptr, isCheckLock);
}

// =========================================================================
// CIconButton::setChecked / isChecked
// =========================================================================

TEST(CIconButtonSetChecked, SetTrue_CheckedReturnsTrue)
{
    auto *btn = createButton();
    btn->setChecked(true);

    EXPECT_TRUE(btn->isChecked());
    EXPECT_EQ(btn->m_currentStatus, CIconButton::Active);

    delete btn;
}

TEST(CIconButtonSetChecked, SetFalse_CheckedReturnsFalse)
{
    auto *btn = createButton();
    btn->setChecked(true);
    btn->setChecked(false);

    EXPECT_FALSE(btn->isChecked());
    EXPECT_EQ(btn->m_currentStatus, CIconButton::Normal);

    delete btn;
}

TEST(CIconButtonSetChecked, DefaultIsUnchecked)
{
    auto *btn = createButton();

    EXPECT_FALSE(btn->isChecked());

    delete btn;
}

TEST(CIconButtonSetChecked, ToggleChecked)
{
    auto *btn = createButton();

    btn->setChecked(true);
    EXPECT_TRUE(btn->isChecked());

    btn->setChecked(false);
    EXPECT_FALSE(btn->isChecked());

    btn->setChecked(true);
    EXPECT_TRUE(btn->isChecked());

    delete btn;
}

// =========================================================================
// CIconButton::setIconMode
// =========================================================================

TEST(CIconButtonSetIconMode, SetTrue_StoresFlag)
{
    auto *btn = createButton();
    EXPECT_FALSE(btn->m_iconMode);

    btn->setIconMode();

    EXPECT_TRUE(btn->m_iconMode);

    delete btn;
}

TEST(CIconButtonSetIconMode, CalledTwice_StaysTrue)
{
    auto *btn = createButton();
    btn->setIconMode();
    btn->setIconMode();

    EXPECT_TRUE(btn->m_iconMode);

    delete btn;
}
