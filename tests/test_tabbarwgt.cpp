// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "publicApi.h"  // drawApp, MainWindow, DrawBoard (internally define/undef private)

// TabBarWgt's constructor and destructor are private by design (factory-managed);
// expose them so the ctor can be exercised directly. private/protected are kept
// public for the test bodies below so the stack object's dtor is accessible.
#define protected public
#define private public
#include "cmultiptabbarwidget.h"

/*
 * TabBarWgt extends DTK's DTabBar. Its constructor (TabBarWgt(DrawBoard*))
 * configures a set of standard tab-bar properties, wires up the add/close/
 * switch signals to the parent DrawBoard's page manager, and finally hides
 * itself. These tests construct one against the running application's
 * DrawBoard and assert the stable, observable properties set in the ctor.
 */
TEST(TabBarWgtCtorTest, SetsStandardPropertiesAndHidesItself)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    TabBarWgt bar(board);

    // Properties set unconditionally in the ctor.
    EXPECT_TRUE(bar.isMovable());
    EXPECT_TRUE(bar.tabsClosable());
    EXPECT_EQ(bar.focusPolicy(), Qt::NoFocus);

    // The ctor ends with hide(); an un-shown, hidden widget is not visible.
    EXPECT_FALSE(bar.isVisible());

    // pageManager() delegates to the parent DrawBoard and must resolve to it.
    EXPECT_NE(bar.pageManager(), nullptr);
    EXPECT_EQ(bar.pageManager(), board);
}
