// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QWidget>
#include <DGuiApplicationHelper>

#define protected public
#define private public
#include "application.h"
#undef protected
#undef private

TEST(ApplicationSlots, onThemChangedLightNoCrash)
{
    drawApp->onThemChanged(DGuiApplicationHelper::LightType);
    SUCCEED();
}

TEST(ApplicationSlots, onThemChangedDarkNoCrash)
{
    drawApp->onThemChanged(DGuiApplicationHelper::DarkType);
    SUCCEED();
}

TEST(ApplicationSlots, onThemChangedUnknownNoCrash)
{
    drawApp->onThemChanged(DGuiApplicationHelper::UnknownType);
    SUCCEED();
}

TEST(ApplicationSlots, onFocusChangedBothNullptrNoCrash)
{
    drawApp->onFocusChanged(nullptr, nullptr);
    SUCCEED();
}

TEST(ApplicationSlots, onFocusChangedOldNullptrNoCrash)
{
    QWidget w;
    drawApp->onFocusChanged(nullptr, &w);
    SUCCEED();
}

TEST(ApplicationSlots, onFocusChangedNewNullptrNoCrash)
{
    QWidget w;
    drawApp->onFocusChanged(&w, nullptr);
    SUCCEED();
}

TEST(ApplicationSlots, onFocusChangedBothValidNoCrash)
{
    QWidget w1, w2;
    drawApp->onFocusChanged(&w1, &w2);
    SUCCEED();
}
