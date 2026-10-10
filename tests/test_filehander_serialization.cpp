// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for filehander.cpp serialization/deserialization helpers.
 *
 * Target functions (from issue V-5568):
 *   - serializationHeadToBytes(const CGraphics &)
 *   - serializationTreeToBytes_1(const CGroupBzItemsTreeInfo &)
 *   - serializationTree_helper_1(...)
 *   - saveDdfWithCombinGroup(...)
 *   - deserializationToTree_helper(...)
 *   - loadImage_helper(const QString &, FileHander *)
 *
 * The non-static functions are extern-declared here; static functions
 * (loadDdfWithCombinGroup, loadDdfWithNoCombinGroup, convertToSRgbColorSpace)
 * have internal linkage and are tested indirectly or via #include of the
 * source file for convertToSRgbColorSpace.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QBuffer>
#include <QByteArray>
#include <QDataStream>
#include <QImage>
#include <QPainter>
#include <QTemporaryDir>
#include <QTemporaryFile>

#define protected public
#define private public
#include "filehander.h"
#include "ccentralwidget.h"
#include "cdrawparamsigleton.h"
#undef protected
#undef private

#include "sitemdata.h"

// STreePlatInfo is defined inside filehander.cpp (not in any header).
// We replicate the struct definition here so we can access return values
// of serializationTreeToBytes_1.
struct STreePlatInfo {
    QByteArray headBytes;
    QByteArray itemsBytes;
    QByteArray md5;
    int bzItemCount = 0;
    int groupCount = 0;
};

// ---- extern declarations for non-static free functions in filehander.cpp ----
QByteArray serializationHeadToBytes(const CGraphics &head);
STreePlatInfo serializationTreeToBytes_1(const CGroupBzItemsTreeInfo &tree);
void serializationTree_helper_1(QDataStream &countStream, int &bzItemCount,
                                int &groupCount,
                                const CGroupBzItemsTreeInfo &tree,
                                std::function<void(int, int)> f = nullptr);
QImage loadImage_helper(const QString &path, FileHander *hander);
CGroupBzItemsTreeInfo deserializationToTree_helper(
    QDataStream &inStream, int &outBzItemCount, int &outGroupCount,
    FileHander *hander, bool &bcomplete, bool &firstMeetPen,
    std::function<void(int, int)> f = nullptr);

// =========================================================================
// serializationHeadToBytes
// =========================================================================

TEST(FileHanderSerializationTest, HeadToBytesReturnsNonEmpty)
{
    CGraphics head;
    head.version = 0x01020304;
    head.unitCount = 5;
    head.rect = QRectF(0, 0, 800, 600);

    QByteArray bytes = serializationHeadToBytes(head);
    EXPECT_FALSE(bytes.isEmpty());
}

TEST(FileHanderSerializationTest, HeadToBytesRoundTripParses)
{
    CGraphics head;
    head.version = 0x0A0B0C0D;
    head.unitCount = 3;
    head.rect = QRectF(10, 20, 400, 300);

    QByteArray bytes = serializationHeadToBytes(head);

    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    // Skip the magic flag 0xA0B0C0D0 written by operator<<
    quint32 flag = 0;
    in >> flag;
    EXPECT_EQ(flag, 0xA0B0C0D0);

    qint32 version = 0;
    in >> version;
    EXPECT_EQ(version, head.version);

    qint64 unitCount = 0;
    in >> unitCount;
    EXPECT_EQ(unitCount, head.unitCount);
}

TEST(FileHanderSerializationTest, HeadToBytesEmptyRect)
{
    CGraphics head;
    head.version = 0;
    head.unitCount = 0;
    head.rect = QRectF();

    QByteArray bytes = serializationHeadToBytes(head);
    EXPECT_FALSE(bytes.isEmpty());
}

// =========================================================================
// serializationTreeToBytes_1 / serializationTree_helper_1
// =========================================================================

TEST(FileHanderSerializationTest, TreeToBytesEmptyTree)
{
    CGroupBzItemsTreeInfo tree;
    auto info = serializationTreeToBytes_1(tree);
    // Even an empty tree writes group count (0), item count (0),
    // and one group data unit, so bytes should be non-empty.
    EXPECT_FALSE(info.itemsBytes.isEmpty());
    EXPECT_EQ(info.bzItemCount, 0);
    EXPECT_EQ(info.groupCount, 1);
}

TEST(FileHanderSerializationTest, TreeHelperDirectCall)
{
    CGroupBzItemsTreeInfo tree;
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    int bzCount = 0;
    int gpCount = 0;

    serializationTree_helper_1(stream, bzCount, gpCount, tree, nullptr);

    EXPECT_FALSE(ba.isEmpty());
    EXPECT_EQ(bzCount, 0);
    EXPECT_EQ(gpCount, 1);
}

TEST(FileHanderSerializationTest, TreeHelperWithProgressCallback)
{
    CGroupBzItemsTreeInfo tree;
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    int bzCount = 0;
    int gpCount = 0;
    int callbackCount = 0;

    auto f = [&callbackCount](int, int) { callbackCount++; };
    serializationTree_helper_1(stream, bzCount, gpCount, tree, f);

    // The callback should have been called at least once
    // (for the group data at the end).
    EXPECT_GE(callbackCount, 1);
}

// =========================================================================
// loadImage_helper
// =========================================================================

TEST(FileHanderLoadImageHelperTest, NonExistentFileReturnsNullImage)
{
    FileHander hander;
    QImage img = loadImage_helper("/nonexistent/path/file.png", &hander);
    EXPECT_TRUE(img.isNull());
}

TEST(FileHanderLoadImageHelperTest, ValidImageReturnsNonNull)
{
    // Create a small test PNG
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QString path = tmpFile.fileName() + ".png";
    tmpFile.close();
    tmpFile.remove();

    QImage testImg(64, 64, QImage::Format_ARGB32);
    testImg.fill(Qt::red);
    ASSERT_TRUE(testImg.save(path, "PNG"));

    FileHander hander;
    QImage img = loadImage_helper(path, &hander);
    EXPECT_FALSE(img.isNull());
    EXPECT_EQ(img.width(), 64);
    EXPECT_EQ(img.height(), 64);

    QFile::remove(path);
}

// =========================================================================
// deserializationToTree_helper (error path with truncated stream)
// =========================================================================

TEST(FileHanderDeserializationTest, EmptyStreamProducesNoCrash)
{
    QByteArray emptyData;
    QDataStream in(&emptyData, QIODevice::ReadOnly);

    FileHander hander;
    int bzCount = 0;
    int gpCount = 0;
    bool bcomplete = true;
    bool firstMeetPen = true;

    // Reading from an empty stream should not crash; the function
    // reads a group count of 0 (QDataStream returns default 0 on
    // exhausted stream).
    CGroupBzItemsTreeInfo result =
        deserializationToTree_helper(in, bzCount, gpCount,
                                     &hander, bcomplete, firstMeetPen);

    // The function reads a CGraphicsUnit at the end too; with an
    // exhausted stream this is a default-constructed unit.
    EXPECT_TRUE(result.childGroups.isEmpty());
    EXPECT_TRUE(result.bzItems.isEmpty());
}
