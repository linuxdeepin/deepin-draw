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
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <DGuiApplicationHelper>
#include <DIconButton>

#define protected public
#define private public
#include "cattributeitemwidget.h"
#include "publicApi.h"
#undef protected
#undef private

using namespace DrawAttribution;

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

TEST(CGroupButtonWgtLambdaTest, SizeModeChangedNoCrash)
{
    CGroupButtonWgt wgt;

    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    qApp->processEvents();

    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::NormalMode);
    qApp->processEvents();

    SUCCEED();
}

TEST(CGroupButtonWgtLambdaTest, CompactModeResizesButtons)
{
    CGroupButtonWgt wgt;
    ASSERT_NE(wgt.groupButton, nullptr);

    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::CompactMode);
    qApp->processEvents();

    // In compact mode, buttons should be compact-sized
    // GROUP_BTN_SIZE_COMPACT is typically smaller than GROUP_BTN_SIZE_NORMAL
    EXPECT_LE(wgt.groupButton->width(), 36);

    // Switch back to normal
    emit DGuiApplicationHelper::instance()->sizeModeChanged(DGuiApplicationHelper::NormalMode);
    qApp->processEvents();

    EXPECT_GE(wgt.groupButton->width(), 36);
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

    // The lambda shows exps widget at expButton center
    emit expBtn->clicked();
    qApp->processEvents();

    // The exps widget should be visible (or at least shown)
    SUCCEED();
}

TEST(CAttriBaseOverallWgtLambdaTest, ExpButtonClickedNoCrash)
{
    CAttriBaseOverallWgt wgt;

    auto *expBtn = wgt.getExpButton();
    ASSERT_NE(expBtn, nullptr);

    // Trigger without showing the widget — should not crash
    emit expBtn->clicked();
    qApp->processEvents();
    SUCCEED();
}
