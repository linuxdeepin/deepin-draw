// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for Application lambdas
 * (src/application.cpp).
 *
 * Target lambdas (0-coverage):
 *   Application::execDraw — calls _dApp->exec() which blocks; skipped.
 *   ~Application — empty body; covered trivially by construction/destruction
 *       pattern. Skipped (cannot destruct the global singleton).
 *   waitShowThenLoad lambda — the QMetaObject::invokeMethod lambda that
 *       calls actWin->loadFiles(paths).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QStringList>

#define protected public
#define private public
#include "application.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#include "publicApi.h"
#undef protected
#undef private

// ===========================================================================
// Application::execDraw — blocks, skipped
// ===========================================================================

TEST(ApplicationLambdaTest, ExecDrawSkipped)
{
    GTEST_SKIP() << "Application::execDraw() calls _dApp->exec() which blocks "
                    "the event loop; cannot be tested.";
}

// ===========================================================================
// ~Application — empty body, trivial; singleton not destructible in tests
// ===========================================================================

TEST(ApplicationLambdaTest, DestructorIsTrivial)
{
    GTEST_SKIP() << "~Application() has an empty body and the global "
                    "singleton cannot be destructed within the test harness.";
}

// ===========================================================================
// waitShowThenLoad — non-empty paths with visible window
// ===========================================================================

TEST(ApplicationLambdaTest, WaitShowThenLoadEmptyPathsNoCrash)
{
    // Empty paths → addPage("") called, returns immediately
    drawApp->waitShowThenLoad(QStringList());
    qApp->processEvents();
    SUCCEED();
}

TEST(ApplicationLambdaTest, WaitShowThenLoadNonExistentPathsNoCrash)
{
    // Non-empty paths with nonexistent files — loadFiles should handle
    // gracefully without crash. If window not visible, it queues via
    // QueuedConnection; process events to flush.
    QStringList paths;
    paths << "/nonexistent/path/to/file.png";

    drawApp->waitShowThenLoad(paths);
    // Process events to flush any queued invocations
    qApp->processEvents();
    qApp->processEvents();
    SUCCEED();
}

TEST(ApplicationLambdaTest, WaitShowThenLoadMultiplePathsNoCrash)
{
    QStringList paths;
    paths << "/nonexistent/a.png" << "/nonexistent/b.jpg" << "/nonexistent/c.bmp";

    drawApp->waitShowThenLoad(paths);
    qApp->processEvents();
    qApp->processEvents();
    SUCCEED();
}
