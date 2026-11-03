// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DZoomMenuComboBox::setArrowICon (issue V-7429)
 *
 * Source: src/widgets/dzoommenucombobox.h / .cpp
 *
 * Target method:
 *   setArrowICon(QIcon icon)
 *
 * NOTE: setArrowICon is declared in the header but has NO implementation
 * in the source code (neither dzoommenucombobox.cpp nor dmenucombobox.cpp
 * contains a definition). Calling it would cause a linker error.
 * DMenuComboBox::setArrowICon has the same issue — dmenucombobox.cpp is
 * entirely wrapped in #if 0.
 *
 * We test the class construction and other related API instead.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QIcon>
#include <QString>

#include "publicApi.h"
#include "dzoommenucombobox.h"

// setArrowICon is declared but not implemented — would cause link error.
// Document this with a QSKIP-style test that verifies the class works
// without it.

TEST(DZoomMenuComboBoxArrowIcon, ConstructsWithoutArrowIcon)
{
    DZoomMenuComboBox combo(nullptr);
    SUCCEED();
}

TEST(DZoomMenuComboBoxArrowIcon, SetMenuFlatDoesNotCrash)
{
    DZoomMenuComboBox combo(nullptr);
    combo.setMenuFlat(true);
    SUCCEED();
}

TEST(DZoomMenuComboBoxArrowIcon, SetItemIConDoesNotCrash)
{
    DZoomMenuComboBox combo(nullptr);
    QIcon icon;
    combo.setItemICon(0, icon);
    SUCCEED();
}
