// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "publicApi.h"
#include "mainwindow.h"

/*
 * MainWindow::initConnection (72 lines, complexity 5)
 *
 * Wires up ~12 signal/slot connections between DrawBoard, TopTilte, drawApp
 * and the quit/export actions. Already called once during application start-up;
 * calling it again re-establishes connections (Qt deduplicates direct
 * signal-slot connections). Smoke test: no crash.
 */

TEST(MainWindowInitConnectionTest, ReinvokeDoesNotCrash)
{
    MainWindow *mw = drawApp->topMainWindow();
    ASSERT_NE(mw, nullptr);

    mw->initConnection();
    SUCCEED();
}
