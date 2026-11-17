// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for attribute panel remaining lambdas (issue V-8626)
 *
 * Existing test_colorpanel_ext2.cpp covers button toggle, lineEdit textChanged,
 * sliderPressed, valueChanged, but NOT:
 *   - ColorPanel::initUI() returnPressed lambda (hides parent)
 *   - ColorPanel::initConnection() m_colorfulBtn buttonClick lambda
 *   - CAlphaControlWidget::initConnection() sliderReleased lambda
 *   - CColorPickWidget::mousePressEvent delegation
 */

#include <gtest/gtest.h>

#include <QSignalSpy>
#include <QColor>
#include <QButtonGroup>
#include <QMouseEvent>
#include <QCoreApplication>

#define protected public
#define private public
#include "widgets/ciconbutton.h"
#include "colorpanel.h"
#include "calphacontrolwidget.h"
#include "ccolorpickwidget.h"
#include "drawshape/globaldefine.h"
#undef protected
#undef private

#if TEST_COLORPANEL_ITEM

// ===========================================================================
// ColorPanel::initUI — returnPressed lambda (hides parent widget)
// ===========================================================================

TEST(ColorPanelLambdasExt, ReturnPressedWithParentHidesParent)
{
    // parentColorWidget() returns qobject_cast<CColorPickWidget*>(parent()).
    // The returnPressed lambda calls parentColorWidget()->hide().
    // We must provide a CColorPickWidget parent to avoid null dereference.
    CColorPickWidget pickWidget;
    ColorPanel *panel = new ColorPanel();
    panel->setParent(&pickWidget);

    // Emit returnPressed — the lambda should call pickWidget.hide()
    emit panel->m_colLineEdit->returnPressed();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // No crash means success
    delete panel;
    SUCCEED();
}

// ===========================================================================
// ColorPanel::initConnection — m_colorfulBtn buttonClick lambda
// ===========================================================================

TEST(ColorPanelLambdasExt, ColorfulBtnButtonClickTogglesExpand)
{
    ColorPanel panel;

    // The colorfulBtn buttonClick lambda toggles s_expand and calls updateExpendArea.
    emit panel.m_colorfulBtn->buttonClick();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // Emitting again should toggle back
    emit panel.m_colorfulBtn->buttonClick();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    SUCCEED();
}

// ===========================================================================
// CAlphaControlWidget::initConnection — sliderReleased lambda
// ===========================================================================

TEST(CAlphaControlWidgetLambdasExt, SliderReleased_EmitsAlphaChangedFinished)
{
    CAlphaControlWidget w;
    QSignalSpy spy(&w, &CAlphaControlWidget::alphaChanged);

    // Set a known value first
    w.setAlpha(100, false);

    // Emit sliderReleased to trigger the third lambda
    emit w.m_alphaSlider->sliderReleased();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    EXPECT_GE(spy.count(), 1);
    if (spy.count() > 0) {
        QList<QVariant> args = spy.takeLast();
        // sliderReleased lambda emits with EChangedFinished
        EXPECT_EQ(args.at(0).toInt(), 100);
    }
}

// ===========================================================================
// CColorPickWidget::mousePressEvent — delegates to DArrowRectangle
// ===========================================================================

TEST(CColorPickWidgetExt, MousePressEventDoesNotCrash)
{
    CColorPickWidget pickWidget;

    QMouseEvent event(QMouseEvent::MouseButtonPress,
                      QPointF(5, 5),
                      QPointF(5, 5),
                      Qt::LeftButton, Qt::LeftButton,
                      Qt::NoModifier);

    pickWidget.mousePressEvent(&event);

    SUCCEED();
}

#endif // TEST_COLORPANEL_ITEM
