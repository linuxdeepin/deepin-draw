// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DZoomMenuComboBox signals (issue V-7618 groups 8-9)
 *
 * Note: DMenuComboBox is disabled (#if 0) in dmenucombobox.h, so only
 * DZoomMenuComboBox signals are tested here.
 *
 * Source: src/widgets/dzoommenucombobox.h/.cpp
 *
 * Target signals:
 *   DZoomMenuComboBox: signalCurrentIndexChanged, signalCurrentTextChanged,
 *                      signalLeftBtnClicked, signalRightBtnClicked
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

// publicApi.h already includes dzoommenucombobox.h with
// protected/private remapped to public (and restores the macros).
#define protected public
#define private public
#include "publicApi.h"
#include "dzoommenucombobox.h"
#undef protected
#undef private

// =========================================================================
// DZoomMenuComboBox signals
// =========================================================================

TEST(DZoomMenuComboBoxSignals, AllSignalsAreValid)
{
    DZoomMenuComboBox combo;

    QSignalSpy spyIndex(&combo, &DZoomMenuComboBox::signalCurrentIndexChanged);
    QSignalSpy spyText(&combo, &DZoomMenuComboBox::signalCurrentTextChanged);
    QSignalSpy spyLeft(&combo, &DZoomMenuComboBox::signalLeftBtnClicked);
    QSignalSpy spyRight(&combo, &DZoomMenuComboBox::signalRightBtnClicked);

    EXPECT_TRUE(spyIndex.isValid());
    EXPECT_TRUE(spyText.isValid());
    EXPECT_TRUE(spyLeft.isValid());
    EXPECT_TRUE(spyRight.isValid());
}

TEST(DZoomMenuComboBoxSignals, SetCurrentIndex_EmitsSignal)
{
    DZoomMenuComboBox combo;
    combo.addItem("100%");
    combo.addItem("200%");
    combo.addItem("50%");

    QSignalSpy spyIndex(&combo, &DZoomMenuComboBox::signalCurrentIndexChanged);
    QSignalSpy spyText(&combo, &DZoomMenuComboBox::signalCurrentTextChanged);
    ASSERT_TRUE(spyIndex.isValid());

    combo.setCurrentIndex(2);

    // setCurrentIndex emits signalCurrentTextChanged and signalCurrentIndexChanged
    // (in that order), each exactly once
    EXPECT_EQ(spyText.count(), 1);
    ASSERT_EQ(spyIndex.count(), 1);
    QList<QVariant> args = spyIndex.takeFirst();
    EXPECT_EQ(args.at(0).toInt(), 2);
}

TEST(DZoomMenuComboBoxSignals, LeftBtnClicked_Emitted)
{
    DZoomMenuComboBox combo;

    QSignalSpy spy(&combo, &DZoomMenuComboBox::signalLeftBtnClicked);
    ASSERT_TRUE(spy.isValid());

    emit combo.signalLeftBtnClicked();

    EXPECT_EQ(spy.count(), 1);
}

TEST(DZoomMenuComboBoxSignals, RightBtnClicked_Emitted)
{
    DZoomMenuComboBox combo;

    QSignalSpy spy(&combo, &DZoomMenuComboBox::signalRightBtnClicked);
    ASSERT_TRUE(spy.isValid());

    emit combo.signalRightBtnClicked();

    EXPECT_EQ(spy.count(), 1);
}
