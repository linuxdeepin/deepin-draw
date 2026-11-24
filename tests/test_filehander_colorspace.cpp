// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for FileHander color space conversion and path helpers
 * (issue V-9255 area 5)
 *
 * Source: src/service/filehander.cpp
 *
 * Target methods:
 *   convertToSRgbColorSpace (static, Qt6) — non-sRGB → conversion, normal → no conversion
 *   toLegalFile               — file:// URL → local path
 *   pathControl               — non-empty path, no FileArmor D-Bus → false
 *   loadImage                 — exercises convertToSRgbColorSpace internally
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QImage>
#include <QPainter>
#include <QColorSpace>
#include <QFile>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

#define protected public
#define private public
#include "filehander.h"
#include "publicApi.h"
#undef protected
#undef private

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

// =========================================================================
// convertToSRgbColorSpace — non-sRGB color space triggers conversion
// =========================================================================

TEST(FileHanderColorSpace, NonSRgbColorSpace_ConvertedSuccessfully)
{
    // Create a small image with DisplayP3 color space (non-sRGB)
    QImage img(4, 4, QImage::Format_RGB32);
    img.fill(QColor(255, 0, 0));
    img.setColorSpace(QColorSpace(QColorSpace::DisplayP3));

    ASSERT_FALSE(img.isNull());
    ASSERT_TRUE(img.colorSpace().isValid());
    ASSERT_NE(img.colorSpace(), QColorSpace(QColorSpace::SRgb));

    // loadImage calls convertToSRgbColorSpace internally.
    // We test convertToSRgbColorSpace indirectly through loadImage.
    // First save the image to a temp file
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_p3.png";
    ASSERT_TRUE(img.save(filePath, "PNG"));

    // Load it back to verify the file has the non-sRGB color space
    QImage loadedRaw(filePath);
    ASSERT_FALSE(loadedRaw.isNull());
    ASSERT_TRUE(loadedRaw.colorSpace().isValid());

    FileHander handler;
    QImage result = handler.loadImage(filePath);

    // After loading via FileHander (which calls convertToSRgbColorSpace),
    // the image should be converted to sRGB
    ASSERT_FALSE(result.isNull());
    EXPECT_EQ(result.colorSpace(), QColorSpace(QColorSpace::SRgb));
}

TEST(FileHanderColorSpace, NormalSRgbColorSpace_NoConversionNeeded)
{
    // Create a small image with sRGB color space
    QImage img(4, 4, QImage::Format_RGB32);
    img.fill(QColor(0, 255, 0));
    img.setColorSpace(QColorSpace(QColorSpace::SRgb));

    ASSERT_FALSE(img.isNull());

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_srgb.png";
    ASSERT_TRUE(img.save(filePath, "PNG"));

    FileHander handler;
    QImage result = handler.loadImage(filePath);

    // sRGB image should load without conversion, still be sRGB
    ASSERT_FALSE(result.isNull());
    EXPECT_EQ(result.colorSpace(), QColorSpace(QColorSpace::SRgb));
}

TEST(FileHanderColorSpace, NoColorSpace_NoConversionNeeded)
{
    // Create a small image without any color space
    QImage img(4, 4, QImage::Format_RGB32);
    img.fill(QColor(0, 0, 255));

    ASSERT_FALSE(img.isNull());
    ASSERT_FALSE(img.colorSpace().isValid());

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_nocolorspace.png";
    ASSERT_TRUE(img.save(filePath, "PNG"));

    FileHander handler;
    QImage result = handler.loadImage(filePath);

    // Image with no valid color space → no conversion needed
    ASSERT_FALSE(result.isNull());
    SUCCEED();
}

TEST(FileHanderColorSpace, NullImage_ReturnsNull)
{
    QImage nullImg;
    // We can't directly call the static convertToSRgbColorSpace, but loadImage
    // with a non-image file should handle it gracefully.
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/not_an_image.txt";

    QFile f(filePath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("This is not an image file");
    f.close();

    FileHander handler;
    QImage result = handler.loadImage(filePath);

    // Loading a non-image file should return a null image
    EXPECT_TRUE(result.isNull());
}

#endif // QT_VERSION >= Qt6

// =========================================================================
// FileHander::toLegalFile — file:// URL → local path
// =========================================================================

TEST(FileHanderToLegalFile, FileUrl_ConvertedToLocalPath)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString localPath = tmpDir.path() + "/test_legal.png";

    // Create the file so it exists
    QFile f(localPath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("test");
    f.close();

    // Convert to file:// URL
    QString fileUrl = QUrl::fromLocalFile(localPath).toString();
    EXPECT_TRUE(fileUrl.startsWith("file://"));

    // toLegalFile should convert it back to the local path
    QString result = FileHander::toLegalFile(fileUrl);
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(result, localPath);
}

TEST(FileHanderToLegalFile, DirectLocalPath_ReturnedAsIs)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString localPath = tmpDir.path() + "/test_direct.txt";

    QFile f(localPath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("data");
    f.close();

    // Pass the local path directly (not a file:// URL)
    QString result = FileHander::toLegalFile(localPath);
    EXPECT_EQ(result, localPath);
}

TEST(FileHanderToLegalFile, NonExistentPath_ReturnsPathWhenParentDirExists)
{
    // toLegalFile returns the path if the parent directory exists,
    // even if the file itself doesn't exist (isLegalFile checks parent dir).
    QString result = FileHander::toLegalFile("/tmp/nonexistent_file_99999_abc.png");
    EXPECT_FALSE(result.isEmpty());
}

TEST(FileHanderToLegalFile, NonExistentFileUrl_ConvertedToLocalPath)
{
    // toLegalFile converts file:// URL to local path, then checks isLegalFile.
    // Since /tmp exists, isLegalFile returns true even for non-existent files.
    QString fileUrl = QUrl::fromLocalFile("/tmp/nonexistent_file_99999_xyz.png").toString();
    QString result = FileHander::toLegalFile(fileUrl);
    EXPECT_FALSE(result.isEmpty());
}

// =========================================================================
// FileHander::pathControl — non-empty path, no FileArmor → false
// =========================================================================

TEST(FileHanderPathControl, EmptyPath_ReturnsFalse)
{
    EXPECT_FALSE(FileHander::pathControl(""));
}

TEST(FileHanderPathControl, NonEmptyPath_NoFileArmor_ReturnsFalse)
{
    // On a system without the com.deepin.FileArmor1 D-Bus service,
    // the QDBusInterface is invalid → pathControl returns false.
    // This is the expected behavior on a test/CI environment.
    QString path = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)
                       .value(0, "/tmp") + "/test_file.txt";

    bool result = FileHander::pathControl(path);
    // Without FileArmor D-Bus service, iface is invalid → returns false
    EXPECT_FALSE(result);
}

TEST(FileHanderPathControl, NonExistentLocation_ReturnsFalse)
{
    // A path that doesn't match any standard location — still returns false
    // because the D-Bus interface is invalid on this system
    bool result = FileHander::pathControl("/some/random/path/file.txt");
    EXPECT_FALSE(result);
}
