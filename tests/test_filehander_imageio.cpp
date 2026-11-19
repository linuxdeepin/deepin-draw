// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for FileHander::loadImage and saveToImage real-data paths
 * (issue V-8981 §4).
 *
 * Source: src/service/filehander.cpp
 *
 * Covers:
 *   - loadImage: valid PNG/JPG load, unsupported suffix, damaged file,
 *     non-existent path.
 *   - saveToImage: PNG save, JPG save, PDF branch, TIFF/TIF branch,
 *     null-render failure, illegal path failure.
 *   - (Qt6) convertToSRgbColorSpace is exercised implicitly through loadImage.
 *
 * These tests use real images written to QTemporaryDir and real PageContext
 * rendering where needed. The application harness (drawApp) must be running.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QPixmap>

#define protected public
#define private public
#include "filehander.h"
#include "publicApi.h"
#include "cdrawparamsigleton.h"
#undef protected
#undef private

// Helper: create a valid small PNG at the given path.
static QString createValidPng(const QString &dirPath, const QString &name = "test.png")
{
    QString path = dirPath + "/" + name;
    QImage img(32, 32, QImage::Format_ARGB32);
    img.fill(Qt::red);
    QPainter p(&img);
    p.setPen(Qt::blue);
    p.drawLine(0, 0, 31, 31);
    p.end();
    img.save(path, "PNG");
    return path;
}

// Helper: create a valid small JPG at the given path.
static QString createValidJpg(const QString &dirPath, const QString &name = "test.jpg")
{
    QString path = dirPath + "/" + name;
    QImage img(32, 32, QImage::Format_RGB32);
    img.fill(Qt::green);
    QPainter p(&img);
    p.setPen(Qt::red);
    p.drawRect(2, 2, 28, 28);
    p.end();
    img.save(path, "JPG");
    return path;
}

// Helper: create a damaged file (garbage bytes with .png suffix).
static QString createDamagedFile(const QString &dirPath, const QString &name = "damaged.png")
{
    QString path = dirPath + "/" + name;
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write("NOT_A_PNG_FILE_GARBAGE_DATA_1234567890");
    f.close();
    return path;
}

// Helper: get current PageContext from the running app.
static PageContext *getCurrentContext()
{
    Page *page = drawApp->topMainWindow()->drawBoard()->currentPage();
    return page ? page->context() : nullptr;
}

// ===========================================================================
// loadImage — valid image loading
// ===========================================================================

TEST(FileHanderLoadImageTest, LoadValidPngReturnsNonEmptyImage)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QString pngPath = createValidPng(dir.path());

    FileHander fh;
    QImage img = fh.loadImage(pngPath);
    EXPECT_FALSE(img.isNull());
    EXPECT_EQ(fh.lastError(), FileHander::NoError);
}

TEST(FileHanderLoadImageTest, LoadValidJpgReturnsNonEmptyImage)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QString jpgPath = createValidJpg(dir.path());

    FileHander fh;
    QImage img = fh.loadImage(jpgPath);
    EXPECT_FALSE(img.isNull());
    EXPECT_EQ(fh.lastError(), FileHander::NoError);
}

// ===========================================================================
// loadImage — error paths
// ===========================================================================

TEST(FileHanderLoadImageTest, LoadDamagedFileReturnsError)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QString damagedPath = createDamagedFile(dir.path());

    FileHander fh;
    QImage img = fh.loadImage(damagedPath);
    EXPECT_TRUE(img.isNull());
    EXPECT_EQ(fh.lastError(), FileHander::EDamagedImageFile);
}

TEST(FileHanderLoadImageTest, LoadNonExistentFileReturnsError)
{
    FileHander fh;
    QImage img = fh.loadImage("/tmp/no_such_dir_12345/nonexistent.png");
    EXPECT_TRUE(img.isNull());
    EXPECT_NE(fh.lastError(), FileHander::NoError);
}

TEST(FileHanderLoadImageTest, LoadUnsupportedSuffixReturnsError)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QString path = dir.path() + "/test.xyz";
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write("data");
    f.close();

    FileHander fh;
    QImage img = fh.loadImage(path);
    EXPECT_TRUE(img.isNull());
    EXPECT_NE(fh.lastError(), FileHander::NoError);
}

TEST(FileHanderLoadImageTest, LoadEmptyPathReturnsEmptyImage)
{
    FileHander fh;
    QImage img = fh.loadImage("");
    EXPECT_TRUE(img.isNull());
}

// ===========================================================================
// saveToImage — valid saves
// ===========================================================================

TEST(FileHanderSaveToImageTest, SavePngSucceeds)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    PageContext *ctx = getCurrentContext();
    ASSERT_NE(ctx, nullptr);

    QString path = dir.path() + "/output.png";
    FileHander fh;
    bool ok = fh.saveToImage(ctx, path, QSize(100, 100), 100);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(QFileInfo::exists(path));
}

TEST(FileHanderSaveToImageTest, SaveJpgSucceeds)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    PageContext *ctx = getCurrentContext();
    ASSERT_NE(ctx, nullptr);

    QString path = dir.path() + "/output.jpg";
    FileHander fh;
    bool ok = fh.saveToImage(ctx, path, QSize(100, 100), 90);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(QFileInfo::exists(path));
}

TEST(FileHanderSaveToImageTest, SavePdfSucceeds)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    PageContext *ctx = getCurrentContext();
    ASSERT_NE(ctx, nullptr);

    QString path = dir.path() + "/output.pdf";
    FileHander fh;
    bool ok = fh.saveToImage(ctx, path, QSize(100, 100), 100);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(QFileInfo::exists(path));
}

TEST(FileHanderSaveToImageTest, SaveTiffSucceeds)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    PageContext *ctx = getCurrentContext();
    ASSERT_NE(ctx, nullptr);

    QString path = dir.path() + "/output.tiff";
    FileHander fh;
    bool ok = fh.saveToImage(ctx, path, QSize(100, 100), 100);
    // TIFF support depends on Qt image format plugins; if it fails the
    // error should be non-zero.
    if (ok) {
        EXPECT_TRUE(QFileInfo::exists(path));
    } else {
        EXPECT_NE(fh.lastError(), FileHander::NoError);
    }
}

// ===========================================================================
// saveToImage — failure paths
// ===========================================================================

TEST(FileHanderSaveToImageTest, SaveToEmptyPathReturnsFalse)
{
    FileHander fh;
    bool ok = fh.saveToImage(nullptr, "");
    EXPECT_FALSE(ok);
}

TEST(FileHanderSaveToImageTest, SaveToIllegalPathReturnsFalse)
{
    PageContext *ctx = getCurrentContext();
    ASSERT_NE(ctx, nullptr);

    FileHander fh;
    // Non-existent parent directory.
    bool ok = fh.saveToImage(ctx, "/tmp/no_such_dir_12345/output.png");
    EXPECT_FALSE(ok);
}

TEST(FileHanderSaveToImageTest, SaveToDirectoryPathReturnsFalse)
{
    PageContext *ctx = getCurrentContext();
    ASSERT_NE(ctx, nullptr);

    FileHander fh;
    // A directory path (no actual file component) should fail to save.
    bool ok = fh.saveToImage(ctx, "/tmp/");
    EXPECT_FALSE(ok);
}
