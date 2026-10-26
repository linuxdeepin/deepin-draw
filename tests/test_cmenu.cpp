// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CMenu (issue V-6403).
 *
 * Source: src/widgets/cmenu.cpp
 *
 * Target methods:
 *   CMenu(QWidget*), CMenu(QString, QWidget*),
 *   hideEvent(QHideEvent*), setActionEnableRecursive(QMenu*, bool)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QAction>
#include <QHideEvent>
#include <QMenu>

#define protected public
#define private public
#include "widgets/cmenu.h"
#undef protected
#undef private

// ---- CMenu constructors ----

TEST(CMenuTest, ConstructWithParentOnly)
{
    CMenu menu;
    EXPECT_EQ(menu.actions().size(), 0);
}

TEST(CMenuTest, ConstructWithTitleAndParent)
{
    CMenu menu(QStringLiteral("TestTitle"));
    EXPECT_EQ(menu.title(), QStringLiteral("TestTitle"));
}

// ---- hideEvent ----

TEST(CMenuHideEvent, EmptyMenuDoesNotCrash)
{
    CMenu menu;
    menu.show();
    QHideEvent event;
    // Directly invoke hideEvent (protected) to avoid platform show/hide
    // timing issues.
    menu.hideEvent(&event);
    SUCCEED();
}

TEST(CMenuHideEvent, ActionsReEnabledAfterHide)
{
    CMenu menu;

    QAction *act1 = menu.addAction(QStringLiteral("Action1"));
    QAction *act2 = menu.addAction(QStringLiteral("Action2"));
    ASSERT_NE(act1, nullptr);
    ASSERT_NE(act2, nullptr);

    // Disable all actions first.
    act1->setEnabled(false);
    act2->setEnabled(false);
    ASSERT_FALSE(act1->isEnabled());
    ASSERT_FALSE(act2->isEnabled());

    menu.show();
    QHideEvent event;
    menu.hideEvent(&event);

    // hideEvent calls setActionEnableRecursive(this, true) which should
    // re-enable every action.
    EXPECT_TRUE(act1->isEnabled());
    EXPECT_TRUE(act2->isEnabled());
}

// ---- setActionEnableRecursive ----

TEST(CMenuSetActionEnableRecursive, DisablesAllActions)
{
    CMenu menu;
    QAction *act1 = menu.addAction(QStringLiteral("A1"));
    QAction *act2 = menu.addAction(QStringLiteral("A2"));

    menu.setActionEnableRecursive(&menu, false);

    EXPECT_FALSE(act1->isEnabled());
    EXPECT_FALSE(act2->isEnabled());
}

TEST(CMenuSetActionEnableRecursive, HandlesSubmenus)
{
    CMenu menu;
    QAction *topAction = menu.addAction(QStringLiteral("Top"));

    CMenu *subMenu = new CMenu(QStringLiteral("Sub"), &menu);
    QAction *subAction = subMenu->addAction(QStringLiteral("SubAction"));
    topAction->setMenu(subMenu);

    menu.setActionEnableRecursive(&menu, false);

    EXPECT_FALSE(topAction->isEnabled());
    EXPECT_FALSE(subAction->isEnabled());

    // Re-enable to verify recursion works in both directions.
    menu.setActionEnableRecursive(&menu, true);

    EXPECT_TRUE(topAction->isEnabled());
    EXPECT_TRUE(subAction->isEnabled());
}

TEST(CMenuSetActionEnableRecursive, EmptyMenuDoesNotCrash)
{
    CMenu menu;
    menu.setActionEnableRecursive(&menu, true);
    SUCCEED();
}
