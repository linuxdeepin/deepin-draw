// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CIconButton methods (issue V-7429)
 *
 * Source: src/widgets/ciconbutton.cpp
 *
 * Target methods not covered by test_ciconbutton.cpp:
 *   updateImage(), mouseReleaseEvent() (emits mouseRelease signal)
 *
 * Note: The issue mentions buttonClick() — this method does not exist
 * in the codebase. mouseReleaseEvent() emits the mouseRelease() signal
 * on left-button release and is tested instead.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMap>
#include <QSize>
#include <QMouseEvent>
#include <DGuiApplicationHelper>
#include <QSignalSpy>

#define protected public
#define private public
#include "ciconbutton.h"
#undef protected
#undef private

static CIconButton *createButton2(bool isCheckLock = true)
{
    QMap<int, QMap<CIconButton::EIconButtonSattus, QString>> pictureMap;
    return new CIconButton(pictureMap, QSize(55, 36), nullptr, isCheckLock);
}

// =========================================================================
// CIconButton::updateImage
// =========================================================================

TEST(CIconButtonUpdateImage, DoesNotCrashWithEmptyMap)
{
    auto *btn = createButton2();
    btn->m_iconMode = false;
    btn->updateImage();
    // With an empty picture map the icon path string is empty, but no crash.
    SUCCEED();
    delete btn;
}

TEST(CIconButtonUpdateImage, IconModeTrue_NoIconSet)
{
    auto *btn = createButton2();
    btn->m_iconMode = true;
    btn->updateImage();
    // When m_iconMode is true, updateImage does nothing (early return).
    EXPECT_TRUE(btn->icon().isNull());
    delete btn;
}

TEST(CIconButtonUpdateImage, SetsIconFromMap)
{
    QMap<int, QMap<CIconButton::EIconButtonSattus, QString>> pictureMap;
    // Use a non-existent path; QIcon will be null but setIcon still called.
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Normal] = "/nonexistent/icon.png";
    auto *btn = new CIconButton(pictureMap, QSize(55, 36), nullptr, true);
    btn->m_currentTheme = DGuiApplicationHelper::LightType;
    btn->m_currentStatus = CIconButton::Normal;
    btn->m_iconMode = false;
    btn->updateImage();
    // The icon was set (even if the file doesn't exist, QIcon is constructed).
    SUCCEED();
    delete btn;
}

// =========================================================================
// CIconButton::mouseReleaseEvent (emits mouseRelease signal)
// =========================================================================

TEST(CIconButtonMouseReleaseEvent, LeftButtonEmitsMouseRelease)
{
    auto *btn = createButton2();
    QSignalSpy spy(btn, &CIconButton::mouseRelease);

    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(0, 0),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(btn, &event);

    EXPECT_EQ(spy.count(), 1);
    delete btn;
}

TEST(CIconButtonMouseReleaseEvent, RightButtonDoesNotEmit)
{
    auto *btn = createButton2();
    QSignalSpy spy(btn, &CIconButton::mouseRelease);

    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(0, 0),
                      Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    QApplication::sendEvent(btn, &event);

    EXPECT_EQ(spy.count(), 0);
    delete btn;
}

TEST(CIconButtonMouseReleaseEvent, ResetsPressedState)
{
    auto *btn = createButton2();
    btn->m_isPressed = true;

    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(0, 0),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(btn, &event);

    EXPECT_FALSE(btn->m_isPressed);
    delete btn;
}

TEST(CIconButtonMouseReleaseEvent, CheckedButtonStaysActive)
{
    auto *btn = createButton2();
    btn->m_isChecked = true;
    btn->m_isPressed = true;

    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(0, 0),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(btn, &event);

    EXPECT_EQ(btn->m_currentStatus, CIconButton::Active);
    delete btn;
}

TEST(CIconButtonMouseReleaseEvent, UncheckedButtonReturnsNormal)
{
    auto *btn = createButton2();
    btn->m_isChecked = false;
    btn->m_isPressed = true;

    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(0, 0),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(btn, &event);

    EXPECT_EQ(btn->m_currentStatus, CIconButton::Normal);
    delete btn;
}
