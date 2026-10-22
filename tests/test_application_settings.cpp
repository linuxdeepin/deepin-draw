// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QWidget>
#include <QSize>

#define protected public
#define private public
#include "application.h"
#include "global.h"
#include "mainwindow.h"
#undef protected
#undef private

/*
 * readSettings() resizes/maximizes the main window from the persisted
 * (or first-launch) geometry. On the offscreen platform that leaves the
 * window shrunk for the rest of the process, so every view created
 * afterwards centres its scene in a smaller viewport and items drawn by
 * later tests land at shifted scene positions. Save the window geometry
 * around readSettings() so the process keeps its original size.
 */
namespace {
QByteArray g_savedGeometry;
QByteArray g_savedWindowState;
QSize g_savedSize;

void saveWindowGeometry()
{
    g_savedGeometry = drawApp->topMainWindow()->saveGeometry();
    g_savedWindowState = drawApp->topMainWindow()->saveState();
    g_savedSize = drawApp->topMainWindow()->size();
}

void restoreWindowGeometry()
{
    // Drop the maximized flag first: restoreGeometry() refuses to resize
    // a maximized window, which would leave readSettings()'s half-size
    // window in place for every later test.
    drawApp->topMainWindow()->setWindowState(Qt::WindowNoState);
    drawApp->topMainWindow()->restoreGeometry(g_savedGeometry);
    drawApp->topMainWindow()->restoreState(g_savedWindowState);
    drawApp->topMainWindow()->resize(g_savedSize);
}
}

TEST(ApplicationSettings, saveAndReadSettingsRoundTrip)
{
    QString testPath = QDir::homePath();
    QString testFilter = "PNG(*.png)";

    drawApp->setDefaultFileDialogPath(testPath);
    drawApp->setDefaultFileDialogNameFilter(testFilter);
    drawApp->saveSettings();

    drawApp->_defaultFileDialogPath = "";
    drawApp->_defaultFileDialogNameFilter = "";
    saveWindowGeometry();
    drawApp->readSettings();
    restoreWindowGeometry();

    EXPECT_EQ(drawApp->defaultFileDialogPath(), testPath);
    EXPECT_EQ(drawApp->defaultFileDialogNameFilter(), testFilter);
}

TEST(ApplicationSettings, readSettingsNoCrashWithMissingConfig)
{
    QString configFile = Global::configPath() + "/config.conf";
    if (QFile::exists(configFile)) {
        QFile::remove(configFile);
    }

    drawApp->_defaultFileDialogPath = "";
    drawApp->_defaultFileDialogNameFilter = "";

    saveWindowGeometry();
    drawApp->readSettings();
    restoreWindowGeometry();

    EXPECT_FALSE(drawApp->defaultFileDialogPath().isEmpty());

    QStringList writeFilters = drawApp->writableFormatNameFilters();
    if (!writeFilters.isEmpty()) {
        EXPECT_FALSE(drawApp->defaultFileDialogNameFilter().isEmpty());
    }
    SUCCEED();
}

TEST(ApplicationSettings, readSettingsFallbackNameFilter)
{
    QString configFile = Global::configPath() + "/config.conf";
    if (QFile::exists(configFile)) {
        QFile::remove(configFile);
    }

    drawApp->_defaultFileDialogNameFilter = "";
    saveWindowGeometry();
    drawApp->readSettings();
    restoreWindowGeometry();

    QStringList writeFilters = drawApp->writableFormatNameFilters();
    if (!writeFilters.isEmpty()) {
        EXPECT_EQ(drawApp->defaultFileDialogNameFilter(),
                  writeFilters.first());
    }
}

TEST(ApplicationSettings, saveSettingsCreatesFile)
{
    drawApp->setDefaultFileDialogPath(QDir::homePath());
    drawApp->setDefaultFileDialogNameFilter("BMP(*.bmp)");
    drawApp->saveSettings();

    QString configFile = Global::configPath() + "/config.conf";
    EXPECT_TRUE(QFile::exists(configFile));
}
