// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QMap>
#include <QString>
#include <unistd.h>

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
