// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>
#include <QStringList>
#include <QWidget>

#define protected public
#define private public
#include "application.h"
#include "ccentralwidget.h"
#include "cgraphicsview.h"
#undef protected
#undef private

#include "cdrawtoolfactory.h"
#include "idrawtool.h"

TEST(ApplicationExitLoad, dApplicationReturnsNonNull)
{
    DApplication *app = drawApp->dApplication();
    EXPECT_NE(app, nullptr);
}

TEST(ApplicationExitLoad, currentDrawScenceReturnsNonNull)
{
    PageScene *scene = drawApp->currentDrawScence();
    EXPECT_NE(scene, nullptr);
}

TEST(ApplicationExitLoad, currentDrawScenceMatchesBoardScene)
{
    PageScene *scene = drawApp->currentDrawScence();
    ASSERT_NE(scene, nullptr);
    ASSERT_NE(drawApp->drawBoard(), nullptr);
    ASSERT_NE(drawApp->drawBoard()->currentPage(), nullptr);
    ASSERT_NE(drawApp->drawBoard()->currentPage()->view(), nullptr);
    EXPECT_EQ(scene, drawApp->drawBoard()->currentPage()->view()->drawScene());
}

TEST(ApplicationExitLoad, quitRequestSignalConnectable)
{
    QSignalSpy spy(drawApp, &Application::quitRequest);
    EXPECT_TRUE(spy.isValid());
}

TEST(ApplicationExitLoad, quitAppSkipped)
{
    GTEST_SKIP() << "quitApp() calls qApp->quit() which terminates "
                    "the test process; skipped to avoid killing tests";
}

TEST(ApplicationExitLoad, execPicturesLimitZeroNoExceed)
{
    int result = drawApp->execPicturesLimit(0);
    EXPECT_EQ(result, 0);
}

TEST(ApplicationExitLoad, execPicturesLimitSkippedModal)
{
    GTEST_SKIP() << "execPicturesLimit with count>30 shows a modal "
                    "DDialog that cannot be automated in offscreen mode; "
                    "skipped";
}

TEST(ApplicationExitLoad, waitShowThenLoadEmptyListNoCrash)
{
    drawApp->waitShowThenLoad(QStringList());
    SUCCEED();
}

TEST(ApplicationExitLoad, loadToolsRepeatedCallNoCrash)
{
    // loadTools() recreates every tool and overwrites the factory
    // map; the new instances carry no drawBoard. Restore the
    // installed tools afterwards so later tests keep board-aware
    // instances and the replaced ones do not leak.
    QMap<int, IDrawTool *> oldTools = CDrawToolFactory::allTools();
    drawApp->loadTools();
    drawApp->loadTools();

    auto &tools = CDrawToolFactory::allTools();
    QList<IDrawTool *> replaced;
    for (auto it = tools.begin(); it != tools.end(); ++it) {
        auto oldIt = oldTools.find(it.key());
        if (oldIt != oldTools.end() && oldIt.value() != it.value()) {
            replaced.append(it.value());
            it.value() = oldIt.value();
        }
    }
    qDeleteAll(replaced);
    SUCCEED();
}

TEST(ApplicationExitLoad, loadPluginToolsNoCrash)
{
    drawApp->loadPluginTools();
    SUCCEED();
}
