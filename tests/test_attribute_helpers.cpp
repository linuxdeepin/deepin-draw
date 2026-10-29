// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CExpButton, CSpline, CExpWgt, CComBoxSettingWgt (issue V-7155)
 *
 * Source: src/frame/AttributesWidgets/private/cattributeitemwidget.h/.cpp
 *         src/frame/AttributesWidgets/private/cattributemanagerwgt.cpp
 *
 * Branch map:
 *   B1  CExpButton constructor creates _expBtn
 *   B2  CExpButton: _expBtn click emits CExpButton::clicked signal
 *   B3  CSpline constructor sets geometry, frame shape, shadow
 *   B4  CSpline show + grab (no crash)
 *   B5  CExpWgt constructor creates _pCenterLay
 *   B6  CExpWgt::addWidget — widgetShowInWindow=false branch: widget hidden, not added
 *   B7  CExpWgt::addWidget — widgetShowInWindow=true, first widget: added, no spline
 *   B8  CExpWgt::addWidget — widgetShowInWindow=true, second widget: spline created
 *   B9  CExpWgt::clear clears _widgets
 *   B9b clear() leaves the added widgets parented to the container (and still
 *       not hidden), and leaves the splines alive: stale controls can remain on
 *       screen after _widgets is already empty (known defect, documented not
 *       fixed — source changes disallowed)
 *   B9c clear()-then-show still finds the stale widget as a child of CExpWgt
 *   B10 CExpWgt::widgetShowInWindow — no property → default true
 *   B11 CExpWgt::widgetShowInWindow — property set false → false
 *   B12 CExpWgt::widgetMarginInWindow — no property → default QMargins(20,0,20,0)
 *   B13 CExpWgt::widgetMarginInWindow — property set → custom margins
 *   B14 CExpWgt::widgetAlignInWindow — no property → default Qt::AlignLeft
 *   B15 CExpWgt::widgetAlignInWindow — property set → custom alignment
 *   B16 CExpWgt showEvent/paintEvent via grab()
 *   B17 CComBoxSettingWgt constructor with text → label visible
 *   B18 CComBoxSettingWgt constructor with empty text → label hidden
 *   B19 CComBoxSettingWgt::comboBox() returns non-null default
 *   B20 CComBoxSettingWgt::setText(empty) hides label
 *   B21 CComBoxSettingWgt::setText(non-empty) shows label
 *   B22 CComBoxSettingWgt::setComboBox replaces old combo
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QLabel>
#include <QComboBox>
#include <QCoreApplication>
#include <QEvent>
#include <QHBoxLayout>

#define protected public
#define private public
#include "cattributeitemwidget.h"
#include "cattributemanagerwgt.h"
#include "publicApi.h"
#undef protected
#undef private

#if TEST_ATTRHELPERS_ITEM

using namespace DrawAttribution;

// =========================================================================
// CExpButton
// =========================================================================

TEST(CExpButtonTest, Constructor_CreatesExpBtn)
{
    // B1: constructor creates _expBtn
    CExpButton btn;
    EXPECT_NE(btn._expBtn, nullptr);
}

TEST(CExpButtonTest, ExpBtnClick_EmitsClicked)
{
    // B2: _expBtn click emits CExpButton::clicked signal
    CExpButton btn;
    QSignalSpy spy(&btn, &CExpButton::clicked);
    btn._expBtn->click();
    EXPECT_EQ(spy.count(), 1);
}

// =========================================================================
// CSpline
// =========================================================================

TEST(CSplineTest, Constructor_SetsGeometryAndFrame)
{
    // B3: constructor sets geometry, frame shape, shadow
    CSpline spline;
    EXPECT_EQ(spline.geometry(), QRect(300, 200, 118, 3));
    EXPECT_EQ(spline.frameShape(), QFrame::HLine);
    EXPECT_EQ(spline.frameShadow(), QFrame::Sunken);
}

TEST(CSplineTest, ShowAndGrab_NoCrash)
{
    // B4: show + grab
    CSpline spline;
    spline.resize(120, 5);
    QPixmap pix = spline.grab();
    EXPECT_FALSE(pix.isNull());
}

// =========================================================================
// CExpWgt
// =========================================================================

TEST(CExpWgtTest, Constructor_CreatesCenterLayout)
{
    // B5: constructor creates _pCenterLay
    CExpWgt wgt;
    EXPECT_NE(wgt._pCenterLay, nullptr);
}

TEST(CExpWgtTest, AddWidget_ShowInWindowFalse_WidgetHiddenNotAdded)
{
    // B6: widgetShowInWindow=false → widget hidden, not added to _widgets
    CExpWgt wgt;
    QWidget child;
    child.setProperty(WidgetShowInVerWindow, false);
    wgt.addWidget(&child);
    EXPECT_TRUE(child.isHidden());
    EXPECT_TRUE(wgt._widgets.isEmpty());
}

TEST(CExpWgtTest, AddWidget_ShowInWindowTrue_FirstWidget_NoSpline)
{
    // B7: widgetShowInWindow=true, first widget → added, no spline
    CExpWgt wgt;
    QWidget child;
    wgt.addWidget(&child);
    EXPECT_EQ(wgt._widgets.count(), 1);
    EXPECT_EQ(wgt._widgets.first(), &child);
    EXPECT_TRUE(wgt._splines.isEmpty());
}

TEST(CExpWgtTest, AddWidget_SecondWidget_SplineCreated)
{
    // B8: second widget → spline separator created
    CExpWgt wgt;
    QWidget child1;
    QWidget child2;
    wgt.addWidget(&child1);
    wgt.addWidget(&child2);
    EXPECT_EQ(wgt._widgets.count(), 2);
    EXPECT_EQ(wgt._splines.count(), 1);
}

TEST(CExpWgtTest, Clear_ClearsWidgets)
{
    // B9: clear clears _widgets
    CExpWgt wgt;
    QWidget child1;
    QWidget child2;
    wgt.addWidget(&child1);
    wgt.addWidget(&child2);
    EXPECT_EQ(wgt._widgets.count(), 2);
    wgt.clear();
    EXPECT_TRUE(wgt._widgets.isEmpty());
}

TEST(CExpWgtTest, Clear_LeavesAddedWidgetsAsVisibleChildren)
{
    // B9b: KNOWN DEFECT (documented, not fixed — source changes disallowed):
    // clear() empties _widgets and deletes the layout items, but it never
    // reparents/hides/deletes the widgets themselves. addWidget() ->
    // packageWidget() does pWidget->setParent(this), so the widgets stay
    // children of the CExpWgt and become visible again with it: stale controls
    // can remain or reappear on screen after _widgets has already been emptied.
    //
    // TODO(V-7155): this test deliberately pins the current (buggy) behavior so
    // the defect is visible in the suite. Once clear() is fixed to hide/remove
    // the widgets, invert the visibility/parent assertions below.
    CExpWgt wgt;
    // Heap-allocate: addWidget() -> packageWidget() reparents the widgets to
    // wgt (pWidget->setParent(this)), so wgt owns and destroys them — no leaks,
    // and ownership is not split with a stack object in this scope.
    QWidget *child1 = new QWidget();
    QWidget *child2 = new QWidget();
    wgt.addWidget(child1);
    wgt.addWidget(child2);
    child1->show();
    child2->show();
    ASSERT_EQ(child1->parentWidget(), &wgt);
    ASSERT_EQ(child2->parentWidget(), &wgt);

    wgt.clear();

    // The bookkeeping list is empty ...
    EXPECT_TRUE(wgt._widgets.isEmpty());
    // ... but the widgets are still parented to the container and still shown.
    EXPECT_EQ(child1->parentWidget(), &wgt);
    EXPECT_EQ(child2->parentWidget(), &wgt);
    EXPECT_TRUE(child1->isVisibleTo(&wgt));
    EXPECT_TRUE(child2->isVisibleTo(&wgt));

    // The spline separators are only scheduled via deleteLater(). Qt dispatches
    // DeferredDelete only when returning to the event loop (processEvents()
    // alone does not), so post them explicitly to make removal deterministic.
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    EXPECT_TRUE(wgt.findChildren<CSpline *>().isEmpty());

    // Re-showing the expansion widget brings the stale widgets back on screen.
    wgt.show();
    EXPECT_TRUE(wgt.isVisible());
    EXPECT_TRUE(child1->isVisible());
    EXPECT_TRUE(child2->isVisible());
    wgt.hide();
}

TEST(CExpWgtTest, ClearThenAddWidget_WidgetsListHoldsOnlyNewWidget)
{
    // B9c: after clear() the bookkeeping list does start from scratch, so only
    // the newly added widget is tracked — while the stale one stays a child.
    CExpWgt wgt;
    // Heap-allocated because addWidget() reparents them to wgt, which owns them.
    QWidget *stale = new QWidget();
    QWidget *fresh = new QWidget();
    wgt.addWidget(stale);
    wgt.clear();

    wgt.addWidget(fresh);

    EXPECT_EQ(wgt._widgets.count(), 1);
    EXPECT_EQ(wgt._widgets.first(), fresh);
    EXPECT_EQ(stale->parentWidget(), &wgt); // stale child still around
}

TEST(CExpWgtTest, WidgetShowInWindow_NoProperty_ReturnsTrue)
{
    // B10: no property → default true
    QWidget w;
    EXPECT_TRUE(CExpWgt::widgetShowInWindow(&w));
}

TEST(CExpWgtTest, WidgetShowInWindow_PropertyFalse_ReturnsFalse)
{
    // B11: property set false → false
    QWidget w;
    w.setProperty(WidgetShowInVerWindow, false);
    EXPECT_FALSE(CExpWgt::widgetShowInWindow(&w));
}

TEST(CExpWgtTest, WidgetMarginInWindow_NoProperty_ReturnsDefault)
{
    // B12: no property → default QMargins(20,0,20,0)
    QWidget w;
    EXPECT_EQ(CExpWgt::widgetMarginInWindow(&w), QMargins(20, 0, 20, 0));
}

TEST(CExpWgtTest, WidgetMarginInWindow_PropertySet_ReturnsCustom)
{
    // B13: property set → custom margins
    QWidget w;
    w.setProperty(WidgetMarginInVerWindow, QVariant::fromValue(QMargins(5, 10, 15, 20)));
    EXPECT_EQ(CExpWgt::widgetMarginInWindow(&w), QMargins(5, 10, 15, 20));
}

TEST(CExpWgtTest, WidgetAlignInWindow_NoProperty_ReturnsAlignLeft)
{
    // B14: no property → default Qt::AlignLeft
    QWidget w;
    EXPECT_EQ(int(CExpWgt::widgetAlignInWindow(&w)), int(Qt::AlignLeft));
}

TEST(CExpWgtTest, WidgetAlignInWindow_PropertySet_ReturnsCustom)
{
    // B15: property set → custom alignment
    QWidget w;
    w.setProperty(WidgetAlignInVerWindow, int(Qt::AlignCenter));
    EXPECT_EQ(int(CExpWgt::widgetAlignInWindow(&w)), int(Qt::AlignCenter));
}

TEST(CExpWgtTest, ShowEvent_Grab_NoCrash)
{
    // B16: showEvent/paintEvent via grab()
    CExpWgt wgt;
    wgt.resize(200, 300);
    wgt.show();
    QPixmap pix = wgt.grab();
    EXPECT_FALSE(pix.isNull());
    wgt.hide();
}

// =========================================================================
// CComBoxSettingWgt
// =========================================================================

TEST(CComBoxSettingWgtTest, Constructor_WithText_LabelVisible)
{
    // B17: constructor with text → label visible
    CComBoxSettingWgt wgt("TestLabel");
    EXPECT_FALSE(wgt._lab->isHidden());
}

TEST(CComBoxSettingWgtTest, Constructor_EmptyText_LabelHidden)
{
    // B18: constructor with empty text → label hidden
    CComBoxSettingWgt wgt("");
    EXPECT_TRUE(wgt._lab->isHidden());
}

TEST(CComBoxSettingWgtTest, ComboBox_DefaultNonNull)
{
    // B19: comboBox() returns non-null default
    CComBoxSettingWgt wgt("Test");
    EXPECT_NE(wgt.comboBox(), nullptr);
}

TEST(CComBoxSettingWgtTest, SetText_Empty_HidesLabel)
{
    // B20: setText(empty) hides label
    CComBoxSettingWgt wgt("Initial");
    wgt.setText("");
    EXPECT_TRUE(wgt._lab->isHidden());
}

TEST(CComBoxSettingWgtTest, SetText_NonEmpty_ShowsLabel)
{
    // B21: setText(non-empty) shows label
    CComBoxSettingWgt wgt("");
    wgt.setText("NewText");
    EXPECT_FALSE(wgt._lab->isHidden());
    EXPECT_EQ(wgt._lab->text(), QString("NewText"));
}

TEST(CComBoxSettingWgtTest, SetComboBox_ReplacesOldCombo)
{
    // B22: setComboBox replaces old combo
    CComBoxSettingWgt wgt("Test");
    QComboBox *oldBox = wgt.comboBox();
    EXPECT_NE(oldBox, nullptr);

    QComboBox *newBox = new QComboBox();
    wgt.setComboBox(newBox);
    EXPECT_EQ(wgt.comboBox(), newBox);
}

#endif // TEST_ATTRHELPERS_ITEM
