// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CAbstractProcessDialog and ProgressDialog
 * (src/widgets/dialog/cprogressdialog.cpp).
 *
 * Issue V-7855: 15 test cases.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QTest>
#include <QTimer>
#include <QWidget>

#define protected public
#define private public
#include "cprogressdialog.h"
#undef protected
#undef private

// =========================================================================
// CAbstractProcessDialog
// =========================================================================

TEST(CAbstractProcessDialogConstructorTest, FixedSize480x80)
{
    CAbstractProcessDialog dlg;
    EXPECT_EQ(dlg.size(), QSize(480, 80));
}

TEST(CAbstractProcessDialogConstructorTest, WindowModal)
{
    CAbstractProcessDialog dlg;
    EXPECT_EQ(dlg.windowModality(), Qt::WindowModal);
}

TEST(CAbstractProcessDialogSetTitleTest, UpdatesTitle)
{
    CAbstractProcessDialog dlg;
    dlg.setTitle("Exporting...");
    EXPECT_EQ(dlg._titleLabel->text(), QString("Exporting..."));
}

TEST(CAbstractProcessDialogSetProcessTest, NormalValue)
{
    CAbstractProcessDialog dlg;
    dlg.setProcess(50);
    EXPECT_EQ(dlg._progressBar->value(), 50);
}

TEST(CAbstractProcessDialogSetProcessTest, OverLimitIgnored)
{
    CAbstractProcessDialog dlg;
    // Set a normal value first so we can observe the ignore behavior.
    dlg.setProcess(50);
    EXPECT_EQ(dlg._progressBar->value(), 50);

    // Over-limit value: DProgressBar ignores it, value stays unchanged.
    dlg.setProcess(200);
    EXPECT_EQ(dlg._progressBar->value(), 50);
}

TEST(CAbstractProcessDialogSetProcessTest, BelowMinimumIgnored)
{
    CAbstractProcessDialog dlg;
    // Set a normal value first so we can observe the ignore behavior.
    dlg.setProcess(50);
    EXPECT_EQ(dlg._progressBar->value(), 50);

    // Below-minimum value: DProgressBar ignores it, value stays unchanged (低于下限忽略).
    dlg.setProcess(-50);
    EXPECT_EQ(dlg._progressBar->value(), 50);
}

TEST(CAbstractProcessDialogPaintEventTest, DoesNotCrash)
{
    CAbstractProcessDialog dlg;
    dlg.show();
    // Render synchronously instead of waiting for window exposure: exposure is
    // not guaranteed for every platform/plugin, while grab() renders the widget
    // directly and therefore always delivers a paint event.
    EXPECT_FALSE(dlg.grab().isNull());
    SUCCEED();
}

// =========================================================================
// ProgressDialog
// =========================================================================

TEST(ProgressDialogConstructorTest, AccessibleName)
{
    ProgressDialog dlg;
    EXPECT_EQ(dlg.accessibleName(), QString("ProgressDialog"));
}

TEST(ProgressDialogConstructorTest, FixedSize400x120)
{
    ProgressDialog dlg;
    EXPECT_EQ(dlg.size(), QSize(400, 120));
}

TEST(ProgressDialogConstructorTest, EmptyText)
{
    ProgressDialog dlg;
    EXPECT_TRUE(dlg._titleLabel->text().isEmpty());
}

TEST(ProgressDialogConstructorTest, InitialText)
{
    ProgressDialog dlg("Loading...");
    EXPECT_EQ(dlg._titleLabel->text(), QString("Loading..."));
}

TEST(ProgressDialogSetTextTest, UpdatesText)
{
    ProgressDialog dlg;
    dlg.setText("Saving...");
    EXPECT_EQ(dlg._titleLabel->text(), QString("Saving..."));
}

TEST(ProgressDialogSetProcessTest, RangeAndValue)
{
    ProgressDialog dlg;
    dlg.setProcess(30, 200);
    EXPECT_EQ(dlg.m_progressBar->minimum(), 0);
    EXPECT_EQ(dlg.m_progressBar->maximum(), 200);
    EXPECT_EQ(dlg.m_progressBar->value(), 30);
}

TEST(ProgressDialogExecTest, NoParentNonBlocking)
{
    ProgressDialog dlg("Test");
    // exec() with no parent: shows dialog and returns 0 immediately.
    int ret = dlg.exec();
    EXPECT_EQ(ret, 0);
    dlg.close();
}

TEST(ProgressDialogExecTest, WithParentNonBlocking)
{
    QWidget parentWidget;
    parentWidget.resize(800, 600);
    parentWidget.show();
    ASSERT_TRUE(QTest::qWaitForWindowExposed(&parentWidget));

    ProgressDialog dlg("Test", &parentWidget);
    // exec() with parent: centers on parent window, shows, returns 0.
    int ret = dlg.exec();
    EXPECT_EQ(ret, 0);
    dlg.close();
}
