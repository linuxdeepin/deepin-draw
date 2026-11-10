// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for attribute widget registration, focus, event filter, and
 * button sizing (src/frame/AttributesWidgets/private/cattributeitemwidget.cpp).
 *
 * Covers (issue V-8025 §3):
 *   - CAttriBaseOverallWgt::addWidget() — child widget registration
 *   - CAttriBaseOverallWgt::setWidgetAllPosterityFocus() — recursive focus
 *   - CGroupButtonWgt::eventFilter() — event delegation
 *   - CColorSettingButton::setDefaultButtonSize() — button sizing
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPushButton>
#include <QLabel>
#include <QEvent>
#include <QResizeEvent>

#define protected public
#define private public
#include "cattributeitemwidget.h"
#include "publicApi.h"
#undef protected
#undef private

using namespace DrawAttribution;

// ===========================================================================
// CAttriBaseOverallWgt::addWidget — child widget registration
// ===========================================================================

TEST(CAttriBaseOverallWgtAddWidget, AppendsWidgetToAllWgts)
{
    CAttriBaseOverallWgt wgt;

    int initialCount = wgt._allWgts.count();

    // Parent to wgt: Qt's parent-child ownership cleans the child up when
    // wgt goes out of scope (addWidget() itself does not take ownership).
    auto *child = new QPushButton("Test", &wgt);
    wgt.addWidget(child);

    EXPECT_EQ(wgt._allWgts.count(), initialCount + 1);
    EXPECT_TRUE(wgt._allWgts.contains(child));
}

TEST(CAttriBaseOverallWgtAddWidget, MultipleWidgetsAppended)
{
    CAttriBaseOverallWgt wgt;

    auto *child1 = new QPushButton("A", &wgt);
    auto *child2 = new QPushButton("B", &wgt);
    auto *child3 = new QLabel("C", &wgt);

    wgt.addWidget(child1);
    wgt.addWidget(child2);
    wgt.addWidget(child3);

    EXPECT_EQ(wgt._allWgts.count(), 3);
    EXPECT_TRUE(wgt._allWgts.contains(child1));
    EXPECT_TRUE(wgt._allWgts.contains(child2));
    EXPECT_TRUE(wgt._allWgts.contains(child3));
}

TEST(CAttriBaseOverallWgtAddWidget, OrderIsPreserved)
{
    CAttriBaseOverallWgt wgt;

    auto *first = new QPushButton("First", &wgt);
    auto *second = new QPushButton("Second", &wgt);
    auto *third = new QPushButton("Third", &wgt);

    wgt.addWidget(first);
    wgt.addWidget(second);
    wgt.addWidget(third);

    EXPECT_EQ(wgt._allWgts.at(0), first);
    EXPECT_EQ(wgt._allWgts.at(1), second);
    EXPECT_EQ(wgt._allWgts.at(2), third);
}

// ===========================================================================
// CAttriBaseOverallWgt::setWidgetAllPosterityFocus — recursive focus
// ===========================================================================

TEST(CAttriBaseOverallWgtSetFocus, SetsStrongFocusOnDirectChildren)
{
    CAttriBaseOverallWgt wgt;

    auto *child = new QPushButton("Child", &wgt);
    wgt.addWidget(child);

    // Initially the child should not have StrongFocus.
    EXPECT_NE(child->focusPolicy(), Qt::StrongFocus);

    CAttriBaseOverallWgt::setWidgetAllPosterityFocus(&wgt);

    // After the call, the child widget should have StrongFocus.
    EXPECT_EQ(child->focusPolicy(), Qt::StrongFocus);
}

TEST(CAttriBaseOverallWgtSetFocus, SetsStrongFocusOnNestedChildren)
{
    CAttriBaseOverallWgt wgt;

    auto *parent = new QPushButton("Parent", &wgt);
    auto *nested = new QLabel("Nested", parent);
    wgt.addWidget(parent);

    EXPECT_NE(nested->focusPolicy(), Qt::StrongFocus);

    CAttriBaseOverallWgt::setWidgetAllPosterityFocus(&wgt);

    // Recursive: nested children should also get StrongFocus.
    EXPECT_EQ(nested->focusPolicy(), Qt::StrongFocus);
}

TEST(CAttriBaseOverallWgtSetFocus, EmptyWidgets_NoCrash)
{
    CAttriBaseOverallWgt wgt;
    CAttriBaseOverallWgt::setWidgetAllPosterityFocus(&wgt);
    SUCCEED();
}

TEST(CAttriBaseOverallWgtSetFocus, MultipleChildren_AllGetFocus)
{
    CAttriBaseOverallWgt wgt;

    auto *child1 = new QPushButton("A", &wgt);
    auto *child2 = new QPushButton("B", &wgt);
    auto *child3 = new QLabel("C", &wgt);

    wgt.addWidget(child1);
    wgt.addWidget(child2);
    wgt.addWidget(child3);

    CAttriBaseOverallWgt::setWidgetAllPosterityFocus(&wgt);

    EXPECT_EQ(child1->focusPolicy(), Qt::StrongFocus);
    EXPECT_EQ(child2->focusPolicy(), Qt::StrongFocus);
    EXPECT_EQ(child3->focusPolicy(), Qt::StrongFocus);
}

// ===========================================================================
// CGroupButtonWgt::eventFilter — event delegation
// ===========================================================================

TEST(CGroupButtonWgtEventFilter, NonParentChangeEvent_NoCrash)
{
    CGroupButtonWgt wgt;

    QEvent enterEvent(QEvent::Enter);
    // eventFilter() only forwards to CAttributeWgt/QWidget::eventFilter,
    // which never claims the event — expect false.
    bool result = wgt.eventFilter(&wgt, &enterEvent);
    EXPECT_FALSE(result);
}

TEST(CGroupButtonWgtEventFilter, ParentChangeEvent_NoCrash)
{
    CGroupButtonWgt wgt;

    QEvent parentEvent(QEvent::ParentChange);
    bool result = wgt.eventFilter(&wgt, &parentEvent);
    EXPECT_FALSE(result);
}

TEST(CGroupButtonWgtEventFilter, MouseButtonPress_NoCrash)
{
    CGroupButtonWgt wgt;

    QEvent mouseEvent(QEvent::MouseButtonPress);
    bool result = wgt.eventFilter(&wgt, &mouseEvent);
    EXPECT_FALSE(result);
}

TEST(CGroupButtonWgtEventFilter, NullWatched_NoCrash)
{
    CGroupButtonWgt wgt;

    QEvent enterEvent(QEvent::Enter);
    // Pass nullptr as watched — should not crash and still return false.
    bool result = wgt.eventFilter(nullptr, &enterEvent);
    EXPECT_FALSE(result);
}

// ===========================================================================
// CColorSettingButton::setDefaultButtonSize — button sizing
// ===========================================================================

TEST(CColorSettingButtonSetDefaultSize, DefaultSize_IsTwentyTwenty)
{
    CColorSettingButton btn;

    // Default _defaultButtonSize should be QSize(20, 20) per source.
    EXPECT_EQ(btn._defaultButtonSize, QSize(20, 20));
}

TEST(CColorSettingButtonSetDefaultSize, SetCustomSize_UpdatesMember)
{
    CColorSettingButton btn;

    btn.setDefaultButtonSize(QSize(30, 40));
    EXPECT_EQ(btn._defaultButtonSize, QSize(30, 40));
}

TEST(CColorSettingButtonSetDefaultSize, SetSquareSize_UpdatesMember)
{
    CColorSettingButton btn;

    btn.setDefaultButtonSize(QSize(50, 50));
    EXPECT_EQ(btn._defaultButtonSize, QSize(50, 50));
}

TEST(CColorSettingButtonSetDefaultSize, SetSize_UpdatesMemberNotWidget)
{
    CColorSettingButton btn;

    btn.setDefaultButtonSize(QSize(35, 35));
    // setDefaultButtonSize updates the member; the widget may not resize
    // immediately (it may happen in a subsequent resizeEvent).
    EXPECT_EQ(btn._defaultButtonSize, QSize(35, 35));
}

TEST(CColorSettingButtonSetDefaultSize, MultipleUpdates_LastWins)
{
    CColorSettingButton btn;

    btn.setDefaultButtonSize(QSize(10, 10));
    btn.setDefaultButtonSize(QSize(25, 25));
    btn.setDefaultButtonSize(QSize(40, 30));

    EXPECT_EQ(btn._defaultButtonSize, QSize(40, 30));
}
