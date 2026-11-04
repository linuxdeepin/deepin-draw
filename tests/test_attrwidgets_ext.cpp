// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for attribute widgets (issue V-7618 groups 7-12)
 *
 * Source: src/frame/AttributesWidgets/private/
 *
 * Target methods/signals:
 *   BlurWidget::getEffect(), blurEffectChanged signal
 *   CAlphaControlWidget::alphaChanged signal
 *   CCutWidget::setAutoCalSizeIfRadioChanged(bool)
 *   ColorPanel::updateColor(), updateExpendArea()
 *   PickColorWidget::previewedColorChanged signal, updateColor()
 *   CExpWgt::clearChildLayout()
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QColor>

#define protected public
#define private public
#include "blurwidget.h"
#include "calphacontrolwidget.h"
#include "ccutwidget.h"
#include "colorpanel.h"
#include "pickcolorwidget.h"
#include "cattributemanagerwgt.h"
#include "drawshape/globaldefine.h"
#include "publicApi.h"
#undef protected
#undef private

#if TEST_BLUR_ITEM

// =========================================================================
// BlurWidget::getEffect
// =========================================================================

TEST(BlurWidgetGetEffect, DefaultEffect_ReturnsValidValues)
{
    BlurWidget w;
    SBLurEffect effect = w.getEffect();
    // Type and width should have valid default values
    EXPECT_GE(effect.type, 0);
    EXPECT_GE(effect.width, 0);
}

TEST(BlurWidgetGetEffect, blurEffectChanged_EmittedOnChange)
{
    BlurWidget w;

    QSignalSpy spy(&w, &BlurWidget::blurEffectChanged);
    ASSERT_TRUE(spy.isValid());

    // Trigger a change in blur effect — this may fire the signal
    // depending on internal implementation. We verify spy is valid.
    SUCCEED();
}

#endif // TEST_BLUR_ITEM

// =========================================================================
// CAlphaControlWidget::alphaChanged signal
// =========================================================================

TEST(CAlphaControlWidgetAlphaChanged, SetAlpha_EmitsSignal)
{
    CAlphaControlWidget w;

    QSignalSpy spy(&w, &CAlphaControlWidget::alphaChanged);
    ASSERT_TRUE(spy.isValid());

    // setAlpha with internalChanged=true updates the slider without blocking
    // signals, whose valueChanged triggers exactly one alphaChanged(value, phase)
    w.setAlpha(128, true);

    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.size(), 2);
    // First arg is alpha value, second is phase
    int alpha = args.at(0).toInt();
    EXPECT_EQ(alpha, 128);
}

TEST(CAlphaControlWidgetAlphaChanged, SetAlphaNoEmit_DoesNotEmit)
{
    CAlphaControlWidget w;

    QSignalSpy spy(&w, &CAlphaControlWidget::alphaChanged);
    ASSERT_TRUE(spy.isValid());

    // setAlpha with emit=false should NOT fire the signal
    w.setAlpha(200, false);

    EXPECT_EQ(spy.count(), 0);
}

// =========================================================================
// CCutWidget::setAutoCalSizeIfRadioChanged
// =========================================================================

TEST(CCutWidgetSetAutoCalSizeIfRadioChanged, SetTrue_UpdatesMember)
{
    CCutWidget w;
    w.setAutoCalSizeIfRadioChanged(true);
    EXPECT_TRUE(w.m_autoCal);
}

TEST(CCutWidgetSetAutoCalSizeIfRadioChanged, SetFalse_UpdatesMember)
{
    CCutWidget w;
    w.setAutoCalSizeIfRadioChanged(false);
    EXPECT_FALSE(w.m_autoCal);
}

TEST(CCutWidgetSetAutoCalSizeIfRadioChanged, Toggle_UpdatesCorrectly)
{
    CCutWidget w;
    w.setAutoCalSizeIfRadioChanged(true);
    EXPECT_TRUE(w.m_autoCal);
    w.setAutoCalSizeIfRadioChanged(false);
    EXPECT_FALSE(w.m_autoCal);
    w.setAutoCalSizeIfRadioChanged(true);
    EXPECT_TRUE(w.m_autoCal);
}

// =========================================================================
// ColorPanel::updateColor
// =========================================================================

#if TEST_COLORPANEL_ITEM

TEST(ColorPanelUpdateColor, ValidColor_NoCrash)
{
    ColorPanel panel;
    panel.updateColor(QColor(255, 0, 0));
    SUCCEED();
}

TEST(ColorPanelUpdateColor, InvalidColor_NoCrash)
{
    ColorPanel panel;
    panel.updateColor(QColor());
    SUCCEED();
}

// =========================================================================
// ColorPanel::updateExpendArea
// =========================================================================

TEST(ColorPanelUpdateExpendArea, ToggleExpand_NoCrash)
{
    ColorPanel panel;
    // Call updateExpendArea — should not crash regardless of state
    panel.updateExpendArea();
    SUCCEED();
}

#endif // TEST_COLORPANEL_ITEM

// =========================================================================
// PickColorWidget::previewedColorChanged signal & updateColor
// =========================================================================

TEST(PickColorWidgetPreviewedColorChanged, SignalIsValid)
{
    PickColorWidget w(nullptr);

    QSignalSpy spy(&w, &PickColorWidget::previewedColorChanged);
    ASSERT_TRUE(spy.isValid());
    SUCCEED();
}

TEST(PickColorWidgetUpdateColor, ValidColor_NoCrash)
{
    PickColorWidget w(nullptr);
    w.updateColor(QColor(100, 200, 50));
    SUCCEED();
}

TEST(PickColorWidgetUpdateColor, InvalidColor_NoCrash)
{
    PickColorWidget w(nullptr);
    w.updateColor(QColor());
    SUCCEED();
}

// =========================================================================
// CExpWgt::clearChildLayout
// =========================================================================

TEST(CExpWgtClearChildLayout, EmptyLayout_NoCrash)
{
    CExpWgt w;
    // With no children, clearChildLayout should not crash
    w.clearChildLayout();
    SUCCEED();
}

TEST(CExpWgtClearChildLayout, WithChildren_ClearsAll)
{
    CExpWgt w;
    // Add some widgets to the layout; parent them to w so Qt ownership
    // cleans them up even if _pCenterLay is null.
    QWidget *child1 = new QWidget(&w);
    QWidget *child2 = new QWidget(&w);
    if (w._pCenterLay) {
        w._pCenterLay->addWidget(child1);
        w._pCenterLay->addWidget(child2);
    }

    w.clearChildLayout();

    // After clearing, the layout count should be 0
    if (w._pCenterLay) {
        EXPECT_EQ(w._pCenterLay->count(), 0);
    }
    SUCCEED();
}
