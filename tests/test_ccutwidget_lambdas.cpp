// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CCutWidget lambda callbacks (issue V-9255 area 2)
 *
 * Source: src/frame/AttributesWidgets/private/ccutwidget.cpp
 *
 * Target lambdas:
 *   m_widthEdit editingFinished  — maxPicSize clamp + Radio[] match → cut_free
 *   m_heightEdit editingFinished — maxPicSize clamp + Radio[] match → cut_free
 *   m_doneBtn clicked            — clearFocus + emit finshed(true)
 *   m_cancelBtn clicked          — emit finshed(false)
 *   m_SizeAddAction triggered    — width/height focus path, >4096 clamp
 *   m_SizeReduceAction triggered — width/height focus path, <10 clamp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QLineEdit>

#define protected public
#define private public
#include "ccutwidget.h"
#include "drawshape/globaldefine.h"
#include "application.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// m_widthEdit editingFinished lambda
// =========================================================================

TEST(CCutWidgetLambdas, WidthEdit_ExceedsMaxPicSize_Clamped)
{
    CCutWidget w;
    QSize maxSize = Application::drawApplication()->maxPicSize();

    // Set width beyond maxPicSize and height to a valid value
    w.m_widthEdit->setText(QString::number(maxSize.width() + 1000));
    w.m_heightEdit->setText("100");
    emit w.m_widthEdit->editingFinished();

    // The lambda clamps newWidth to maxSize.width() and writes it back
    int clampedWidth = w.m_widthEdit->text().trimmed().toInt();
    EXPECT_LE(clampedWidth, maxSize.width());
}

TEST(CCutWidgetLambdas, WidthEdit_ValidRatio_MatchesRadioTable)
{
    CCutWidget w;
    // 1:1 ratio — width==height matches Radio[cut_1_1] = 1.0
    w.m_widthEdit->setText("200");
    w.m_heightEdit->setText("200");
    emit w.m_widthEdit->editingFinished();

    // After editingFinished, setCutSize is called with (200, 100)
    // and setCutType(cut_free) is called
    EXPECT_EQ(w.cutSize().width(), 200);
    EXPECT_EQ(w.cutSize().height(), 200);
}

// =========================================================================
// m_heightEdit editingFinished lambda
// =========================================================================

TEST(CCutWidgetLambdas, HeightEdit_ExceedsMaxPicSize_Clamped)
{
    CCutWidget w;
    QSize maxSize = Application::drawApplication()->maxPicSize();

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText(QString::number(maxSize.height() + 1000));
    emit w.m_heightEdit->editingFinished();

    int clampedHeight = w.m_heightEdit->text().trimmed().toInt();
    EXPECT_LE(clampedHeight, maxSize.height());
}

TEST(CCutWidgetLambdas, HeightEdit_NormalValue_SetsCutSize)
{
    CCutWidget w;

    w.m_widthEdit->setText("300");
    w.m_heightEdit->setText("200");
    emit w.m_heightEdit->editingFinished();

    EXPECT_EQ(w.cutSize().width(), 300);
    EXPECT_EQ(w.cutSize().height(), 200);
}

// =========================================================================
// m_doneBtn clicked lambda — emit finshed(true)
// =========================================================================

TEST(CCutWidgetLambdas, DoneBtn_Clicked_EmitsFinshedTrue)
{
    CCutWidget w;

    QSignalSpy spy(&w, &CCutWidget::finshed);
    ASSERT_TRUE(spy.isValid());

    emit w.m_doneBtn->clicked();
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.size(), 1);
    EXPECT_TRUE(args.at(0).toBool());
}

// =========================================================================
// m_cancelBtn clicked lambda — emit finshed(false)
// =========================================================================

TEST(CCutWidgetLambdas, CancelBtn_Clicked_EmitsFinshedFalse)
{
    CCutWidget w;

    QSignalSpy spy(&w, &CCutWidget::finshed);
    ASSERT_TRUE(spy.isValid());

    emit w.m_cancelBtn->clicked();
    qApp->processEvents();

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.size(), 1);
    EXPECT_FALSE(args.at(0).toBool());
}

// =========================================================================
// m_SizeAddAction triggered lambda — width focus path
// =========================================================================

TEST(CCutWidgetLambdas, SizeAddAction_WidthFocus_IncrementsWidth)
{
    CCutWidget w;

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText("200");
    // Give width edit focus so the lambda takes the width branch
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_widthEdit->lineEdit()->hasFocus());

    emit w.m_SizeAddAction->triggered(false);

    int newWidth = w.m_widthEdit->text().trimmed().toInt();
    EXPECT_EQ(newWidth, 101);
}

TEST(CCutWidgetLambdas, SizeAddAction_WidthAtMax_DoesNotIncrement)
{
    CCutWidget w;

    w.m_widthEdit->setText("4096");
    w.m_heightEdit->setText("200");
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_widthEdit->lineEdit()->hasFocus());

    emit w.m_SizeAddAction->triggered(false);

    // 4096 + 1 = 4097 > 4096 → lambda returns early, text stays 4096
    int width = w.m_widthEdit->text().trimmed().toInt();
    EXPECT_EQ(width, 4096);
}

// =========================================================================
// m_SizeAddAction triggered lambda — height focus path
// =========================================================================

TEST(CCutWidgetLambdas, SizeAddAction_HeightFocus_IncrementsHeight)
{
    CCutWidget w;

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText("200");
    // Give height edit focus
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->clearFocus();
    qApp->processEvents();
    w.m_heightEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_heightEdit->lineEdit()->hasFocus());

    emit w.m_SizeAddAction->triggered(false);

    int newHeight = w.m_heightEdit->text().trimmed().toInt();
    EXPECT_EQ(newHeight, 201);
}

TEST(CCutWidgetLambdas, SizeAddAction_HeightAtMax_DoesNotIncrement)
{
    CCutWidget w;

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText("4096");
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->clearFocus();
    qApp->processEvents();
    w.m_heightEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_heightEdit->lineEdit()->hasFocus());

    emit w.m_SizeAddAction->triggered(false);

    int height = w.m_heightEdit->text().trimmed().toInt();
    EXPECT_EQ(height, 4096);
}

// =========================================================================
// m_SizeReduceAction triggered lambda — width focus path
// =========================================================================

TEST(CCutWidgetLambdas, SizeReduceAction_WidthFocus_DecrementsWidth)
{
    CCutWidget w;

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText("200");
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_widthEdit->lineEdit()->hasFocus());

    emit w.m_SizeReduceAction->triggered(false);

    int newWidth = w.m_widthEdit->text().trimmed().toInt();
    EXPECT_EQ(newWidth, 99);
}

TEST(CCutWidgetLambdas, SizeReduceAction_WidthAtMin_DoesNotDecrement)
{
    CCutWidget w;

    w.m_widthEdit->setText("10");
    w.m_heightEdit->setText("200");
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_widthEdit->lineEdit()->hasFocus());

    emit w.m_SizeReduceAction->triggered(false);

    // 10 - 1 = 9 < 10 → lambda returns early, text stays 10
    int width = w.m_widthEdit->text().trimmed().toInt();
    EXPECT_EQ(width, 10);
}

// =========================================================================
// m_SizeReduceAction triggered lambda — height focus path
// =========================================================================

TEST(CCutWidgetLambdas, SizeReduceAction_HeightFocus_DecrementsHeight)
{
    CCutWidget w;

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText("200");
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->clearFocus();
    qApp->processEvents();
    w.m_heightEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_heightEdit->lineEdit()->hasFocus());

    emit w.m_SizeReduceAction->triggered(false);

    int newHeight = w.m_heightEdit->text().trimmed().toInt();
    EXPECT_EQ(newHeight, 199);
}

TEST(CCutWidgetLambdas, SizeReduceAction_HeightAtMin_DoesNotDecrement)
{
    CCutWidget w;

    w.m_widthEdit->setText("100");
    w.m_heightEdit->setText("10");
    w.show();
    qApp->processEvents();
    w.m_widthEdit->lineEdit()->clearFocus();
    qApp->processEvents();
    w.m_heightEdit->lineEdit()->setFocus(Qt::OtherFocusReason);
    qApp->processEvents();
    ASSERT_TRUE(w.m_heightEdit->lineEdit()->hasFocus());

    emit w.m_SizeReduceAction->triggered(false);

    int height = w.m_heightEdit->text().trimmed().toInt();
    EXPECT_EQ(height, 10);
}
