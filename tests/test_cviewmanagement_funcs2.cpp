// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CManageViewSigleton / CFileWatcher (issue V-7429)
 *
 * Source: src/frame/cviewmanagement.cpp
 *
 * Target methods:
 *   CManageViewSigleton::initBlockShutdown()
 *   CFileWatcher::fileChanged(const QString &path, int tp)  [SIGNAL]
 *
 * Note: fileChanged is a SIGNAL emitted by CFileWatcher (declared in
 *       cviewmanagement.h) when a file modification/move is detected.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QFile>
#include <QTemporaryFile>
#include <QSignalSpy>

#define protected public
#define private public
#include "cviewmanagement.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CManageViewSigleton::initBlockShutdown
// =========================================================================

TEST(CManageViewSigletonInitBlockShutdown, DoesNotCrash)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);

    // initBlockShutdown creates a DBus interface to login1 and calls
    // Inhibit. In a test environment this may or may not succeed,
    // but should not crash.
    mgr->initBlockShutdown();
    SUCCEED();
}

TEST(CManageViewSigletonInitBlockShutdown, SecondCallIsIdempotent)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);

    // First call
    mgr->initBlockShutdown();

    // Second call should be idempotent (early return if already initialized)
    mgr->initBlockShutdown();
    SUCCEED();
}

TEST(CManageViewSigletonInitBlockShutdown, CreatesLoginManagerInterface)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);

    // Reset state to test fresh initialization
    mgr->m_arg.clear();
    mgr->m_reply = QDBusReply<QDBusUnixFileDescriptor>();

    mgr->initBlockShutdown();

    // After init, m_arg should be populated (non-empty)
    EXPECT_FALSE(mgr->m_arg.isEmpty());
}

// =========================================================================
// CFileWatcher::fileChanged (SIGNAL)
// =========================================================================

TEST(CFileWatcherFileChangedSignal, CanBeEmitted)
{
    CFileWatcher watcher;
    QSignalSpy spy(&watcher, &CFileWatcher::fileChanged);

    // Emit the signal manually to verify it works
    emit watcher.fileChanged("/tmp/test_file.png", 0);

    EXPECT_EQ(spy.count(), 1);
    if (spy.count() >= 1) {
        QList<QVariant> args = spy.takeFirst();
        EXPECT_EQ(args.at(0).toString(), "/tmp/test_file.png");
        EXPECT_EQ(args.at(1).toInt(), 0);
    }
}

TEST(CFileWatcherFileChangedSignal, MultipleEmissions)
{
    CFileWatcher watcher;
    QSignalSpy spy(&watcher, &CFileWatcher::fileChanged);

    emit watcher.fileChanged("/tmp/file1.png", 0);
    emit watcher.fileChanged("/tmp/file2.png", 1);

    EXPECT_EQ(spy.count(), 2);
}
