// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ProgressLayout (issue V-6403).
 *
 * Source: src/widgets/progresslayout.cpp
 *
 * Target methods:
 *   ProgressLayout(QWidget*), setRange, setProgressValue,
 *   setText, setSubText, setAutoFillSubText,
 *   showInCenter, delayClose, reset
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTimer>
#include <QEventLoop>
#include <DWidget>

#define protected public
#define private public
#include "widgets/progresslayout.h"
#undef protected
#undef private

// ---- ProgressLayout constructor ----

TEST(ProgressLayoutTest, ConstructDoesNotCrash)
{
    ProgressLayout layout;
    EXPECT_EQ(layout.m_start, 0);
    EXPECT_EQ(layout.m_end, 0);
}

TEST(ProgressLayoutTest, ConstructorCreatesProgressbar)
{
    ProgressLayout layout;
    EXPECT_NE(layout.m_progressbar, nullptr);
    EXPECT_NE(layout.m_label, nullptr);
    EXPECT_NE(layout.m_progressLabel, nullptr);
}

// ---- setRange ----

TEST(ProgressLayoutSetRange, NormalRange)
{
    ProgressLayout layout;
    layout.setRange(10, 100);
    EXPECT_EQ(layout.m_start, 10);
    EXPECT_EQ(layout.m_end, 100);
    EXPECT_EQ(layout.m_progressbar->minimum(), 10);
    EXPECT_EQ(layout.m_progressbar->maximum(), 100);
    EXPECT_EQ(layout.m_progressbar->value(), 10);
}

TEST(ProgressLayoutSetRange, BoundaryStartGreaterThanEnd)
{
    ProgressLayout layout;
    layout.setRange(100, 10);
    // m_start / m_end store the raw values passed in
    EXPECT_EQ(layout.m_start, 100);
    EXPECT_EQ(layout.m_end, 10);
    // QProgressBar clamps: setMinimum(100) raises maximum to 100,
    // then setMaximum(10) lowers minimum to 10.
    EXPECT_EQ(layout.m_progressbar->minimum(), 10);
    EXPECT_EQ(layout.m_progressbar->maximum(), 10);
}

TEST(ProgressLayoutSetRange, NegativeValues)
{
    ProgressLayout layout;
    layout.setRange(-10, -1);
    EXPECT_EQ(layout.m_start, -10);
    EXPECT_EQ(layout.m_end, -1);
}

// ---- setProgressValue ----

TEST(ProgressLayoutSetProgressValue, InRangeValue)
{
    ProgressLayout layout;
    layout.setRange(0, 100);
    layout.setProgressValue(50);
    EXPECT_EQ(layout.m_progressbar->value(), 50);
}

TEST(ProgressLayoutSetProgressValue, OutOfRangeHighValue)
{
    ProgressLayout layout;
    layout.setRange(0, 100);
    // setRange sets value to start (0). Qt6 QProgressBar::setValue
    // rejects values outside [min, max], so value stays at 0.
    layout.setProgressValue(200);
    EXPECT_EQ(layout.m_progressbar->value(), 0);
}

TEST(ProgressLayoutSetProgressValue, OutOfRangeLowValue)
{
    ProgressLayout layout;
    layout.setRange(10, 100);
    // setRange sets value to start (10). Qt6 QProgressBar::setValue
    // rejects values below minimum, so value stays at 10.
    layout.setProgressValue(-50);
    EXPECT_EQ(layout.m_progressbar->value(), 10);
}

TEST(ProgressLayoutSetProgressValue, AutoFillSubTextShowsProgress)
{
    ProgressLayout layout;
    layout.setRange(0, 100);
    layout.setAutoFillSubText(true);
    layout.setProgressValue(50);
    EXPECT_EQ(layout.m_progressLabel->text(),
              QString("50/100"));
}

TEST(ProgressLayoutSetProgressValue, ManualSubTextShowsCustomText)
{
    ProgressLayout layout;
    layout.setRange(0, 100);
    layout.setSubText("Custom text");
    layout.setProgressValue(50);
    EXPECT_EQ(layout.m_progressLabel->text(), "Custom text");
}

// ---- setText ----

TEST(ProgressLayoutSetText, SetsLabelText)
{
    ProgressLayout layout;
    layout.setText("Loading");
    EXPECT_EQ(layout.m_label->text(), "Loading");
}

TEST(ProgressLayoutSetText, EmptyString)
{
    ProgressLayout layout;
    layout.setText("Hello");
    layout.setText("");
    EXPECT_TRUE(layout.m_label->text().isEmpty());
}

// ---- setSubText ----

TEST(ProgressLayoutSetSubText, SetsSubTextAndDisablesAutoFill)
{
    ProgressLayout layout;
    layout.setSubText("my subtext");
    EXPECT_FALSE(layout.m_autoFillSubText);
    EXPECT_EQ(layout.m_subText, "my subtext");
    EXPECT_EQ(layout.m_progressLabel->text(), "my subtext");
}

// ---- setAutoFillSubText ----

TEST(ProgressLayoutSetAutoFillSubText, EnableAutoFill)
{
    ProgressLayout layout;
    layout.setAutoFillSubText(false);
    EXPECT_FALSE(layout.m_autoFillSubText);
    layout.setAutoFillSubText(true);
    EXPECT_TRUE(layout.m_autoFillSubText);
}

// ---- showInCenter ----

TEST(ProgressLayoutShowInCenter, ValidWidgetDoesNotCrash)
{
    ProgressLayout layout;
    DWidget w;
    w.resize(400, 300);
    layout.showInCenter(&w);
    SUCCEED();
}

TEST(ProgressLayoutShowInCenter, NullptrSkipped)
{
    // showInCenter posts a deferred lambda that dereferences w;
    // with nullptr this would crash if the event loop runs.
    GTEST_SKIP() << "nullptr would crash in the deferred lambda; "
                    "skipped per issue requirement";
}

// ---- delayClose ----

TEST(ProgressLayoutDelayClose, ClearsSubText)
{
    ProgressLayout layout;
    layout.setSubText("temp");
    ASSERT_EQ(layout.m_subText, "temp");
    layout.delayClose();
    EXPECT_TRUE(layout.m_subText.isEmpty());
}

// ---- reset ----

TEST(ProgressLayoutReset, ResetsToInitialState)
{
    ProgressLayout layout;
    layout.setRange(10, 100);
    layout.setProgressValue(50);
    layout.setText("hello");
    layout.setSubText("custom");

    layout.reset();

    EXPECT_TRUE(layout.m_autoFillSubText);
    EXPECT_TRUE(layout.m_subText.isEmpty());
    EXPECT_TRUE(layout.m_label->text().isEmpty());
    EXPECT_TRUE(layout.m_progressLabel->text().isEmpty());
    EXPECT_EQ(layout.m_progressbar->value(),
              layout.m_progressbar->minimum());
}

TEST(ProgressLayoutReset, AfterResetAutoFillWorksAgain)
{
    ProgressLayout layout;
    layout.setRange(0, 100);
    layout.setSubText("manual");
    ASSERT_FALSE(layout.m_autoFillSubText);

    layout.reset();
    layout.setProgressValue(30);

    EXPECT_EQ(layout.m_progressLabel->text(), QString("30/100"));
}
