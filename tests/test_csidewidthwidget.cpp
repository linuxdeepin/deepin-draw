// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSideWidthWidget (issue V-6403).
 *
 * Source: src/widgets/csidewidthwidget.cpp
 *
 * Target methods:
 *   CSideWidthWidget(DWidget*), menuComboBox(), setText, setSpace,
 *   setWidth, setVaild, sizeHint()
 *   Signal: widthChanged(int, bool)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QSize>

#define protected public
#define private public
#include "widgets/csidewidthwidget.h"
#undef protected
#undef private

// ---- CSideWidthWidget constructor ----

TEST(CSideWidthWidgetTest, ConstructDoesNotCrash)
{
    CSideWidthWidget w;
    EXPECT_EQ(w.m_comboxHeight, 28);
}

TEST(CSideWidthWidgetTest, ConstructWithParent)
{
    DWidget parent;
    CSideWidthWidget w(&parent);
    EXPECT_EQ(w.parentWidget(), &parent);
}

// ---- menuComboBox ----

TEST(CSideWidthWidgetTest, MenuComboBoxReturnsNonNull)
{
    CSideWidthWidget w;
    EXPECT_NE(w.menuComboBox(), nullptr);
}

TEST(CSideWidthWidgetTest, MenuComboBoxHasSixItems)
{
    CSideWidthWidget w;
    auto *combo = w.menuComboBox();
    ASSERT_NE(combo, nullptr);
    EXPECT_EQ(combo->count(), 6);
    EXPECT_EQ(combo->itemText(0), "0px");
    EXPECT_EQ(combo->itemText(1), "1px");
    EXPECT_EQ(combo->itemText(2), "2px");
    EXPECT_EQ(combo->itemText(3), "4px");
    EXPECT_EQ(combo->itemText(4), "8px");
    EXPECT_EQ(combo->itemText(5), "10px");
}

TEST(CSideWidthWidgetTest, DefaultIndexIsOne)
{
    CSideWidthWidget w;
    EXPECT_EQ(w.menuComboBox()->currentIndex(), 1);
    EXPECT_EQ(w.menuComboBox()->currentText(), "1px");
}

// ---- setText ----

TEST(CSideWidthWidgetSetText, MakesLabelVisibleWithText)
{
    CSideWidthWidget w;
    w.setText("Width");
    // isVisible() is false until the widget is shown, so check
    // that the label is not explicitly hidden instead.
    EXPECT_FALSE(w._textLabel->isHidden());
    EXPECT_EQ(w._textLabel->text(), "Width");
}

TEST(CSideWidthWidgetSetText, EmptyStringHidesLabel)
{
    CSideWidthWidget w;
    w.setText("something");
    w.setText("");
    EXPECT_TRUE(w._textLabel->isHidden());
}

// ---- setSpace ----

TEST(CSideWidthWidgetSetSpace, SetsLayoutSpacing)
{
    CSideWidthWidget w;
    w.setSpace(15);
    EXPECT_EQ(w.m_layout->spacing(), 15);
}

TEST(CSideWidthWidgetSetSpace, ZeroSpacing)
{
    CSideWidthWidget w;
    w.setSpace(0);
    EXPECT_EQ(w.m_layout->spacing(), 0);
}

// ---- setWidth ----

TEST(CSideWidthWidgetSetWidth, ValidWidthSetsCurrentText)
{
    CSideWidthWidget w;
    w.setWidth(2);
    EXPECT_EQ(w.menuComboBox()->currentText(), "2px");
}

TEST(CSideWidthWidgetSetWidth, ValidWidthEmitsSignal)
{
    CSideWidthWidget w;
    QSignalSpy spy(&w, &CSideWidthWidget::widthChanged);
    w.setWidth(4);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toInt(), 4);
}

TEST(CSideWidthWidgetSetWidth, NegativeWidthCallsSetVaildFalse)
{
    CSideWidthWidget w;
    w.setWidth(-1);
    EXPECT_FALSE(w.m_maskLable->isHidden());
    EXPECT_EQ(w.menuComboBox()->currentIndex(), -1);
}

TEST(CSideWidthWidgetSetWidth, ZeroWidthValid)
{
    CSideWidthWidget w;
    QSignalSpy spy(&w, &CSideWidthWidget::widthChanged);
    w.setWidth(0);
    EXPECT_EQ(w.menuComboBox()->currentText(), "0px");
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toInt(), 0);
}

// ---- setVaild ----

TEST(CSideWidthWidgetSetVaild, FalseClearsIndexAndShowsMask)
{
    CSideWidthWidget w;
    w.setVaild(false);
    EXPECT_EQ(w.menuComboBox()->currentIndex(), -1);
    EXPECT_FALSE(w.m_maskLable->isHidden());
}

TEST(CSideWidthWidgetSetVaild, TrueHidesMask)
{
    CSideWidthWidget w;
    w.setVaild(false);
    w.setVaild(true);
    EXPECT_TRUE(w.m_maskLable->isHidden());
}

// ---- sizeHint ----

TEST(CSideWidthWidgetTest, SizeHintIsValid)
{
    CSideWidthWidget w;
    QSize hint = w.sizeHint();
    EXPECT_GE(hint.width(), 0);
    EXPECT_GE(hint.height(), 0);
}
