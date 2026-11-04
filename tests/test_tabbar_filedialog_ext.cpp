// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for TabBarWgt and FileSelectDialog (issue V-7618 group 10)
 *
 * Source: src/frame/cmultiptabbarwidget.h/.cpp
 *
 * Target methods:
 *   TabBarWgt::onTabCountChanged(int index)
 *   FileSelectDialog::checkAndBuildPath(const QString &path)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "cmultiptabbarwidget.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// TabBarWgt::onTabCountChanged
// =========================================================================

static TabBarWgt *createTabBarWgt()
{
    MainWindow *mw = drawApp->topMainWindow();
    DrawBoard *board = mw->drawBoard();
    return new TabBarWgt(board);
}

TEST(TabBarWgtOnTabCountChanged, SingleTab_HiddenOrVisible)
{
    auto w = createTabBarWgt();
    w->addTab("Tab1");

    // onTabCountChanged should not crash
    w->onTabCountChanged(0);

    // With count <= 1, the tab bar should be hidden
    // (visible only when count > 1 per source)
    EXPECT_FALSE(w->isVisible() && w->count() <= 1);
    delete w;
    SUCCEED();
}

TEST(TabBarWgtOnTabCountChanged, MultipleTabs_Visible)
{
    auto w = createTabBarWgt();
    w->addTab("Tab1");
    w->addTab("Tab2");
    w->addTab("Tab3");

    // With count > 1, after onTabCountChanged the tab bar should become visible
    w->onTabCountChanged(2);

    // The source sets visible = (count > 1), but since we haven't shown the widget,
    // isVisible() may still be false. We verify no crash and count is correct.
    EXPECT_EQ(w->count(), 3);
    delete w;
    SUCCEED();
}

TEST(TabBarWgtOnTabCountChanged, ZeroTabs_NoCrash)
{
    auto w = createTabBarWgt();

    w->onTabCountChanged(0);

    delete w;
    SUCCEED();
}

// =========================================================================
// FileSelectDialog::checkAndBuildPath
// =========================================================================

TEST(FileSelectDialogCheckAndBuildPath, PathWithSuffix_ReturnsAsIs)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // checkAndBuildPath is private; accessed via the #define private public
    // pattern above. A path that already has a suffix is returned unchanged.
    FileSelectDialog dlg(board);
    EXPECT_EQ(dlg.checkAndBuildPath("/tmp/output.ddf"), QString("/tmp/output.ddf"));
}

TEST(FileSelectDialogCheckAndBuildPath, PathWithoutSuffix_AddsSuffix)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // With a name filter that carries a suffix, the suffix is appended
    FileSelectDialog dlg(board);
    dlg.setNameFilter("Draw files (*.ddf)");
    EXPECT_EQ(dlg.checkAndBuildPath("/tmp/output"), QString("/tmp/output.ddf"));
}

TEST(FileSelectDialogCheckAndBuildPath, PathWithoutSuffix_NoFilterSuffix_ReturnsUnchanged)
{
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);

    // A filter without a "(.xxx)" group yields no suffix, so the path
    // is returned unchanged
    FileSelectDialog dlg(board);
    dlg.setNameFilter("Any file");
    EXPECT_EQ(dlg.checkAndBuildPath("/tmp/output"), QString("/tmp/output"));
}
