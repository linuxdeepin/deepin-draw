// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for PickColorWidget constructor lambdas
 * (src/frame/AttributesWidgets/private/pickcolorwidget.cpp).
 *
 * Target lambdas (0-coverage):
 *   1. m_cp colorPicked lambda — checks uuid, calls setColor, resets picker.
 *   2. m_colorSlider valueChanged lambda — calls m_colorLabel->setHue(val).
 *   3. m_colorLabel pickedColor lambda — calls setColor(color).
 *   4. m_colorLabel signalPreViewColor lambda — calls updateColor(color).
 *   5. m_picker mouseRelease lambda — calls m_cp->StartPick(pid).
 *   6. (colorPicked QTimer::singleShot sub-lambda — deferred, not directly
 *      triggerable; covered indirectly.)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QColor>

#define protected public
#define private public
#include "pickcolorwidget.h"
#include "colorpickerinterface.h"
#include "publicApi.h"
#undef protected
#undef private

// ===========================================================================
// m_colorSlider valueChanged lambda — setHue on colorLabel
// ===========================================================================

TEST(PickColorWidgetLambdaTest, SliderValueChangedNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorSlider, nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    // Emit valueChanged to trigger the lambda: m_colorLabel->setHue(val)
    emit w.m_colorSlider->valueChanged(120);
    qApp->processEvents();
    SUCCEED();
}

TEST(PickColorWidgetLambdaTest, SliderValueChangedZeroNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorSlider, nullptr);

    emit w.m_colorSlider->valueChanged(0);
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// m_colorLabel pickedColor lambda — setColor
// ===========================================================================

TEST(PickColorWidgetLambdaTest, ColorLabelPickedColorNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);

    // Emit pickedColor to trigger the lambda: this->setColor(color)
    emit w.m_colorLabel->pickedColor(QColor(100, 150, 200));
    qApp->processEvents();

    // setColor should have been called, which emits colorChanged if changed
    EXPECT_EQ(w.color(), QColor(100, 150, 200));
}

TEST(PickColorWidgetLambdaTest, ColorLabelPickedInvalidColorNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    emit w.m_colorLabel->pickedColor(QColor());
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// m_colorLabel signalPreViewColor lambda — updateColor
// ===========================================================================

TEST(PickColorWidgetLambdaTest, SignalPreViewColorNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::previewedColorChanged);

    // Emit signalPreViewColor to trigger the lambda: this->updateColor(color)
    emit w.m_colorLabel->signalPreViewColor(QColor(50, 100, 150));
    qApp->processEvents();

    // updateColor with valid color emits previewedColorChanged
    EXPECT_EQ(spy.count(), 1);
}

TEST(PickColorWidgetLambdaTest, SignalPreViewColorInvalidNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    // Invalid color — updateColor should use curColor instead, no crash
    emit w.m_colorLabel->signalPreViewColor(QColor());
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// m_picker mouseRelease lambda — StartPick
// ===========================================================================

TEST(PickColorWidgetLambdaTest, PickerMouseReleaseNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_picker, nullptr);

    // Emit mouseRelease to trigger the lambda: m_cp->StartPick(pid).
    // m_cp normally points at the real "com.deepin.Picker" session-bus
    // service, which is D-Bus activatable — calling StartPick on it would
    // actually launch the deepin-picker application on the desktop. Point
    // m_cp at a non-activatable stub service instead so the lambda still
    // runs (keeping the coverage) but the async call only yields a
    // ServiceUnknown error.
    w.m_cp->deleteLater();
    w.m_cp = new ColorPickerInterface("com.deepin.Picker.utstub",
                                      "/com/deepin/Picker",
                                      QDBusConnection::sessionBus(), &w);

    emit w.m_picker->mouseRelease();
    qApp->processEvents();
    SUCCEED();
}

// ===========================================================================
// m_cp colorPicked lambda — with matching/non-matching uuid
// ===========================================================================

TEST(PickColorWidgetLambdaTest, ColorPickedNonMatchingUuidNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_cp, nullptr);

    // Emit colorPicked with a non-matching uuid — lambda should skip setColor
    emit w.m_cp->colorPicked("wrong-uuid", "#FF0000");
    qApp->processEvents();
    SUCCEED();
}

TEST(PickColorWidgetLambdaTest, ColorPickedMatchingUuidSetsColor)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_cp, nullptr);

    QString pid = QString("%1").arg(qApp->applicationPid());

    // Emit colorPicked with matching uuid — lambda should call setColor
    emit w.m_cp->colorPicked(pid, "#00FF00");
    qApp->processEvents();

    EXPECT_EQ(w.color(), QColor("#00FF00"));
}
