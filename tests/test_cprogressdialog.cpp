// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ProgressDialog / CAbstractProcessDialog: constructors,
 * setTitle/setProcess state, paintEvent (grab/render) and ProgressDialog::exec.
 * Private members are accessed with the repo-standard #define private public.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

// Include Qt / DTK headers BEFORE #define private to avoid breaking their
// internals (include guards keep them intact inside the guarded include).
#include <DDialog>
#include <DLabel>
#include <DProgressBar>
#include <DWidget>

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QWidget>

#define private public
#define protected public
#include "cprogressdialog.h"
#undef protected
#undef private

// =========================================================================
// CAbstractProcessDialog
// =========================================================================

TEST(CAbstractProcessDialogTest, ConstructorFixesSizeAndModality)
{
    CAbstractProcessDialog dlg;

    EXPECT_EQ(dlg.minimumSize(), QSize(480, 80));
    EXPECT_EQ(dlg.maximumSize(), QSize(480, 80));
    EXPECT_EQ(dlg.windowModality(), Qt::WindowModal);
}

TEST(CAbstractProcessDialogTest, SetTitleUpdatesLabelText)
{
    CAbstractProcessDialog dlg;

    dlg.setTitle("Exporting...");

    ASSERT_NE(dlg._titleLabel, nullptr);
    EXPECT_EQ(dlg._titleLabel->text(), QString("Exporting..."));
}

TEST(CAbstractProcessDialogTest, SetProcessUpdatesBarValue)
{
    CAbstractProcessDialog dlg;

    dlg.setProcess(40);

    ASSERT_NE(dlg._progressBar, nullptr);
    EXPECT_EQ(dlg._progressBar->maximum(), 100);
    EXPECT_EQ(dlg._progressBar->value(), 40);
}

TEST(CAbstractProcessDialogTest, SetProcessBeyondMaxResetsBar)
{
    CAbstractProcessDialog dlg;

    // Qt6 QProgressBar: a value above the maximum resets the bar, so
    // value() reports the reset sentinel -1 (the 0..100 range is kept).
    dlg.setProcess(150);

    EXPECT_EQ(dlg._progressBar->value(), -1);
    EXPECT_EQ(dlg._progressBar->minimum(), 0);
    EXPECT_EQ(dlg._progressBar->maximum(), 100);
}

TEST(CAbstractProcessDialogTest, SetProcessBelowMinIsIgnored)
{
    CAbstractProcessDialog dlg;
    dlg.setProcess(40);

    // Qt6 QProgressBar ignores values below the minimum.
    dlg.setProcess(-5);

    EXPECT_EQ(dlg._progressBar->value(), 40);
}

TEST(CAbstractProcessDialogTest, PaintEventViaGrabDoesNotCrash)
{
    CAbstractProcessDialog dlg;

    QPixmap pix = dlg.grab();

    EXPECT_FALSE(pix.isNull());
}

TEST(CAbstractProcessDialogTest, PaintEventViaRenderDoesNotCrash)
{
    CAbstractProcessDialog dlg;

    QImage image(480, 80, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    dlg.render(&painter);
    painter.end();

    EXPECT_EQ(image.size(), QSize(480, 80));
}

// =========================================================================
// ProgressDialog
// =========================================================================

TEST(ProgressDialogTest, ConstructorSetsAccessibleNameAndSize)
{
    ProgressDialog dlg;

    EXPECT_EQ(dlg.accessibleName(), QString("ProgressDialog"));
    EXPECT_EQ(dlg.minimumSize(), QSize(400, 120));
    EXPECT_EQ(dlg.maximumSize(), QSize(400, 120));
    EXPECT_NE(dlg.m_progressBar, nullptr);
    EXPECT_EQ(dlg.m_progressBar->value(), 0);
}

TEST(ProgressDialogTest, ConstructorWithEmptyTextShowsEmptyLabel)
{
    ProgressDialog dlg("");

    ASSERT_NE(dlg._titleLabel, nullptr);
    EXPECT_TRUE(dlg._titleLabel->text().isEmpty());
}

TEST(ProgressDialogTest, ConstructorWithInitialTextShowsLabel)
{
    ProgressDialog dlg("Saving file...");

    ASSERT_NE(dlg._titleLabel, nullptr);
    EXPECT_EQ(dlg._titleLabel->text(), QString("Saving file..."));
}

TEST(ProgressDialogTest, SetTextUpdatesLabelText)
{
    ProgressDialog dlg;
    dlg.setText("Step 2 of 3");

    EXPECT_EQ(dlg._titleLabel->text(), QString("Step 2 of 3"));
}

TEST(ProgressDialogTest, SetProcessUpdatesRangeAndValue)
{
    ProgressDialog dlg;

    dlg.setProcess(30, 200);

    EXPECT_EQ(dlg.m_progressBar->maximum(), 200);
    EXPECT_EQ(dlg.m_progressBar->value(), 30);
}

TEST(ProgressDialogTest, ExecWithoutParentReturnsZeroAndShows)
{
    ProgressDialog dlg;

    int result = dlg.exec();

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(dlg.isVisible());
    dlg.close();
}

TEST(ProgressDialogTest, ExecWithParentWidgetReturnsZeroAndShows)
{
    QWidget parent;
    parent.setGeometry(100, 100, 800, 600);
    parent.show();
    ProgressDialog dlg("Working", &parent);

    int result = dlg.exec();

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(dlg.isVisible());
}

TEST(ProgressDialogTest, ExecWhenAlreadyVisibleReturnsZero)
{
    ProgressDialog dlg;
    dlg.show();
    ASSERT_TRUE(dlg.isVisible());

    int result = dlg.exec();

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(dlg.isVisible());
    dlg.close();
}
