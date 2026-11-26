// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for FileHander old-ddf cancel, damaged image, and
 * writability error paths (issue V-9310 §1).
 *
 * Source: src/service/filehander.cpp
 *
 * Target paths:
 *   loadDdf(":/oldBlurItem.ddf") — EUserCancelLoad_OldBlur (191-199, 749-750)
 *   loadDdf(":/oldPenItem.ddf")  — EUserCancelLoad_OldPen  (284-299, 369-373)
 *   loadDdf with pen width <= 0  — continue skip (302-307)
 *   loadImage damaged PNG        — EDamagedImageFile (794-797)
 *   checkFileWritable read-only  — EUnWritableFile (1045-1047)
 *   checkFileWritable read-only dir — EUnWritableDir (1053-1055)
 *   saveToDdf open failure (symlink loop) — OpenError (605-607)
 *   saveToImage empty QImage     — returns false (826-828)
 *
 * Notes:
 *   - FileHander is used standalone (not attached to DrawBoard). Its
 *     message_waitAnswer signal is unconnected, so returnRet stays default
 *     1, causing the "cancel load" branch — no dialog pops up.
 *   - saveToImage takes PageContext*, not QImage directly. The null-image
 *     guard (826-828) is reached only when context->renderToImage() returns
 *     null, which cannot be reliably forced from the test side. This path
 *     is documented as not covered due to API signature constraints.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QSignalSpy>
#include <unistd.h>

#define protected public
#define private public
#include "filehander.h"
#include "sitemdata.h"
#include "publicApi.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// loadDdf(":/oldBlurItem.ddf") — version 6, EUserCancelLoad_OldBlur
// The message_waitAnswer signal is unconnected, so returnRet stays 1 (cancel).
// ---------------------------------------------------------------------------

TEST(FileHanderOldDdfTest, LoadOldBlurItem_ReturnsNullAndCancelError)
{
    FileHander handler;
    PageContext *result = handler.loadDdf(":/oldBlurItem.ddf");
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(handler.lastError(), FileHander::EUserCancelLoad_OldBlur);
}

TEST(FileHanderOldDdfTest, LoadOldBlurItem_LastErrorDescribeSet)
{
    FileHander handler;
    handler.loadDdf(":/oldBlurItem.ddf");
    EXPECT_FALSE(handler.lastErrorDescribe().isEmpty());
}

// ---------------------------------------------------------------------------
// loadDdf(":/oldPenItem.ddf") — version 9, EUserCancelLoad_OldPen
// ---------------------------------------------------------------------------

TEST(FileHanderOldDdfTest, LoadOldPenItem_ReturnsNullAndCancelError)
{
    FileHander handler;
    PageContext *result = handler.loadDdf(":/oldPenItem.ddf");
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(handler.lastError(), FileHander::EUserCancelLoad_OldPen);
}

TEST(FileHanderOldDdfTest, LoadOldPenItem_LastErrorDescribeSet)
{
    FileHander handler;
    handler.loadDdf(":/oldPenItem.ddf");
    EXPECT_FALSE(handler.lastErrorDescribe().isEmpty());
}

// ---------------------------------------------------------------------------
// loadDdf with user choosing to continue (returnRet = 0)
// Connect a slot that sets returnRet to 0 so the user "chooses to continue".
// If oldPenItem.ddf has pen width <= 0, the continue skip branch (302-307)
// is hit.  If all pen widths are > 0, loading proceeds normally.
// ---------------------------------------------------------------------------

TEST(FileHanderOldDdfTest, LoadOldPenItem_ContinueLoading_DoesNotCancel)
{
    FileHander handler;

    // Connect a slot that sets returnRet = 0 (user chooses to continue loading)
    QObject::connect(&handler, &FileHander::message_waitAnswer,
                     [](const SMessage &, int &ret) {
                         ret = 0;
                     });

    PageContext *result = handler.loadDdf(":/oldPenItem.ddf");

    // With returnRet = 0, loading proceeds.  It should NOT return the
    // cancel error — either it succeeds or fails for another reason.
    EXPECT_NE(handler.lastError(), FileHander::EUserCancelLoad_OldPen);

    if (result != nullptr) {
        delete result;
    }
}

// ---------------------------------------------------------------------------
// loadImage — damaged PNG (valid signature but truncated)
// canRead() returns true but reader.read() fails → 794-797 EDamagedImageFile
// ---------------------------------------------------------------------------

TEST(FileHanderDamagedImageTest, LoadImage_TruncatedPng_ReturnsNullAndDamagedError)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString path = tmpDir.path() + "/truncated.png";

    // Write a valid PNG signature followed by truncated IHDR data.
    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    static const unsigned char pngSig[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
    };
    f.write(reinterpret_cast<const char *>(pngSig), sizeof(pngSig));
    // Partial IHDR chunk — truncated, not enough for a valid image
    f.write("\x00\x00\x00\x0D", 4);
    f.write("IHDR", 4);
    f.write("\x00\x00\x01\x00", 4);  // width = 256
    f.write("\x00\x00\x01", 3);      // truncated height
    f.close();

    FileHander handler;
    QImage img = handler.loadImage(path);
    EXPECT_TRUE(img.isNull());
    EXPECT_EQ(handler.lastError(), FileHander::EDamagedImageFile);
}

// ---------------------------------------------------------------------------
// loadImage — pure garbage bytes with .png extension
// canRead() may return false → 794-797 EDamagedImageFile
// ---------------------------------------------------------------------------

TEST(FileHanderDamagedImageTest, LoadImage_GarbagePng_ReturnsNullAndDamagedError)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString path = tmpDir.path() + "/garbage.png";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("This is not a PNG file at all, just random garbage data.");
    f.close();

    FileHander handler;
    QImage img = handler.loadImage(path);
    EXPECT_TRUE(img.isNull());
    EXPECT_EQ(handler.lastError(), FileHander::EDamagedImageFile);
}

// ---------------------------------------------------------------------------
// checkFileWritable — read-only file → EUnWritableFile (1045-1047)
// ---------------------------------------------------------------------------

TEST(FileHanderWritableTest, CheckFileWritable_ReadOnlyFile_ReturnsFalse)
{
    if (geteuid() == 0) {
        GTEST_SKIP() << "Root bypasses file permissions; skip on root.";
    }

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString path = tmpDir.path() + "/readonly.txt";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("test");
    f.close();

    ASSERT_TRUE(QFile::setPermissions(path, QFileDevice::ReadOwner));

    FileHander handler;
    EXPECT_FALSE(handler.checkFileWritable(path));
    EXPECT_EQ(handler.lastError(), FileHander::EUnWritableFile);
}

// ---------------------------------------------------------------------------
// checkFileWritable — read-only directory → EUnWritableDir (1053-1055)
// ---------------------------------------------------------------------------

TEST(FileHanderWritableTest, CheckFileWritable_ReadOnlyDir_ReturnsFalse)
{
    if (geteuid() == 0) {
        GTEST_SKIP() << "Root bypasses directory permissions; skip on root.";
    }

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString dirPath = tmpDir.path() + "/subdir";
    ASSERT_TRUE(QDir().mkdir(dirPath));

    ASSERT_TRUE(QFile::setPermissions(dirPath,
                QFileDevice::ReadOwner | QFileDevice::ExeOwner));

    // A file that does not exist yet → checkFileWritable checks the parent dir.
    QString nonExistentFile = dirPath + "/newfile.txt";

    FileHander handler;
    EXPECT_FALSE(handler.checkFileWritable(nonExistentFile));
    EXPECT_EQ(handler.lastError(), FileHander::EUnWritableDir);
}

// ---------------------------------------------------------------------------
// saveToDdf — open failure via symlink loop (ELOOP) → OpenError (605-607)
// Construct a→b, b→a symlink loop as target path.
// QFileInfo::exists() returns false, directory is writable, but open()
// fails with ELOOP.
// ---------------------------------------------------------------------------

TEST(FileHanderSaveDdfTest, SaveToDdf_SymlinkLoop_ReturnsOpenError)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());

    QString dirPath = tmpDir.path();
    QString linkA = dirPath + "/a.ddf";
    QString linkB = dirPath + "/b.ddf";

    // Create a symlink loop: a → b, b → a
    if (!QFile::link(linkB, linkA) || !QFile::link(linkA, linkB)) {
        GTEST_SKIP() << "Cannot create symlink loop in this environment.";
    }

    // The loop makes QFileInfo::exists() return false.
    if (QFileInfo(linkA).exists()) {
        GTEST_SKIP() << "Symlink loop target unexpectedly exists; "
                        "ELOOP not enforceable in this environment.";
    }

    // Obtain a valid PageContext from the test harness.
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    PageScene *scene = view->drawScene();
    ASSERT_NE(scene, nullptr);
    PageContext *ctx = scene->pageContext();
    ASSERT_NE(ctx, nullptr);

    // Verify that opening the symlink loop target actually fails.
    QFile testFile(linkA);
    if (testFile.open(QIODevice::WriteOnly)) {
        testFile.close();
        GTEST_SKIP() << "Symlink loop did not cause ELOOP; open() succeeded. "
                        "This environment resolves the loop differently.";
    }

    FileHander handler;
    bool ret = handler.saveToDdf(ctx, linkA);
    EXPECT_FALSE(ret);
    EXPECT_EQ(handler.lastError(), QFileDevice::OpenError);
}

// ---------------------------------------------------------------------------
// saveToImage — empty QImage returns false (826-828)
//
// saveToImage takes PageContext* and calls context->renderToImage() internally.
// The null-image guard at 826-828 is reached only when renderToImage() returns
// a null QImage, which cannot be reliably forced from the test side without
// modifying business code.  This path is documented as not covered due to
// API signature constraints (the function does not accept a QImage directly).
// ---------------------------------------------------------------------------
TEST(FileHanderSaveImageTest, SaveToImage_EmptyImagePath_DocumentedSkip)
{
    GTEST_SKIP() << "saveToImage takes PageContext* (not QImage directly); "
                    "the null-image guard at 826-828 cannot be triggered "
                    "without modifying business code.";
}
