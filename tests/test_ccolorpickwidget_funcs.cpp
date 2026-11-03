// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CColorPickWidget methods (issue V-7429)
 *
 * Source: src/frame/AttributesWidgets/ccolorpickwidget.cpp
 *
 * Target method:
 *   setCaller(QWidget *pCaller) — sets _caller
 *   caller() — returns _caller
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWidget>

#define protected public
#define private public
#include "ccolorpickwidget.h"
#undef protected
#undef private

TEST(CColorPickWidgetSetCaller, SetsAndGetsCaller)
{
    CColorPickWidget wgt;
    QWidget *caller = new QWidget();
    wgt.setCaller(caller);
    EXPECT_EQ(wgt.caller(), caller);
    delete caller;
}

TEST(CColorPickWidgetSetCaller, DefaultCallerIsNull)
{
    CColorPickWidget wgt;
    EXPECT_EQ(wgt.caller(), nullptr);
}

TEST(CColorPickWidgetSetCaller, ReplaceCaller)
{
    CColorPickWidget wgt;
    QWidget *caller1 = new QWidget();
    QWidget *caller2 = new QWidget();
    wgt.setCaller(caller1);
    EXPECT_EQ(wgt.caller(), caller1);

    wgt.setCaller(caller2);
    EXPECT_EQ(wgt.caller(), caller2);
    EXPECT_NE(wgt.caller(), caller1);

    delete caller1;
    delete caller2;
}

TEST(CColorPickWidgetSetCaller, SetNullptrClearsCaller)
{
    CColorPickWidget wgt;
    QWidget *caller = new QWidget();
    wgt.setCaller(caller);
    wgt.setCaller(nullptr);
    EXPECT_EQ(wgt.caller(), nullptr);
    delete caller;
}
