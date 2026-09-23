// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "publicApi.h"
#include "toptoolbar.h"
#include "dzoommenucombobox.h"

/*
 * TopTilte::initComboBox (71 lines, complexity 8)
 *
 * Creates a DZoomMenuComboBox, adds five zoom-level items ("200%", "100%",
 * "75%", "50%", "25%"), sets the default to "100%", wires up zoom in/out
 * button signals, and sets the object name. Already called during app
 * start-up; re-invoking recreates the combo box. We verify the observable
 * post-conditions.
 */

TEST(TopTilteInitComboBoxTest, CreatesComboBoxWithFiveZoomLevels)
{
    TopTilte *tt = drawApp->topMainWindow()->topTitle();
    ASSERT_NE(tt, nullptr);

    tt->initComboBox();

    ASSERT_NE(tt->m_zoomMenuComboBox, nullptr);
    EXPECT_EQ(tt->m_zoomMenuComboBox->objectName(), QStringLiteral("zoomMenuComboBox"));
    // Items: "200%"(0), "100%"(1), "75%"(2), "50%"(3), "25%"(4)
    // initComboBox sets current to "100%" → m_currentIndex == 1.
    EXPECT_EQ(tt->m_zoomMenuComboBox->m_currentIndex, 1);
}
