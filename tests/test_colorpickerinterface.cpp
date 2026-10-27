// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ColorPickerInterface (issue V-6699).
 *
 * Source: src/service/colorpickerinterface.cpp/h
 *
 * Target methods:
 *   Constructor, destructor, staticInterfaceName, StartPick
 *   Signal colorPicked connection, namespace alias
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "colorpickerinterface.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QSignalSpy>
#include <QObject>
#include <QCoreApplication>
#include <type_traits>

// =========================================================================
// Construction tests
// =========================================================================

TEST(ColorPickerInterface, Construct_WithoutParent_ParentIsNull)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    ColorPickerInterface iface("com.deepin.Picker", "/com/deepin/Picker",
                               QDBusConnection::sessionBus());
    EXPECT_EQ(iface.parent(), nullptr);
    EXPECT_EQ(iface.service(), QStringLiteral("com.deepin.Picker"));
    EXPECT_EQ(iface.path(), QStringLiteral("/com/deepin/Picker"));
}

TEST(ColorPickerInterface, Construct_WithParent_ParentSet)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    QObject parent;
    ColorPickerInterface iface("com.deepin.Picker", "/com/deepin/Picker",
                               QDBusConnection::sessionBus(), &parent);
    EXPECT_EQ(iface.parent(), &parent);
}

TEST(ColorPickerInterface, Construct_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    EXPECT_NO_FATAL_FAILURE({
        ColorPickerInterface iface("com.deepin.Picker", "/com/deepin/Picker",
                                   QDBusConnection::sessionBus());
    });
}

// =========================================================================
// staticInterfaceName
// =========================================================================

TEST(ColorPickerInterface, StaticInterfaceName_ReturnsCorrectValue)
{
    EXPECT_STREQ(ColorPickerInterface::staticInterfaceName(), "com.deepin.Picker");
}

// =========================================================================
// StartPick
// =========================================================================

TEST(ColorPickerInterface, StartPick_NoCrash_ReturnsPendingReply)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    // A method call on the real "com.deepin.Picker" name lets dbus-daemon
    // auto-activate it (/usr/share/dbus-1/services/com.deepin.Picker.service
    // -> /usr/bin/deepin-picker), popping up the actual picker app on the
    // desktop. Point the proxy at a non-activatable name instead: StartPick()
    // is still fully exercised, but the call can only end in an error reply
    // (ServiceUnknown), never in launching the real application.
    ColorPickerInterface iface("com.deepin.Picker.UnitTest", "/com/deepin/Picker",
                               QDBusConnection::sessionBus());
    EXPECT_NO_FATAL_FAILURE({
        QDBusPendingReply<> reply = iface.StartPick("test_app_id");
        (void)reply;
    });
}

// =========================================================================
// Signal colorPicked connection
// =========================================================================

TEST(ColorPickerInterface, ColorPickedSignal_CanBeConnected)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    ColorPickerInterface iface("com.deepin.Picker", "/com/deepin/Picker",
                               QDBusConnection::sessionBus());

    QSignalSpy spy(&iface, &ColorPickerInterface::colorPicked);
    EXPECT_TRUE(spy.isValid());
}

// =========================================================================
// Destructor
// =========================================================================

TEST(ColorPickerInterface, Destructor_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    auto *iface = new ColorPickerInterface("com.deepin.Picker", "/com/deepin/Picker",
                                           QDBusConnection::sessionBus());
    EXPECT_NO_FATAL_FAILURE(delete iface);
}

// =========================================================================
// Namespace alias
// =========================================================================

TEST(ColorPickerInterface, NamespaceAlias_SameType)
{
    bool same = std::is_same<com::deepin::Picker, ColorPickerInterface>::value;
    EXPECT_TRUE(same);
}
