// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for FileHander error/version/dirty/space methods
 * (issue V-5860)
 *
 * Source: src/service/filehander.cpp
 *
 * Target methods:
 *   lastError, lastErrorDescribe,
 *   isVolumeSpaceAvailabel, getDdfVersion, isDdfFileDirty,
 *   setQDataStreamVersion
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <QFile>
#include <QStorageInfo>
#include <QCryptographicHash>

#define protected public
#define private public
#include "publicApi.h"
#undef protected
#undef private
#define protected public
#define private public
#include "filehander.h"
#include "sitemdata.h"
#undef protected
#undef private

// ---------------------------------------------------------------------------
// lastError / lastErrorDescribe
// ---------------------------------------------------------------------------

TEST(FileHanderLastErrorTest, DefaultLastErrorIsNoError)
{
    FileHander handler;
    EXPECT_EQ(handler.lastError(), FileHander::NoError);
}

TEST(FileHanderLastErrorTest, DefaultLastErrorDescribeIsEmpty)
{
    FileHander handler;
    EXPECT_TRUE(handler.lastErrorDescribe().isEmpty());
}

TEST(FileHanderLastErrorTest, ErrorAfterLoadNonExistentFile)
{
    FileHander handler;
    handler.loadDdf("/nonexistent/file.ddf");
    EXPECT_NE(handler.lastError(), FileHander::NoError);
    EXPECT_FALSE(handler.lastErrorDescribe().isEmpty());
}

TEST(FileHanderLastErrorTest, ErrorAfterLoadIllegalFileName)
{
    FileHander handler;
    handler.loadDdf("/tmp/file with spaces and /slashes/.ddf");
    EXPECT_NE(handler.lastError(), FileHander::NoError);
}

// ---------------------------------------------------------------------------
// isVolumeSpaceAvailabel
// ---------------------------------------------------------------------------

TEST(FileHanderIsVolumeSpaceTest, SufficientSpaceReturnsTrue)
{
    FileHander handler;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test.ddf";
    EXPECT_TRUE(handler.isVolumeSpaceAvailabel(filePath, 1));
}

TEST(FileHanderIsVolumeSpaceTest, InsufficientSpaceReturnsFalse)
{
    FileHander handler;
    // Use a path on a read-only filesystem (sysfs) to trigger the
    // isReadOnly() branch which returns false.
    EXPECT_FALSE(handler.isVolumeSpaceAvailabel("/sys/test.ddf", 1));
}

TEST(FileHanderIsVolumeSpaceTest, ZeroNeededBytesReturnsTrue)
{
    FileHander handler;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test.ddf";
    EXPECT_TRUE(handler.isVolumeSpaceAvailabel(filePath, 0));
}

// ---------------------------------------------------------------------------
// getDdfVersion
// ---------------------------------------------------------------------------

TEST(FileHanderGetDdfVersionTest, NonExistentFileReturnsUnknown)
{
    FileHander handler;
    EXPECT_EQ(handler.getDdfVersion("/nonexistent/file.ddf"), EDdfUnknowed);
}

TEST(FileHanderGetDdfVersionTest, EmptyFileReturnsUnknown)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.close();
    // Empty file: getVersion will read garbage and return EDdfUnknowed
    // (no valid header magic 0xA0B0C0D0, so it returns EDdf5_8_0_Base
    //  according to getVersion logic — but an empty file has no data to
    //  read, so the stream read fails and returns EDdfUnknowed)
    EDdfVersion ver = handler.getDdfVersion(tmpFile.fileName());
    // Empty file → stream read fails → EDdfUnknowed or EDdf5_8_0_Base
    // depending on Qt version; both are valid for "no valid header"
    EXPECT_TRUE(ver == EDdfUnknowed || ver == EDdf5_8_0_Base);
}

TEST(FileHanderGetDdfVersionTest, FileWithValidHeaderReturnsVersion)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QDataStream stream(&tmpFile);
    quint32 magic = static_cast<quint32>(0xA0B0C0D0);
    int version = static_cast<int>(EDdf5_8_0_20);
    stream << magic;
    stream << version;
    tmpFile.close();

    EXPECT_EQ(handler.getDdfVersion(tmpFile.fileName()), EDdf5_8_0_20);
}

TEST(FileHanderGetDdfVersionTest, FileWithoutMagicReturnsBaseVersion)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    // Write some data but not the magic header
    tmpFile.write("Hello World", 11);
    tmpFile.close();

    // getVersion reads first 4 bytes; if not 0xA0B0C0D0, returns
    // EDdf5_8_0_Base
    EXPECT_EQ(handler.getDdfVersion(tmpFile.fileName()), EDdf5_8_0_Base);
}

// ---------------------------------------------------------------------------
// isDdfFileDirty
// ---------------------------------------------------------------------------

TEST(FileHanderIsDdfFileDirtyTest, NonExistentFileReturnsTrue)
{
    FileHander handler;
    EXPECT_TRUE(handler.isDdfFileDirty("/nonexistent/file.ddf"));
}

TEST(FileHanderIsDdfFileDirtyTest, OldVersionFileReturnsTrue)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QDataStream stream(&tmpFile);
    // Write a version < EDdf5_8_0_20 (old version → dirty=true)
    quint32 magic = static_cast<quint32>(0xA0B0C0D0);
    int version = static_cast<int>(EDdf5_8_0_Base); // 0
    stream << magic;
    stream << version;
    tmpFile.write("some data");
    tmpFile.close();

    EXPECT_TRUE(handler.isDdfFileDirty(tmpFile.fileName()));
}

TEST(FileHanderIsDdfFileDirtyTest, ValidMd5FileReturnsFalse)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QDataStream stream(&tmpFile);
    quint32 magic = static_cast<quint32>(0xA0B0C0D0);
    int version = static_cast<int>(EDdf5_8_0_20);
    stream << magic;
    stream << version;
    tmpFile.write("content data here");
    tmpFile.close();

    // Now compute and append the correct MD5
    QFile file(tmpFile.fileName());
    ASSERT_TRUE(file.open(QFile::ReadOnly));
    QByteArray allData = file.readAll();
    file.close();

    QByteArray content = allData.left(allData.size());
    QByteArray md5 = QCryptographicHash::hash(content, QCryptographicHash::Md5);

    ASSERT_TRUE(file.open(QFile::WriteOnly | QFile::Append));
    file.write(md5);
    file.close();

    EXPECT_FALSE(handler.isDdfFileDirty(tmpFile.fileName()));
}

TEST(FileHanderIsDdfFileDirtyTest, CorruptedMd5FileReturnsTrue)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QDataStream stream(&tmpFile);
    quint32 magic = static_cast<quint32>(0xA0B0C0D0);
    int version = static_cast<int>(EDdf5_8_0_20);
    stream << magic;
    stream << version;
    tmpFile.write("content data here");
    // Append wrong MD5 (16 zero bytes)
    tmpFile.write(QByteArray(16, '\0'));
    tmpFile.close();

    EXPECT_TRUE(handler.isDdfFileDirty(tmpFile.fileName()));
}

// ---------------------------------------------------------------------------
// setQDataStreamVersion
// ---------------------------------------------------------------------------

TEST(FileHanderSetQDataStreamVersionTest, OldVersionSetsQt56)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QDataStream writeStream(&tmpFile);
    quint32 magic = static_cast<quint32>(0xA0B0C0D0);
    int version = static_cast<int>(EDdf5_8_0_Base);
    writeStream << magic;
    writeStream << version;
    tmpFile.close();

    QDataStream readStream;
    // CGraphics head — just default-constructed; version field is 0
    CGraphics head;
    handler.setQDataStreamVersion(tmpFile.fileName(), readStream, head);
    // For old versions (<= EDdf5_8_0_84_LATER), setVersion(Qt_5_6) is called
    EXPECT_EQ(readStream.version(), QDataStream::Qt_5_6);
}

TEST(FileHanderSetQDataStreamVersionTest, NewVersionWithNoDeviceSetsQt56)
{
    FileHander handler;
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QDataStream writeStream(&tmpFile);
    quint32 magic = static_cast<quint32>(0xA0B0C0D0);
    int version = static_cast<int>(EDdf5_9_0_3_LATER);
    writeStream << magic;
    writeStream << version;
    tmpFile.close();

    QDataStream readStream; // no device attached
    CGraphics head;
    handler.setQDataStreamVersion(tmpFile.fileName(), readStream, head);
    // data.device() == nullptr → setVersion(Qt_5_6)
    EXPECT_EQ(readStream.version(), QDataStream::Qt_5_6);
}
