// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPushButton>

#define protected public
#define private public
#include "cattributeitemwidget.h"

using namespace DrawAttribution;

/*
 * CAttriBaseOverallWgt::autoResizeUpdate (43 lines, complexity 6)
 *
 * When the total recommended width of all child widgets (_allWgts) exceeds the
 * widget width, the expand button is shown and overflow widgets are moved into
 * a dropdown. Otherwise the expand button is hidden and all widgets are shown
 * inline.
 *
 * Test 1: empty _allWgts, totalNeedWidth()==0 ≤ width() → expand button hidden.
 * Test 2: _allWgts with content exceeding width → expand button shown.
 */

TEST(CAttriBaseOverallWgtAutoResizeTest, EmptyWidgetsHidesExpandButton)
{
    CAttriBaseOverallWgt wgt;
    wgt.resize(200, 40);

    // _allWgts is empty → totalNeedWidth() returns 0 ≤ 200 → else branch:
    // expand button is hidden.
    wgt.autoResizeUpdate();

    // Use isHidden() (explicit hide flag) rather than isVisible() because the
    // widget is never shown on screen in a headless test.
    EXPECT_TRUE(wgt.getExpButton()->isHidden());
}

TEST(CAttriBaseOverallWgtAutoResizeTest, OverflowShowsExpandButton)
{
    CAttriBaseOverallWgt wgt;
    wgt.resize(10, 40);  // very narrow width

    // Add child widgets whose combined recommended width exceeds 10px.
    auto *child1 = new QPushButton("AAAA");
    auto *child2 = new QPushButton("BBBB");
    wgt._allWgts.append(child1);
    wgt._allWgts.append(child2);

    wgt.autoResizeUpdate();

    // totalNeedWidth() > width() → expand button shown (not hidden).
    EXPECT_FALSE(wgt.getExpButton()->isHidden());

    // Cleanup: reparent to avoid double-free.
    child1->setParent(nullptr);
    child2->setParent(nullptr);
    delete child1;
    delete child2;
}
