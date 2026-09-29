// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QImage>
#include <QPainter>

#define protected public
#define private public
#include "publicApi.h"
// publicApi.h #undefs private/protected — re-define before including
// headers that declare private members we need to access.
#undef protected
#undef private
#define protected public
#define private public
#include "filehander.h"
#include "ccentralwidget.h"

/*
 * FileHander unit tests (V-5282)
 *
 * Exercises: isLegalFile, toLegalFile, pathControl, checkFileBeforeLoad,
 * checkFileBeforeSave, supDdfStuffix, supPictureSuffix,
 * saveToDdf, saveToImage
 *
 * isLegalFile / toLegalFile / supDdfStuffix / supPictureSuffix are static
 * and can be tested without an application instance.
 * pathControl / checkFileBeforeLoad / checkFileBeforeSave require a
 * FileHander instance (checkFileBeforeLoad/Save are private but accessible
 * via #define private public).
 * saveToDdf / saveToImage require a valid PageContext; we test the
 * early-return / error paths.
 */

// ---------------------------------------------------------------------------
// supDdfStuffix (static)
// ---------------------------------------------------------------------------

TEST(FileHanderSupDdfStuffixTest, ReturnsDdfOnly)
{
    QStringList suffixes = FileHander::supDdfStuffix();
    EXPECT_EQ(suffixes.size(), 1);
    EXPECT_TRUE(suffixes.contains("ddf"));
}

// ---------------------------------------------------------------------------
// supPictureSuffix (static, needs drawApp)
// ---------------------------------------------------------------------------

TEST(FileHanderSupPictureSuffixTest, ReturnsNonEmptyList)
{
    // supPictureSuffix() calls drawApp->readableFormats(); the application
    // must be initialised (it is, via the test main / publicApi.h).
    QStringList suffixes = FileHander::supPictureSuffix();
    EXPECT_FALSE(suffixes.isEmpty());
    // Common formats should be present.
    EXPECT_TRUE(suffixes.contains("png") || suffixes.contains("jpg") ||
                suffixes.contains("bmp"));
}

// ---------------------------------------------------------------------------
// isLegalFile (static)
// ---------------------------------------------------------------------------

TEST(FileHanderIsLegalFileTest, EmptyPathReturnsFalse)
{
    EXPECT_FALSE(FileHander::isLegalFile(""));
}

TEST(FileHanderIsLegalFileTest, ValidFileReturnsTrue)
{
    QTemporaryFile tmp;
    ASSERT_TRUE(tmp.open());
    tmp.write("test");
    tmp.close();
    // QTemporaryFile::fileName() is the full path; the file exists.
    EXPECT_TRUE(FileHander::isLegalFile(tmp.fileName()));
}

TEST(FileHanderIsLegalFileTest, DirectoryReturnsFalse)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    // A directory path is not a legal file.
    EXPECT_FALSE(FileHander::isLegalFile(dir.path()));
}

TEST(FileHanderIsLegalFileTest, NonExistentParentDirReturnsFalse)
{
    // isLegalFile checks that the parent directory exists and the path is
    // not a directory — it does NOT check whether the file itself exists.
    // Use a path whose parent directory does not exist.
    QString path = "/tmp/no_such_dir_12345/nonexistent_file.png";
    EXPECT_FALSE(FileHander::isLegalFile(path));
}

// ---------------------------------------------------------------------------
// toLegalFile (static)
// ---------------------------------------------------------------------------

TEST(FileHanderToLegalFileTest, EmptyStringReturnsEmpty)
{
    EXPECT_TRUE(FileHander::toLegalFile("").isEmpty());
}

TEST(FileHanderToLegalFileTest, ValidFilePathReturnedAsIs)
{
    QTemporaryFile tmp;
    ASSERT_TRUE(tmp.open());
    tmp.write("test");
    tmp.close();
    EXPECT_EQ(FileHander::toLegalFile(tmp.fileName()), tmp.fileName());
}

TEST(FileHanderToLegalFileTest, NonExistentParentDirReturnsEmpty)
{
    // toLegalFile calls isLegalFile which checks the parent dir exists.
    // With a non-existent parent dir, isLegalFile returns false → empty result.
    QString path = "/tmp/no_such_dir_12345/nonexistent_file.png";
    EXPECT_TRUE(FileHander::toLegalFile(path).isEmpty());
}

// ---------------------------------------------------------------------------
// pathControl (static)
// ---------------------------------------------------------------------------

TEST(FileHanderPathControlTest, EmptyPathReturnsFalse)
{
    // Empty path -> early return false (no DBus call needed).
    EXPECT_FALSE(FileHander::pathControl(""));
}

// ---------------------------------------------------------------------------
// checkFileBeforeLoad (private, needs FileHander instance)
// ---------------------------------------------------------------------------

TEST(FileHanderCheckFileBeforeLoadTest, EmptyFileReturnsFalse)
{
    FileHander fh;
    // Empty file -> toLegalFile returns "" -> returns false.
    EXPECT_FALSE(fh.checkFileBeforeLoad("", false));
    EXPECT_FALSE(fh.checkFileBeforeLoad("", true));
}

TEST(FileHanderCheckFileBeforeLoadTest, NonExistentFileReturnsFalse)
{
    FileHander fh;
    QString path = "/tmp/nonexistent_checkload_12345.png";
    EXPECT_FALSE(fh.checkFileBeforeLoad(path, false));
}

TEST(FileHanderCheckFileBeforeLoadTest, UnsupportedSuffixReturnsFalse)
{
    FileHander fh;
    QTemporaryFile tmp;
    ASSERT_TRUE(tmp.open());
    tmp.write("test");
    tmp.close();
    // Rename to an unsupported suffix.
    QString badPath = tmp.fileName() + ".xyz";
    QFile::copy(tmp.fileName(), badPath);

    EXPECT_FALSE(fh.checkFileBeforeLoad(badPath, false));

    QFile::remove(badPath);
}

// ---------------------------------------------------------------------------
// checkFileBeforeSave (private, needs FileHander instance)
// ---------------------------------------------------------------------------

TEST(FileHanderCheckFileBeforeSaveTest, EmptyFileReturnsFalse)
{
    FileHander fh;
    // Empty file -> toLegalFile returns "" -> returns false.
    EXPECT_FALSE(fh.checkFileBeforeSave("", false));
    EXPECT_FALSE(fh.checkFileBeforeSave("", true));
}

TEST(FileHanderCheckFileBeforeSaveTest, NonExistentParentDirReturnsFalse)
{
    FileHander fh;
    // checkFileBeforeSave calls toLegalFile which checks the parent dir.
    // With a non-existent parent dir, toLegalFile returns empty → false.
    QString path = "/tmp/no_such_dir_12345/nonexistent_file.png";
    EXPECT_FALSE(fh.checkFileBeforeSave(path, false));
}

// ---------------------------------------------------------------------------
// saveToDdf
// ---------------------------------------------------------------------------

TEST(FileHanderSaveToDdfTest, NullContextNonExistentFileReturnsFalse)
{
    FileHander fh;
    // saveToDdf accesses context->file() when file is empty, so we pass a
    // non-empty path. checkFileBeforeSave returns false (non-existent file)
    // before context is accessed, so saveToDdf returns false safely.
    EXPECT_FALSE(fh.saveToDdf(nullptr, "/tmp/no_such_dir_12345/test.ddf"));
}

// ---------------------------------------------------------------------------
// saveToImage
// ---------------------------------------------------------------------------

TEST(FileHanderSaveToImageTest, EmptyFileReturnsFalse)
{
    FileHander fh;
    // Empty file -> checkFileBeforeSave returns false -> returns false.
    EXPECT_FALSE(fh.saveToImage(nullptr, ""));
}
