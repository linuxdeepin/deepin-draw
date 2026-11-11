// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CAlphaControlWidget and ColorPanel zero-coverage methods (V-8216)
 *
 * Target methods:
 *   - CAlphaControlWidget::alpha()
 *   - CAlphaControlWidget::initConnection() lambda (slider signals)
 *   - ColorPanel::initUI() / initConnection() lambdas
 *
 * Source:
 *   src/frame/AttributesWidgets/private/calphacontrolwidget.cpp
 *   src/frame/AttributesWidgets/private/colorpanel.cpp
 */

#include <gtest/gtest.h>

#include <QSignalSpy>
#include <QColor>
#include <QButtonGroup>
#include <QVariant>

#define protected public
#define private public
#include "colorpanel.h"
#include "calphacontrolwidget.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#if TEST_COLORPANEL_ITEM

// =========================================================================
// CAlphaControlWidget::alpha
// =========================================================================

TEST(CAlphaControlWidgetExt2, Alpha_Default_ReturnsZero)
{
    CAlphaControlWidget w;
    // Default slider value is 0
    const int val = w.alpha();
    EXPECT_EQ(val, 0);
}

TEST(CAlphaControlWidgetExt2, Alpha_AfterSetAlpha_ReturnsValue)
{
    CAlphaControlWidget w;
    w.setAlpha(128, false);
    EXPECT_EQ(w.alpha(), 128);
}

TEST(CAlphaControlWidgetExt2, Alpha_AfterSetMax_Returns255)
{
    CAlphaControlWidget w;
    w.setAlpha(255, false);
    EXPECT_EQ(w.alpha(), 255);
}

// =========================================================================
// CAlphaControlWidget::initConnection — slider signal lambdas
// =========================================================================

TEST(CAlphaControlWidgetExt2, InitConnection_ValueChanged_EmitsAlphaChanged)
{
    CAlphaControlWidget w;
    QSignalSpy spy(&w, &CAlphaControlWidget::alphaChanged);

    // Simulate slider value change to trigger the valueChanged lambda
    w.m_alphaSlider->setValue(100);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    EXPECT_GE(spy.count(), 1);
    if (spy.count() > 0) {
        QList<QVariant> args = spy.takeFirst();
        EXPECT_EQ(args.at(0).toInt(), 100);
    }
}

TEST(CAlphaControlWidgetExt2, InitConnection_SliderPressed_EmitsBegin)
{
    CAlphaControlWidget w;
    QSignalSpy spy(&w, &CAlphaControlWidget::alphaChanged);

    // Emit sliderPressed to trigger the first lambda
    emit w.m_alphaSlider->sliderPressed();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    EXPECT_GE(spy.count(), 1);
}

// =========================================================================
// ColorPanel::initUI / initConnection — exercise lambdas
// =========================================================================

TEST(ColorPanelExt2, InitUI_ColorButtonGroup_ToggleChangesColor)
{
    ColorPanel panel;
    QSignalSpy spy(&panel, &ColorPanel::colorChanged);

    // Toggle a color button to trigger the buttonToggled lambda
    auto *btnGroup = panel.m_colorsButtonGroup;
    ASSERT_NE(btnGroup, nullptr);
    auto buttons = btnGroup->buttons();
    ASSERT_GT(buttons.size(), 0);

    buttons.first()->setChecked(true);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // The lambda calls setColor which emits colorChanged
    EXPECT_GE(spy.count(), 1);
}

TEST(ColorPanelExt2, InitConnection_LineEdit_TextChanged_SetsColor)
{
    ColorPanel panel;
    QSignalSpy spy(&panel, &ColorPanel::colorChanged);

    // Set a valid 6-char hex color in the line edit to trigger the textChanged lambda
    panel.m_colLineEdit->setText("ff0000");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // The lambda should have called setColor -> emit colorChanged
    EXPECT_GE(spy.count(), 1);
}

TEST(ColorPanelExt2, InitConnection_AlphaChanged_UpdatesColor)
{
    ColorPanel panel;
    // Set a base color first so alpha can apply
    panel.setColor(QColor(0, 0, 255), true, EChanged);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // Trigger alphaChanged signal from the alpha control widget
    emit panel.m_alphaControlWidget->alphaChanged(128, EChanged);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    QColor c = panel.color();
    EXPECT_EQ(c.alpha(), 128);
}

TEST(ColorPanelExt2, InitConnection_LineEdit_InvalidColor_NoSignal)
{
    ColorPanel panel;
    QSignalSpy spy(&panel, &ColorPanel::colorChanged);

    // An invalid 6-char string should not trigger setColor
    panel.m_colLineEdit->setText("zzzzzz");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    EXPECT_EQ(spy.count(), 0);
}

#endif // TEST_COLORPANEL_ITEM
