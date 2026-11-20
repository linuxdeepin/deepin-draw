// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Extended unit tests for Application methods (V-9047)
 *
 * Target methods:
 *   - Application::isFileNameLegal(const QString&, int*) — all branches
 *   - Application::execPicturesLimit(int) — < 30 no dialog
 *   - Application::waitShowThenLoad(QStringList()) — empty paths adds page
 *
 * Source: src/application.cpp
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QDir>
#include <QString>
#include <QStringList>

#define protected public
#define private public
#include "application.h"
#include "mainwindow.h"
#include "ccentralwidget.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// Application::isFileNameLegal — full branch matrix
// =========================================================================

TEST(IsFileNameLegalExt, EmptyPathReturnsFalseAndSetsErrorZero)
{
    int outError = -1;
    EXPECT_FALSE(drawApp->isFileNameLegal("", &outError));
    EXPECT_EQ(outError, 0);
}

TEST(IsFileNameLegalExt, EmptyPathWithNullErrorPointer)
{
    // outErrorReson == nullptr must not crash.
    EXPECT_FALSE(drawApp->isFileNameLegal("", nullptr));
}

TEST(IsFileNameLegalExt, IllegalCharsReturnsFalseAndSetsErrorOne)
{
    int outError = -1;
    // Each illegal character individually
    EXPECT_FALSE(drawApp->isFileNameLegal("file:name.txt", &outError));
    EXPECT_EQ(outError, 1);

    EXPECT_FALSE(drawApp->isFileNameLegal("file*name.txt", &outError));
    EXPECT_EQ(outError, 1);

    EXPECT_FALSE(drawApp->isFileNameLegal("file?name.txt", &outError));
    EXPECT_EQ(outError, 1);

    EXPECT_FALSE(drawApp->isFileNameLegal("file\"name.txt", &outError));
    EXPECT_EQ(outError, 1);

    EXPECT_FALSE(drawApp->isFileNameLegal("file<name.txt", &outError));
    EXPECT_EQ(outError, 1);

    EXPECT_FALSE(drawApp->isFileNameLegal("file>name.txt", &outError));
    EXPECT_EQ(outError, 1);

    EXPECT_FALSE(drawApp->isFileNameLegal("file|name.txt", &outError));
    EXPECT_EQ(outError, 1);
}

TEST(IsFileNameLegalExt, IllegalCharsWithNullErrorPointer)
{
    EXPECT_FALSE(drawApp->isFileNameLegal("bad:file", nullptr));
    EXPECT_FALSE(drawApp->isFileNameLegal("bad*file", nullptr));
}

TEST(IsFileNameLegalExt, NonExistentDirectoryReturnsFalseAndSetsErrorTwo)
{
    int outError = -1;
    // The directory portion does not exist on disk.
    QString path = "/nonexistent_dir_12345/sub/file.txt";
    EXPECT_FALSE(drawApp->isFileNameLegal(path, &outError));
    EXPECT_EQ(outError, 2);
}

TEST(IsFileNameLegalExt, ExistingDirectoryValidFileReturnsTrue)
{
    int outError = -1;
    // Use the current directory which always exists.
    QString path = QDir::currentPath() + "/test_legal_file.txt";
    EXPECT_TRUE(drawApp->isFileNameLegal(path, &outError));
    // outError should NOT be set for a valid path (no assignment in success path).
}

TEST(IsFileNameLegalExt, PathEndingWithSlashReturnsFalse)
{
    int outError = -1;
    QString path = QDir::currentPath() + "/";
    EXPECT_FALSE(drawApp->isFileNameLegal(path, &outError));
}

TEST(IsFileNameLegalExt, PathEndingWithBackslashReturnsFalse)
{
    int outError = -1;
    // Use a Windows-style path ending with backslash; directory must exist.
    QString dir = QDir::currentPath();
    QString path = dir + "\\";
    EXPECT_FALSE(drawApp->isFileNameLegal(path, &outError));
}

// =========================================================================
// Application::execPicturesLimit — < 30 no dialog, returns 0
// =========================================================================

TEST(ExecPicturesLimitExt, EmptySceneCountBelowLimitReturnsZero)
{
    // With no items in the scene, exitPicNum stays 0.
    // count < 30 so the dialog is not shown and ret == 0.
    createNewViewByShortcutKey();
    int result = drawApp->execPicturesLimit(5);
    EXPECT_EQ(result, 0);
}

TEST(ExecPicturesLimitExt, ZeroCountReturnsZero)
{
    createNewViewByShortcutKey();
    int result = drawApp->execPicturesLimit(0);
    EXPECT_EQ(result, 0);
}

TEST(ExecPicturesLimitExt, CountAtLimitBoundaryReturnsZero)
{
    // 0 + 30 == 30, not > 30, so no dialog.
    createNewViewByShortcutKey();
    int result = drawApp->execPicturesLimit(30);
    EXPECT_EQ(result, 0);
}

// =========================================================================
// Application::waitShowThenLoad — empty paths adds blank page
// =========================================================================

TEST(WaitShowThenLoadExt, EmptyPathsAddsBlankPage)
{
    createNewViewByShortcutKey();
    DrawBoard *board = drawApp->topMainWindow()->drawBoard();
    ASSERT_NE(board, nullptr);
    int countBefore = board->count();

    drawApp->waitShowThenLoad(QStringList());

    // The empty-paths branch calls drawBoard()->addPage("") synchronously.
    EXPECT_EQ(board->count(), countBefore + 1);
}
