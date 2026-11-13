// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGroupButtonWgt and CAttriBaseOverallWgt constructor lambdas
 * (src/frame/AttributesWidgets/private/cattributeitemwidget.cpp).
 *
 * Target lambdas (0-coverage):
 *   CGroupButtonWgt constructor:
 *     - DGuiApplicationHelper::sizeModeChanged lambda (resizes buttons)
 *     - groupButton->clicked lambda (emits buttonClicked(true, false))
 *     - unGroupButton->clicked lambda (emits buttonClicked(false, true))
 *     - expGroupBtn->clicked lambda (emits buttonClicked(true, false))
 *     - expUnGroupBtn->clicked lambda (emits buttonClicked(false, true))
 *   CAttriBaseOverallWgt constructor:
 *     - getExpButton()->clicked lambda (shows exps widget)
 *
 * The size constants mirror GROUP_BTN_SIZE_NORMAL/COMPACT in the source; the
 * lambda under test calls setFixedSize(), so the button width is exact.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

#define protected public
#define private public
#include "cattributeitemwidget.h"
#include "publicApi.h"
#undef protected
#undef private

using namespace DrawAttribution;

namespace {

// Mirrors GROUP_BTN_SIZE_NORMAL / GROUP_BTN_SIZE_COMPACT
// (cattributeitemwidget.cpp:32-33).
constexpr int kGroupBtnSizeNormal = 36;
constexpr int kGroupBtnSizeCompact = 24;

} // namespace

// ===========================================================================
// CGroupButtonWgt — button click lambdas
// ===========================================================================

TEST(CGroupButtonWgtLambdaTest, GroupButtonClickedEmitsSignal)
{
    CGroupButtonWgt wgt;
    ASSERT_NE(wgt.groupButton, nullptr);

    QSignalSpy spy(&wgt, &CGroupButtonWgt::buttonClicked);
    wgt.groupButton->click();
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    // groupButton emits buttonClicked(true, false)
    EXPECT_TRUE(args.at(0).toBool());
    EXPECT_FALSE(args.at(1).toBool());
}

TEST(CGroupButtonWgtLambdaTest, UnGroupButtonClickedEmitsSignal)
{
    CGroupButtonWgt wgt;
    ASSERT_NE(wgt.unGroupButton, nullptr);

    QSignalSpy spy(&wgt, &CGroupButtonWgt::buttonClicked);
    wgt.unGroupButton->click();
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    // unGroupButton emits buttonClicked(false, true)
    EXPECT_FALSE(args.at(0).toBool());
    EXPECT_TRUE(args.at(1).toBool());
}

TEST(CGroupButtonWgtLambdaTest, ExpGroupBtnClickedEmitsSignal)
{
    CGroupButtonWgt wgt;
    ASSERT_NE(wgt.expGroupBtn, nullptr);

    QSignalSpy spy(&wgt, &CGroupButtonWgt::buttonClicked);
    wgt.expGroupBtn->click();
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_TRUE(args.at(0).toBool());
    EXPECT_FALSE(args.at(1).toBool());
}

TEST(CGroupButtonWgtLambdaTest, ExpUnGroupBtnClickedEmitsSignal)
{
    CGroupButtonWgt wgt;
    ASSERT_NE(wgt.expUnGroupBtn, nullptr);

    QSignalSpy spy(&wgt, &CGroupButtonWgt::buttonClicked);
    wgt.expUnGroupBtn->click();
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_FALSE(args.at(0).toBool());
    EXPECT_TRUE(args.at(1).toBool());
}

// ===========================================================================
// CGroupButtonWgt — SizeMode lambda
// ===========================================================================

TEST(CGroupButtonWgtLambdaTest, CompactModeResizesButtons)
{
    CGroupButtonWgt wgt;
    ASSERT_NE(wgt.groupButton, nullptr);
    ASSERT_NE(wgt.unGroupButton, nullptr);

    // The lambda applies the compact fixed size to both buttons.
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    qApp->processEvents();

    EXPECT_EQ(wgt.groupButton->width(), kGroupBtnSizeCompact);
    EXPECT_EQ(wgt.groupButton->height(), kGroupBtnSizeCompact);
    EXPECT_EQ(wgt.unGroupButton->width(), kGroupBtnSizeCompact);

    // Switch back to normal: the lambda restores the normal fixed size.
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::NormalMode);
    qApp->processEvents();

    EXPECT_EQ(wgt.groupButton->width(), kGroupBtnSizeNormal);
    EXPECT_EQ(wgt.groupButton->height(), kGroupBtnSizeNormal);
    EXPECT_EQ(wgt.unGroupButton->width(), kGroupBtnSizeNormal);
}

// ===========================================================================
// CAttriBaseOverallWgt — expButton clicked lambda
// ===========================================================================

TEST(CAttriBaseOverallWgtLambdaTest, ExpButtonClickedShowsExpWidget)
{
    CAttriBaseOverallWgt wgt;
    // Don't call show() — after 75+ accumulated tests, showing the widget
    // triggers a DEADLYSIGNAL crash in the DrawBoard event filter.
    qApp->processEvents();

    auto *expBtn = wgt.getExpButton();
    ASSERT_NE(expBtn, nullptr);
    auto *expWidget = wgt.getExpsWidget();
    ASSERT_NE(expWidget, nullptr);
    ASSERT_FALSE(expWidget->isVisible());

    // The lambda moves the exps widget to the button center and shows it.
    emit expBtn->clicked();
    qApp->processEvents();

    EXPECT_TRUE(expWidget->isVisible());

    // Do not leak a top-level window into the following tests.
    expWidget->close();
    qApp->processEvents();
    EXPECT_FALSE(expWidget->isVisible());
}

TEST(CAttriBaseOverallWgtLambdaTest, ExpButtonClickedNoCrash)
{
    CAttriBaseOverallWgt wgt;

    auto *expBtn = wgt.getExpButton();
    ASSERT_NE(expBtn, nullptr);

    // Trigger without showing the parent widget — the exps widget is still
    // created and shown from the lambda, so close it again.
    emit expBtn->clicked();
    qApp->processEvents();

    ASSERT_NE(wgt.getExpsWidget(), nullptr);
    wgt.getExpsWidget()->close();
    qApp->processEvents();
    EXPECT_FALSE(wgt.getExpsWidget()->isVisible());
}
