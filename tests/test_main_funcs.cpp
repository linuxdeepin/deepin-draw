// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for free functions in main.cpp.
 *
 * Target functions (from issue V-5568):
 *   - getFilesFromQCommandLineParser(const QCommandLineParser &)
 *   - checkOnly()
 *
 * The production main.cpp is compiled into the test binary (with its
 * main() entry renamed, see tests/CMakeLists.txt), so the tests below
 * invoke the real implementations directly.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>

// Production helpers from src/deepin-draw/main.cpp.
extern QStringList getFilesFromQCommandLineParser(const QCommandLineParser &parser);
extern bool checkOnly();

// =========================================================================
// getFilesFromQCommandLineParser
// =========================================================================

TEST(MainFuncsTest, GetFilesFromEmptyParser)
{
    QCommandLineParser parser;
    QStringList files = getFilesFromQCommandLineParser(parser);
    EXPECT_TRUE(files.isEmpty());
}

TEST(MainFuncsTest, GetFilesWithPositionalArgs)
{
    QCommandLineParser parser;
    parser.parse(QStringList() << "app" << "file1.png" << "file2.ddf" << "file3.jpg");
    QStringList files = getFilesFromQCommandLineParser(parser);
    EXPECT_EQ(files.size(), 3);
    EXPECT_TRUE(files.contains("file1.png"));
    EXPECT_TRUE(files.contains("file2.ddf"));
    EXPECT_TRUE(files.contains("file3.jpg"));
}

TEST(MainFuncsTest, GetFilesIgnoresOptions)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("debug"));
    parser.parse(QStringList() << "app" << "--debug" << "image.png");
    QStringList files = getFilesFromQCommandLineParser(parser);
    EXPECT_EQ(files.size(), 1);
    EXPECT_TRUE(files.contains("image.png"));
}

TEST(MainFuncsTest, GetFilesWithSingleFile)
{
    QCommandLineParser parser;
    parser.parse(QStringList() << "app" << "only.ddf");
    QStringList files = getFilesFromQCommandLineParser(parser);
    EXPECT_EQ(files.size(), 1);
    EXPECT_EQ(files.first(), QString("only.ddf"));
}

// =========================================================================
// checkOnly
// =========================================================================

TEST(MainFuncsTest, CheckOnlyReturnsTrue)
{
    // checkOnly creates/locks a lock file; the first call should succeed.
    bool result = checkOnly();
    EXPECT_TRUE(result);
}

TEST(MainFuncsTest, CheckOnlyIdempotent)
{
    // Calling checkOnly again should still succeed (the lock is held
    // by the same process, and we create a new fd each time).
    bool result = checkOnly();
    EXPECT_TRUE(result);
}
