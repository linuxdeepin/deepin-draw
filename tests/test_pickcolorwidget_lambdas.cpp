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
 *   6. colorPicked's deferred QTimer::singleShot sub-lambda — flushes the
 *      200 ms timer while the widget is still alive.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QColor>
#include <QSignalSpy>
#include <QTest>

#define protected public
#define private public
#include "pickcolorwidget.h"
#include "colorpickerinterface.h"
#include "publicApi.h"
#undef protected
#undef private

#include "test_helpers.h"

namespace {

// Hue values fed to the slider: a mid-range value and its lower bound.
constexpr int kSliderHueMid = 120;
constexpr int kSliderHueMin = 0;

// The colorPicked lambda schedules a 200 ms singleShot that restores the
// current view's focus. Wait past it so the deferred sub-lambda runs while
// the widget (and the page created by the fixture) are still alive.
constexpr int kDeferredFocusDelayMs = 200;
constexpr int kDeferredFocusFlushMs = kDeferredFocusDelayMs + 50;

const QColor kLabelPickedColor(100, 150, 200);
const QColor kPreviewColor(50, 100, 150);
const char *const kPickedSystemColorName = "#00FF00";

// Keeps the harness DrawBoard page state clean between tests. It also
// guarantees a current view: the matching-uuid test exercises the deferred
// captureFocus() branch, which only runs when CURRENTVIEW is non-null.
class PickColorWidgetLambdaTest : public PageStateGuard
{
};

} // namespace

// ===========================================================================
// m_colorSlider valueChanged lambda — setHue on colorLabel
// ===========================================================================

TEST_F(PickColorWidgetLambdaTest, SliderValueChangedNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorSlider, nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    // Emit valueChanged to trigger the lambda: m_colorLabel->setHue(val)
    emit w.m_colorSlider->valueChanged(kSliderHueMid);
    qApp->processEvents();

    EXPECT_EQ(w.m_colorLabel->m_hue, kSliderHueMid);
}

TEST_F(PickColorWidgetLambdaTest, SliderValueChangedZeroNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorSlider, nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    emit w.m_colorSlider->valueChanged(kSliderHueMin);
    qApp->processEvents();

    EXPECT_EQ(w.m_colorLabel->m_hue, kSliderHueMin);
}

// ===========================================================================
// m_colorLabel pickedColor lambda — setColor
// ===========================================================================

TEST_F(PickColorWidgetLambdaTest, ColorLabelPickedColorNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    // Emit pickedColor to trigger the lambda: this->setColor(color)
    emit w.m_colorLabel->pickedColor(kLabelPickedColor);
    qApp->processEvents();

    // setColor changed the (initially invalid) color and emitted colorChanged.
    EXPECT_EQ(w.color(), kLabelPickedColor);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).value<QColor>(), kLabelPickedColor);
}

TEST_F(PickColorWidgetLambdaTest, ColorLabelPickedInvalidColorNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    // An invalid color equals the initial curColor, so setColor is a no-op.
    emit w.m_colorLabel->pickedColor(QColor());
    qApp->processEvents();

    EXPECT_FALSE(w.color().isValid());
    EXPECT_EQ(spy.count(), 0);
}

// ===========================================================================
// m_colorLabel signalPreViewColor lambda — updateColor
// ===========================================================================

TEST_F(PickColorWidgetLambdaTest, SignalPreViewColorNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::previewedColorChanged);
    ASSERT_TRUE(spy.isValid());

    // Emit signalPreViewColor to trigger the lambda: this->updateColor(color)
    emit w.m_colorLabel->signalPreViewColor(kPreviewColor);
    qApp->processEvents();

    // updateColor with valid color emits previewedColorChanged
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).value<QColor>(), kPreviewColor);
}

TEST_F(PickColorWidgetLambdaTest, SignalPreViewColorInvalidNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_colorLabel, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::previewedColorChanged);
    ASSERT_TRUE(spy.isValid());

    // Invalid color — updateColor shows curColor instead and emits nothing.
    emit w.m_colorLabel->signalPreViewColor(QColor());
    qApp->processEvents();

    EXPECT_EQ(spy.count(), 0);
}

// ===========================================================================
// m_picker mouseRelease lambda — StartPick
// ===========================================================================

TEST_F(PickColorWidgetLambdaTest, PickerMouseReleaseNoCrash)
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

    // Starting the picker must not leave a modal widget behind.
    EXPECT_EQ(QApplication::activeModalWidget(), nullptr);
}

// ===========================================================================
// m_cp colorPicked lambda — with matching/non-matching uuid
// ===========================================================================

TEST_F(PickColorWidgetLambdaTest, ColorPickedNonMatchingUuidNoCrash)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_cp, nullptr);
    ASSERT_NE(w.m_picker, nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());

    // Pretend a pick is in progress so we can verify the lambda resets it.
    w.m_picker->setChecked(true);

    // Emit colorPicked with a non-matching uuid — lambda must skip setColor
    // but still reset the picker.
    emit w.m_cp->colorPicked("wrong-uuid", "#FF0000");
    qApp->processEvents();

    EXPECT_FALSE(w.color().isValid());
    EXPECT_EQ(spy.count(), 0);
    EXPECT_FALSE(w.m_picker->isChecked());
}

TEST_F(PickColorWidgetLambdaTest, ColorPickedMatchingUuidSetsColor)
{
    PickColorWidget w(nullptr);
    ASSERT_NE(w.m_cp, nullptr);
    ASSERT_NE(w.m_picker, nullptr);

    // The fixture guarantees a page; make the deferred branch's precondition
    // explicit.
    ASSERT_NE(getCurView(), nullptr);

    QSignalSpy spy(&w, &PickColorWidget::colorChanged);
    ASSERT_TRUE(spy.isValid());
    w.m_picker->setChecked(true);

    QString pid = QString("%1").arg(qApp->applicationPid());
    const QColor expected(QString::fromLatin1(kPickedSystemColorName));

    // Emit colorPicked with matching uuid — lambda should call setColor.
    emit w.m_cp->colorPicked(pid, QString::fromLatin1(kPickedSystemColorName));
    qApp->processEvents();

    EXPECT_EQ(w.color(), expected);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).value<QColor>(), expected);
    EXPECT_FALSE(w.m_picker->isChecked());

    // Flush the 200 ms deferred focus-restore timer while w is still alive.
    QTest::qWait(kDeferredFocusFlushMs);
}
