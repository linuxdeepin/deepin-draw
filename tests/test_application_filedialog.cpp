// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QString>
#include <QDir>
#include <QStandardPaths>

#define protected public
#define private public
#include "application.h"
#undef protected
#undef private

TEST(ApplicationFileDialog, setAndgetDefaultFileDialogPath)
{
    QString testPath = QDir::homePath();
    drawApp->setDefaultFileDialogPath(testPath);
    EXPECT_EQ(drawApp->defaultFileDialogPath(), testPath);
}

TEST(ApplicationFileDialog, setFileDialogPathEmpty)
{
    drawApp->setDefaultFileDialogPath("");
    drawApp->defaultFileDialogPath();  // should not crash
    SUCCEED();
}

TEST(ApplicationFileDialog, setFileDialogPathNonExistent)
{
    QString testPath = QDir::homePath() + "/test_nonexistent_draw_path";
    QDir().mkpath(testPath);
    drawApp->setDefaultFileDialogPath(testPath);
    QString result = drawApp->defaultFileDialogPath();
    QDir dir(result);
    EXPECT_TRUE(dir.exists());
    QDir().rmpath(testPath);
}

TEST(ApplicationFileDialog, setFileDialogPathChinese)
{
    QString chinesePath = QDir::homePath() + "/测试目录";
    QDir().mkpath(chinesePath);
    drawApp->setDefaultFileDialogPath(chinesePath);
    EXPECT_EQ(drawApp->defaultFileDialogPath(), chinesePath);
    QDir().rmpath(chinesePath);
}

TEST(ApplicationFileDialog, setAndgetDefaultFileDialogNameFilter)
{
    QString testFilter = "PNG(*.png)";
    drawApp->setDefaultFileDialogNameFilter(testFilter);
    EXPECT_EQ(drawApp->defaultFileDialogNameFilter(), testFilter);
}

TEST(ApplicationFileDialog, setFileDialogNameFilterEmpty)
{
    drawApp->setDefaultFileDialogNameFilter("");
    EXPECT_EQ(drawApp->defaultFileDialogNameFilter(), QString(""));
}

TEST(ApplicationFileDialog, setFileDialogNameFilterVarious)
{
    QStringList testFilters;
    testFilters << "JPEG(*.jpeg *.jpg)" << "BMP(*.bmp)" << "PDF(*.pdf)";
    for (const QString &f : testFilters) {
        drawApp->setDefaultFileDialogNameFilter(f);
        EXPECT_EQ(drawApp->defaultFileDialogNameFilter(), f);
    }
}
