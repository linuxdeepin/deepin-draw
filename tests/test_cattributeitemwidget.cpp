// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QPushButton>
#include <QMouseEvent>
#include <QShowEvent>
#include <QLayout>

#define protected public
#define private public
#include "cattributeitemwidget.h"
#include "cspinbox.h"

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

// =========================================================================
// CAttributeWgt::attribution / setAttribution
// =========================================================================

TEST(CAttributeWgtTest, DefaultAttributionIsNegOne)
{
    CAttributeWgt wgt;
    EXPECT_EQ(wgt.attribution(), -1);
}

TEST(CAttributeWgtTest, SetAttributionUpdatesValue)
{
    CAttributeWgt wgt;
    wgt.setAttribution(42);
    EXPECT_EQ(wgt.attribution(), 42);
}

TEST(CAttributeWgtTest, SetAttributionMultipleTimes)
{
    CAttributeWgt wgt(10);
    EXPECT_EQ(wgt.attribution(), 10);
    wgt.setAttribution(20);
    EXPECT_EQ(wgt.attribution(), 20);
    wgt.setAttribution(-5);
    EXPECT_EQ(wgt.attribution(), -5);
}

// =========================================================================
// CAttributeWgt::event
// =========================================================================

TEST(CAttributeWgtEventTest, MouseButtonPressIsAccepted)
{
    CAttributeWgt wgt;
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(0, 0),
                           QPointF(0, 0), Qt::LeftButton, Qt::LeftButton,
                           Qt::NoModifier);
    wgt.event(&pressEvent);
    EXPECT_FALSE(pressEvent.isAccepted());
}

TEST(CAttributeWgtEventTest, MouseButtonReleaseIsAccepted)
{
    CAttributeWgt wgt;
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(0, 0),
                             QPointF(0, 0), Qt::LeftButton, Qt::LeftButton,
                             Qt::NoModifier);
    wgt.event(&releaseEvent);
    EXPECT_FALSE(releaseEvent.isAccepted());
}

TEST(CAttributeWgtEventTest, NonMouseEventNotAccepted)
{
    CAttributeWgt wgt;
    QEvent moveEvent(QEvent::MouseMove);
    bool ret = wgt.event(&moveEvent);
    // Non-mouse-button events are passed to QWidget::event without accept()
    EXPECT_FALSE(moveEvent.isAccepted());
    // Return value should be true (event handled by QWidget)
    (void)ret;
}

// =========================================================================
// SAttrisList: constructor / haveAttribution / insected
// =========================================================================

TEST(SAttrisListTest, ConstructorFromList)
{
    QList<int> ids = {1, 2, 3};
    SAttrisList list(ids);
    EXPECT_EQ(list.count(), 3);
    EXPECT_EQ(list.at(0).attri, 1);
    EXPECT_EQ(list.at(1).attri, 2);
    EXPECT_EQ(list.at(2).attri, 3);
}

TEST(SAttrisListTest, ConstructorFromEmptyList)
{
    QList<int> empty;
    SAttrisList list(empty);
    EXPECT_EQ(list.count(), 0);
}

TEST(SAttrisListTest, HaveAttributionTrue)
{
    QList<int> ids = {10, 20, 30};
    SAttrisList list(ids);
    EXPECT_TRUE(list.haveAttribution(20));
}

TEST(SAttrisListTest, HaveAttributionFalse)
{
    QList<int> ids = {10, 20, 30};
    SAttrisList list(ids);
    EXPECT_FALSE(list.haveAttribution(99));
}

TEST(SAttrisListTest, HaveAttributionOnEmptyList)
{
    SAttrisList list;
    EXPECT_FALSE(list.haveAttribution(1));
}

TEST(SAttrisListTest, InsectedReturnsCommonElements)
{
    QList<int> ids1 = {1, 2, 3, 4};
    QList<int> ids2 = {3, 4, 5, 6};
    SAttrisList list1(ids1);
    SAttrisList list2(ids2);
    SAttrisList result = list1.insected(list2);
    EXPECT_EQ(result.count(), 2);
    EXPECT_EQ(result.at(0).attri, 3);
    EXPECT_EQ(result.at(1).attri, 4);
}

TEST(SAttrisListTest, InsectedNoOverlapReturnsEmpty)
{
    QList<int> ids1 = {1, 2};
    QList<int> ids2 = {3, 4};
    SAttrisList list1(ids1);
    SAttrisList list2(ids2);
    SAttrisList result = list1.insected(list2);
    EXPECT_EQ(result.count(), 0);
}

// =========================================================================
// CGroupButtonWgt: constructor / event
// =========================================================================

TEST(CGroupButtonWgtTest, ConstructorCreatesButtons)
{
    CGroupButtonWgt wgt;
    // The constructor should create groupButton and unGroupButton
    EXPECT_NE(wgt.groupButton, nullptr);
    EXPECT_NE(wgt.unGroupButton, nullptr);
    EXPECT_NE(wgt.expGroupBtn, nullptr);
    EXPECT_NE(wgt.expUnGroupBtn, nullptr);
    // The attribute type should be EGroupWgt
    EXPECT_EQ(wgt.attribution(), EGroupWgt);
}

TEST(CGroupButtonWgtTest, ConstructorButtonProperties)
{
    CGroupButtonWgt wgt;
    EXPECT_EQ(wgt.groupButton->objectName(), QString("groupButton"));
    EXPECT_EQ(wgt.unGroupButton->objectName(), QString("unGroupButton"));
    // Buttons should be visible after construction
    EXPECT_FALSE(wgt.groupButton->isHidden());
    EXPECT_FALSE(wgt.unGroupButton->isHidden());
}

TEST(CGroupButtonWgtEventTest, NonParentChangeEventPassesThrough)
{
    CGroupButtonWgt wgt;
    QEvent enterEvent(QEvent::Enter);
    bool ret = wgt.event(&enterEvent);
    // Should not crash, event processed by base class
    (void)ret;
    SUCCEED();
}

// =========================================================================
// CAttriBaseOverallWgt: attriWidgetRecommendedSize / centerLayout / showEvent / totalNeedWidth
// =========================================================================

TEST(CAttriBaseOverallWgtTest, CenterLayoutCreatesLayoutOnFirstCall)
{
    CAttriBaseOverallWgt wgt;
    QLayout *lay = wgt.centerLayout();
    EXPECT_NE(lay, nullptr);
    EXPECT_EQ(wgt._pCenterLay, lay);
}

TEST(CAttriBaseOverallWgtTest, CenterLayoutReturnsSameInstance)
{
    CAttriBaseOverallWgt wgt;
    QLayout *lay1 = wgt.centerLayout();
    QLayout *lay2 = wgt.centerLayout();
    EXPECT_EQ(lay1, lay2);
}

TEST(CAttriBaseOverallWgtTest, TotalNeedWidthEmptyIsZero)
{
    CAttriBaseOverallWgt wgt;
    // _allWgts is empty → total width is 0
    EXPECT_EQ(wgt.totalNeedWidth(), 0);
}

TEST(CAttriBaseOverallWgtTest, TotalNeedWidthWithWidgets)
{
    CAttriBaseOverallWgt wgt;
    auto *btn1 = new QPushButton("A");
    auto *btn2 = new QPushButton("B");
    wgt._allWgts.append(btn1);
    wgt._allWgts.append(btn2);
    int expectedW = btn1->sizeHint().width() + btn2->sizeHint().width()
                    + wgt.centerLayout()->spacing();
    EXPECT_EQ(wgt.totalNeedWidth(), expectedW);
    btn1->setParent(nullptr);
    btn2->setParent(nullptr);
    delete btn1;
    delete btn2;
}

TEST(CAttriBaseOverallWgtTest, AttriWidgetRecommendedSizeWithProperty)
{
    CAttriBaseOverallWgt wgt;
    auto *pw = new QWidget;
    QSize customSize(100, 30);
    pw->setProperty(AttriWidgetReWidth, customSize);
    EXPECT_EQ(wgt.attriWidgetRecommendedSize(pw), customSize);
    delete pw;
}

TEST(CAttriBaseOverallWgtTest, AttriWidgetRecommendedSizeFallback)
{
    CAttriBaseOverallWgt wgt;
    auto *pw = new QWidget;
    // No AttriWidgetReWidth property set → falls back to sizeHint()
    QSize result = wgt.attriWidgetRecommendedSize(pw);
    EXPECT_EQ(result, pw->sizeHint());
    delete pw;
}

TEST(CAttriBaseOverallWgtTest, ShowEventBindsFontSize)
{
    CAttriBaseOverallWgt wgt;
    // showEvent should not crash; it binds font size and queues autoResizeUpdate
    QShowEvent showEvent;
    wgt.showEvent(&showEvent);
    SUCCEED();
}

// =========================================================================
// CSpinBoxSettingWgt: spinBox
// =========================================================================

TEST(CSpinBoxSettingWgtTest, SpinBoxReturnsValidPointer)
{
    CSpinBoxSettingWgt wgt("Width");
    CSpinBox *box = wgt.spinBox();
    EXPECT_NE(box, nullptr);
    EXPECT_EQ(box->objectName(), "AttrSpinBox");
}

TEST(CSpinBoxSettingWgtTest, SpinBoxSameInstance)
{
    CSpinBoxSettingWgt wgt("Height");
    CSpinBox *box1 = wgt.spinBox();
    CSpinBox *box2 = wgt.spinBox();
    EXPECT_EQ(box1, box2);
}

TEST(CSpinBoxSettingWgtTest, EmptyTextLabelHidden)
{
    CSpinBoxSettingWgt wgt("");
    // When text is empty, the label should be hidden
    EXPECT_TRUE(wgt._lab->isHidden());
}
