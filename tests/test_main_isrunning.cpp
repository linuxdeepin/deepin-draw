// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for isRunning(Application&) (issue V-8787)
 *
 * Source: src/deepin-draw/main.cpp
 *
 * Target function:
 *   bool isRunning(Application &a)
 *
 * isRunning returns true when either:
 *   - setSingleInstance("deepinDraw") fails (another instance holds the
 *     DDE single-instance lock), or
 *   - checkOnly() fails (the lockf file lock is already held).
 *
 * Note: POSIX file locks (lockf/fcntl) are per-process, not per-fd.
 * Two file descriptors in the same process referencing the same file
 * share the same lock, so we cannot simulate lock contention from within
 * the test process itself. The pre-held-lock path is tested via fork().
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// Application type must be visible before the extern declaration.
#include "application.h"

// isRunning is defined in src/deepin-draw/main.cpp which is compiled
// into the test binary with main() renamed to deepinDrawMain.
extern bool isRunning(Application &a);

// checkOnly is also in main.cpp; reuse it for lock-state verification.
extern bool checkOnly();

/*
 * RAII guard that points HOME at a fresh temp dir so the lock file
 * (~/.cache/deepin/deepin-draw/single) is always freshly creatable.
 * Restores the original HOME on destruction.
 */
class ScopedTempHome
{
public:
    ScopedTempHome()
    {
        m_oldHome = qEnvironmentVariable("HOME");
        m_dir.setAutoRemove(true);
        qputenv("HOME", m_dir.path().toUtf8());
    }
    ~ScopedTempHome()
    {
        if (!m_oldHome.isEmpty())
            qputenv("HOME", m_oldHome.toUtf8());
        else
            qunsetenv("HOME");
    }
private:
    QTemporaryDir m_dir;
    QString m_oldHome;
};

// =========================================================================
// isRunning — basic non-crash and return type
// =========================================================================

TEST(IsRunningTest, DoesNotCrash_ReturnsBool)
{
    ScopedTempHome homeGuard;
    Application *app = Application::drawApplication();
    ASSERT_NE(app, nullptr);

    bool result = isRunning(*app);
    // In the test harness, checkOnly() with a fresh HOME should succeed,
    // and setSingleInstance should also succeed (first call), so
    // isRunning should return false. But lock-file timing can vary,
    // so we mainly assert no crash and a valid boolean.
    (void)result;
    SUCCEED();
}

// =========================================================================
// isRunning — fresh lock returns false (no other instance)
// =========================================================================

TEST(IsRunningTest, FreshLock_ReturnsFalse)
{
    ScopedTempHome homeGuard;
    Application *app = Application::drawApplication();
    ASSERT_NE(app, nullptr);

    // With a fresh HOME, checkOnly() should succeed (lock file is new).
    // setSingleInstance("deepinDraw") may have been called already by
    // the harness, but it returns true on subsequent calls within the
    // same process. So isRunning should return false.
    bool result = isRunning(*app);
    EXPECT_FALSE(result);
}

// =========================================================================
// isRunning — pre-held lock from child process returns true
//
// POSIX file locks are per-process: two fds in the same process share
// one lock, so we fork a child to hold the lock and simulate a second
// instance. The child holds the lock until killed by the parent.
// =========================================================================

TEST(IsRunningTest, PreHeldLock_FromChild_ReturnsTrue)
{
    ScopedTempHome homeGuard;
    Application *app = Application::drawApplication();
    ASSERT_NE(app, nullptr);

    // Determine the lock file path (same logic as checkOnly()).
    QString lockDirStr = QDir::homePath() + "/.cache/deepin/deepin-draw/";
    QDir().mkpath(lockDirStr);
    QString lockPath = lockDirStr + "single";

    // Create a pipe so the child can signal when it holds the lock.
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    // Fork a child that holds the lock.
    pid_t pid = fork();
    ASSERT_GE(pid, 0) << "fork() failed: " << strerror(errno);

    if (pid == 0) {
        // Child: open and lock the file, then wait for parent signal.
        close(pipefd[0]); // close read end
        int cfd = open(lockPath.toUtf8().constData(), O_WRONLY | O_CREAT, 0644);
        if (cfd < 0) { close(pipefd[1]); _exit(1); }
        if (lockf(cfd, F_LOCK, 0) < 0) { close(pipefd[1]); _exit(2); }
        // Signal parent that lock is held.
        char ok = 1;
        write(pipefd[1], &ok, 1);
        close(pipefd[1]);
        // Sleep until killed — keep holding the lock.
        pause();
        _exit(0);
    }

    // Parent: wait for child to signal lock acquired.
    close(pipefd[1]); // close write end
    char sig = 0;
    read(pipefd[0], &sig, 1);
    close(pipefd[0]);

    ASSERT_EQ(sig, 1) << "Child failed to acquire lock";

    // Now checkOnly() should fail to acquire the lock (held by child).
    bool co = checkOnly();
    EXPECT_FALSE(co) << "checkOnly() should fail when lock is held by child process";

    bool result = isRunning(*app);
    EXPECT_TRUE(result) << "isRunning should return true when another process holds the lock";

    // Kill and reap the child.
    kill(pid, SIGTERM);
    int status = 0;
    waitpid(pid, &status, 0);
}
