// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for DrawInterface (issue V-6699).
 *
 * Source: src/service/drawinterface.cpp/h
 *
 * Target methods:
 *   Constructor, destructor, staticInterfaceName,
 *   openFiles, openImages
 *   Namespace alias
 *
 * The openFiles/openImages tests make real DBus calls. They only run when
 * THIS test process itself owns com.deepin.Draw (registered by the harness),
 * so the system-installed deepin-draw app is never raised or activated by
 * the run.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "drawinterface.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>
#include <QImage>
#include <QStringList>
#include <type_traits>

// Returns a non-empty skip reason when the com.deepin.Draw name on the
// session bus is NOT owned by this test process: an external owner means a
// real call would raise the system-installed deepin-draw app, and an
// unowned name means dbus-daemon would auto-activate it via ll-cli.
// Empty result means this process itself owns the name (the harness
// registers it via its in-process DrawBoard), so the real calls below are
// delivered to the in-process service — safe to run.
//
// Note: Qt 6.8 has no QDBusAbstractInterface-level auto-start toggle (it
// moved to QDBusMessage, unreachable through DrawInterface's product API),
// so the ownership check is the only guard needed/possible.
static QString drawServiceSkipReason()
{
    QDBusConnectionInterface *bus = QDBusConnection::sessionBus().interface();
    if (bus == nullptr) {
        return QStringLiteral("no session bus interface");
    }
    const QString owner = bus->serviceOwner(QStringLiteral("com.deepin.Draw"));
    if (owner.isEmpty()) {
        return QStringLiteral("com.deepin.Draw is unowned — calling would trigger dbus auto-start");
    }
    if (owner != QDBusConnection::sessionBus().baseService()) {
        return QStringLiteral("com.deepin.Draw owned by external process ") + owner;
    }
    return QString();
}

// =========================================================================
// Construction tests
// =========================================================================

TEST(DrawInterface, Construct_WithoutParent_ServicePathMatch)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                        QDBusConnection::sessionBus());
    EXPECT_EQ(iface.parent(), nullptr);
    EXPECT_EQ(iface.service(), QStringLiteral("com.deepin.Draw"));
    EXPECT_EQ(iface.path(), QStringLiteral("/com/deepin/Draw"));
}

TEST(DrawInterface, Construct_WithParent_ParentSet)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    QObject parent;
    DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                        QDBusConnection::sessionBus(), &parent);
    EXPECT_EQ(iface.parent(), &parent);
}

TEST(DrawInterface, Construct_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    EXPECT_NO_FATAL_FAILURE({
        DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                            QDBusConnection::sessionBus());
    });
}

// =========================================================================
// staticInterfaceName
// =========================================================================

TEST(DrawInterface, StaticInterfaceName_ReturnsCorrectValue)
{
    EXPECT_STREQ(DrawInterface::staticInterfaceName(), "com.deepin.Draw");
}

// =========================================================================
// openFiles
// =========================================================================

TEST(DrawInterface, OpenFiles_EmptyList_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }
    // Only call when THIS process owns the service name — otherwise the
    // real call would reach (or launch) the system-installed deepin-draw.
    const QString skipReason = drawServiceSkipReason();
    if (!skipReason.isEmpty()) {
        GTEST_SKIP() << skipReason.toStdString();
    }

    DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                        QDBusConnection::sessionBus());
    QList<QString> emptyList;
    EXPECT_NO_FATAL_FAILURE({
        QDBusPendingReply<> reply = iface.openFiles(emptyList);
        (void)reply;
    });
}

TEST(DrawInterface, OpenFiles_NonExistentPaths_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }
    // Only call when THIS process owns the service name — otherwise the
    // real call would reach (or launch) the system-installed deepin-draw.
    const QString skipReason = drawServiceSkipReason();
    if (!skipReason.isEmpty()) {
        GTEST_SKIP() << skipReason.toStdString();
    }

    DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                        QDBusConnection::sessionBus());
    QList<QString> paths;
    paths << "/tmp/not_exist_1.png" << "/tmp/not_exist_2.png";
    EXPECT_NO_FATAL_FAILURE({
        QDBusPendingReply<> reply = iface.openFiles(paths);
        (void)reply;
    });
}

// =========================================================================
// openImages
// =========================================================================

TEST(DrawInterface, OpenImages_EmptyList_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }
    // Only call when THIS process owns the service name — otherwise the
    // real call would reach (or launch) the system-installed deepin-draw.
    const QString skipReason = drawServiceSkipReason();
    if (!skipReason.isEmpty()) {
        GTEST_SKIP() << skipReason.toStdString();
    }

    DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                        QDBusConnection::sessionBus());
    QList<QImage> emptyList;
    EXPECT_NO_FATAL_FAILURE({
        QDBusPendingReply<> reply = iface.openImages(emptyList);
        (void)reply;
    });
}

TEST(DrawInterface, OpenImages_ValidImage_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }
    // Only call when THIS process owns the service name — otherwise the
    // real call would reach (or launch) the system-installed deepin-draw.
    const QString skipReason = drawServiceSkipReason();
    if (!skipReason.isEmpty()) {
        GTEST_SKIP() << skipReason.toStdString();
    }

    DrawInterface iface("com.deepin.Draw", "/com/deepin/Draw",
                        QDBusConnection::sessionBus());
    QList<QImage> images;
    images << QImage(10, 10, QImage::Format_ARGB32);
    EXPECT_NO_FATAL_FAILURE({
        QDBusPendingReply<> reply = iface.openImages(images);
        (void)reply;
    });
}

// =========================================================================
// Destructor
// =========================================================================

TEST(DrawInterface, Destructor_NoCrash)
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        GTEST_SKIP() << "No session bus available — skipping DBus test";
    }

    auto *iface = new DrawInterface("com.deepin.Draw", "/com/deepin/Draw",
                                    QDBusConnection::sessionBus());
    EXPECT_NO_FATAL_FAILURE(delete iface);
}

// =========================================================================
// Namespace alias
// =========================================================================

TEST(DrawInterface, NamespaceAlias_SameType)
{
    bool same = std::is_same<com::deepin::Draw, DrawInterface>::value;
    EXPECT_TRUE(same);
}
