// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMap>
#include <QString>
#include <unistd.h>
#include <QCoreApplication>
#include <QFile>
#include <QIODevice>

#define protected public
#define private public
#include "cviewmanagement.h"

/*
 * CFileWatcher is a QThread that monitors a set of files via inotify.
 *   - clear() removes every watched descriptor and empties the bookkeeping maps.
 *   - doRun() is the blocking event loop; it bails out immediately when the
 *     inotify handle is invalid (isVaild() == false).
 *
 * Because doRun() blocks on fread() when the handle is valid, these tests only
 * exercise the non-blocking paths: the early-return guard of doRun() and the
 * clear() path. The fake descriptors used below are never registered with
 * inotify, so inotify_rm_watch() simply fails (and its return value is ignored
 * by clear()), making the test safe and self-contained.
 */

// doRun() must return immediately when the inotify handle is invalid, rather
// than entering its blocking read loop.
TEST(CFileWatcherDoRunTest, ReturnsImmediatelyWhenHandleInvalid)
{
    CFileWatcher watcher;
    // A freshly-constructed watcher owns a real inotify fd; close it and mark the
    // handle invalid so isVaild() == false and doRun() takes the early-return
    // path (no blocking, no fd leak).
    if (watcher._handleId >= 0)
        ::close(watcher._handleId);
    watcher._handleId = -1;

    watcher.doRun();  // returns without blocking
    SUCCEED();
}

// clear() empties both the path->wd and wd->path maps.
TEST(CFileWatcherClearTest, EmptiesWatchedFilesMaps)
{
    CFileWatcher watcher;
    watcher.watchedFiles.insert(QStringLiteral("/nonexistent/a"), 111);
    watcher.watchedFiles.insert(QStringLiteral("/nonexistent/b"), 222);
    watcher.watchedFilesId.insert(111, QStringLiteral("/nonexistent/a"));
    watcher.watchedFilesId.insert(222, QStringLiteral("/nonexistent/b"));
    ASSERT_EQ(watcher.watchedFiles.size(), 2);
    ASSERT_EQ(watcher.watchedFilesId.size(), 2);

    watcher.clear();

    EXPECT_TRUE(watcher.watchedFiles.isEmpty());
    EXPECT_TRUE(watcher.watchedFilesId.isEmpty());
}

// clear() on an already-empty watcher is a no-op and must not crash.
TEST(CFileWatcherClearTest, ClearOnEmptyIsNoOp)
{
    CFileWatcher watcher;
    watcher.watchedFiles.clear();
    watcher.watchedFilesId.clear();

    watcher.clear();

    EXPECT_TRUE(watcher.watchedFiles.isEmpty());
    EXPECT_TRUE(watcher.watchedFilesId.isEmpty());
}

// =========================================================================
// CFileWatcher: constructor / isVaild / addWather / removePath / run
// =========================================================================

TEST(CFileWatcherCtorTest, ConstructorCreatesValidHandle)
{
    CFileWatcher watcher;
    // A freshly-constructed watcher should have a valid inotify fd.
    EXPECT_GE(watcher._handleId, 0);
    EXPECT_TRUE(watcher.isVaild());
}

TEST(CFileWatcherIsVaildTest, InvalidHandleReturnsFalse)
{
    CFileWatcher watcher;
    if (watcher._handleId >= 0)
        ::close(watcher._handleId);
    watcher._handleId = -1;
    EXPECT_FALSE(watcher.isVaild());
}

TEST(CFileWatcherIsVaildTest, ValidHandleReturnsTrue)
{
    CFileWatcher watcher;
    EXPECT_TRUE(watcher.isVaild());
}

TEST(CFileWatcherAddWatherTest, InvalidHandleDoesNotAdd)
{
    CFileWatcher watcher;
    if (watcher._handleId >= 0)
        ::close(watcher._handleId);
    watcher._handleId = -1;

    watcher.addWather(QStringLiteral("/tmp"));
    EXPECT_TRUE(watcher.watchedFiles.isEmpty());
    EXPECT_TRUE(watcher.watchedFilesId.isEmpty());
}

TEST(CFileWatcherAddWatherTest, NonExistentPathDoesNotAdd)
{
    CFileWatcher watcher;
    watcher.addWather(QStringLiteral("/nonexistent/path/that/should/not/exist"));
    EXPECT_TRUE(watcher.watchedFiles.isEmpty());
    EXPECT_TRUE(watcher.watchedFilesId.isEmpty());
}

TEST(CFileWatcherAddWatherTest, ExistingFileIsAdded)
{
    CFileWatcher watcher;
    // Create a temporary file.
    QString tmpPath = QStringLiteral("/tmp/test_cfilewatcher_") +
                      QString::number(QCoreApplication::applicationPid()) + ".txt";
    QFile tmpFile(tmpPath);
    EXPECT_TRUE(tmpFile.open(QIODevice::WriteOnly));
    tmpFile.write("test");
    tmpFile.close();

    watcher.addWather(tmpPath);
    EXPECT_EQ(watcher.watchedFiles.size(), 1);
    EXPECT_TRUE(watcher.watchedFiles.contains(tmpPath));

    // Stop the thread if it was started.
    watcher._running = false;
    if (watcher.isRunning()) {
        watcher.terminate();
        watcher.wait(1000);
    }

    tmpFile.remove();
}

TEST(CFileWatcherRemovePathTest, RemoveExistingPath)
{
    CFileWatcher watcher;
    QString tmpPath = QStringLiteral("/tmp/test_cfilewatcher_rm_") +
                      QString::number(QCoreApplication::applicationPid()) + ".txt";
    QFile tmpFile(tmpPath);
    EXPECT_TRUE(tmpFile.open(QIODevice::WriteOnly));
    tmpFile.write("test");
    tmpFile.close();

    watcher.addWather(tmpPath);
    ASSERT_EQ(watcher.watchedFiles.size(), 1);

    watcher._running = false;
    if (watcher.isRunning()) {
        watcher.terminate();
        watcher.wait(1000);
    }

    watcher.removePath(tmpPath);
    EXPECT_FALSE(watcher.watchedFiles.contains(tmpPath));
    EXPECT_FALSE(watcher.watchedFilesId.values().contains(tmpPath));

    tmpFile.remove();
}

TEST(CFileWatcherRemovePathTest, RemoveNonExistentPathNoCrash)
{
    CFileWatcher watcher;
    watcher.removePath(QStringLiteral("/nonexistent/path"));
    EXPECT_TRUE(watcher.watchedFiles.isEmpty());
}

TEST(CFileWatcherRemovePathTest, RemoveOnInvalidHandleNoCrash)
{
    CFileWatcher watcher;
    if (watcher._handleId >= 0)
        ::close(watcher._handleId);
    watcher._handleId = -1;
    watcher.removePath(QStringLiteral("/some/path"));
    SUCCEED();
}

TEST(CFileWatcherRunTest, RunWithInvalidHandleReturnsImmediately)
{
    CFileWatcher watcher;
    if (watcher._handleId >= 0)
        ::close(watcher._handleId);
    watcher._handleId = -1;

    // run() calls doRun() which should return immediately with invalid handle.
    watcher.run();
    SUCCEED();
}

// =========================================================================
// CManageViewSigleton: GetInstance / isEmpty / removeView
// =========================================================================

TEST(CManageViewSigletonTest, GetInstanceReturnsNonNull)
{
    CManageViewSigleton *inst = CManageViewSigleton::GetInstance();
    EXPECT_NE(inst, nullptr);
}

TEST(CManageViewSigletonTest, GetInstanceReturnsSameInstance)
{
    CManageViewSigleton *inst1 = CManageViewSigleton::GetInstance();
    CManageViewSigleton *inst2 = CManageViewSigleton::GetInstance();
    EXPECT_EQ(inst1, inst2);
}

TEST(CManageViewSigletonTest, IsEmptyReturnsBoolWithoutCrash)
{
    CManageViewSigleton *inst = CManageViewSigleton::GetInstance();
    // Should return true or false without crashing.
    bool result = inst->isEmpty();
    (void)result;
    SUCCEED();
}

TEST(CManageViewSigletonTest, RemoveViewNullPtrNoCrash)
{
    CManageViewSigleton *inst = CManageViewSigleton::GetInstance();
    inst->removeView(nullptr);
    SUCCEED();
}

TEST(CManageViewSigletonTest, RemoveViewNotInListNoCrash)
{
    CManageViewSigleton *inst = CManageViewSigleton::GetInstance();
    // Use a dummy non-null pointer that is not in the list.
    // removeView checks m_allViews.contains(view) first.
    inst->removeView(reinterpret_cast<PageView *>(0xDEAD));
    SUCCEED();
}
