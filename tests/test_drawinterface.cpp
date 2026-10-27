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
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "drawinterface.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QImage>
#include <QStringList>
#include <type_traits>

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
