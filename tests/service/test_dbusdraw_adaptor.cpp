// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for dbusdraw_adaptor: openFiles/openFile forwarding to the
 * parent's openFiles slot (plain path / file url / missing / remote cases).
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTemporaryDir>
#include <QUrl>
#include <QVariant>
#include <QWidget>

#include "dbusdraw_adaptor.h"

// Harness: plain QObject parent that records the forwarded openFiles calls.
class AdaptorHost : public QObject
{
    Q_OBJECT
public:
    explicit AdaptorHost(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void openFiles(QStringList paths, bool open)
    {
        lastPaths = paths;
        lastOpen = open;
        ++callCount;
    }

    QStringList lastPaths;
    bool lastOpen = false;
    int callCount = 0;
};

// Harness: QWidget parent variant, needed to exercise show/raise branches.
class AdaptorHostWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdaptorHostWidget(QWidget *parent = nullptr) : QWidget(parent) {}

    Q_INVOKABLE void openFiles(QStringList paths, bool open)
    {
        lastPaths = paths;
        lastOpen = open;
        ++callCount;
    }

    QStringList lastPaths;
    bool lastOpen = false;
    int callCount = 0;
};

// --- Constructor ---

TEST(DbusdrawAdaptorTest, ConstructorAttachesToParent)
{
    AdaptorHost host;
    dbusdraw_adaptor adaptor(&host);

    EXPECT_EQ(adaptor.parent(), &host);
}

// --- openFiles ---

TEST(DbusdrawAdaptorTest, OpenFilesEmptyListForwardsEmptyAndReturnsTrue)
{
    AdaptorHost host;
    dbusdraw_adaptor adaptor(&host);

    bool result = adaptor.openFiles(QList<QVariant>());

    EXPECT_TRUE(result);
    EXPECT_EQ(host.callCount, 1);
    EXPECT_TRUE(host.lastPaths.isEmpty());
    EXPECT_TRUE(host.lastOpen);
}

TEST(DbusdrawAdaptorTest, OpenFilesConvertsVariantsToStrings)
{
    AdaptorHost host;
    dbusdraw_adaptor adaptor(&host);

    QList<QVariant> paths;
    paths << QVariant(QString("a.png")) << QVariant(QString("b.ddf"));

    bool result = adaptor.openFiles(paths);

    EXPECT_TRUE(result);
    EXPECT_EQ(host.callCount, 1);
    EXPECT_EQ(host.lastPaths, QStringList() << "a.png" << "b.ddf");
    EXPECT_TRUE(host.lastOpen);
}

TEST(DbusdrawAdaptorTest, OpenFilesEmptyListShowsWidgetParentAndForwards)
{
    AdaptorHostWidget host;
    dbusdraw_adaptor adaptor(&host);

    bool result = adaptor.openFiles(QList<QVariant>());

    EXPECT_TRUE(result);
    EXPECT_TRUE(host.isVisible());
    EXPECT_EQ(host.callCount, 1);
    EXPECT_TRUE(host.lastPaths.isEmpty());
}

// --- openFile ---

TEST(DbusdrawAdaptorTest, OpenFileExistingFileForwardsPathAndReturnsTrue)
{
    QTemporaryDir dir;
    QString filePath = dir.path() + "/exist.ddf";
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.close();
    ASSERT_TRUE(QFileInfo::exists(filePath));

    AdaptorHostWidget host;
    dbusdraw_adaptor adaptor(&host);

    bool result = adaptor.openFile(filePath);

    EXPECT_TRUE(result);
    EXPECT_EQ(host.callCount, 1);
    EXPECT_EQ(host.lastPaths, QStringList() << filePath);
    EXPECT_TRUE(host.lastOpen);
}

TEST(DbusdrawAdaptorTest, OpenFileExistingFileUrlResolvesToLocalPath)
{
    QTemporaryDir dir;
    QString filePath = dir.path() + "/fromurl.ddf";
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.close();

    AdaptorHostWidget host;
    dbusdraw_adaptor adaptor(&host);

    QString urlString = QUrl::fromLocalFile(filePath).toString();
    bool result = adaptor.openFile(urlString);

    EXPECT_TRUE(result);
    EXPECT_EQ(host.callCount, 1);
    EXPECT_EQ(host.lastPaths, QStringList() << filePath);
}

TEST(DbusdrawAdaptorTest, OpenFileMissingPathReturnsFalseWithoutForwarding)
{
    AdaptorHostWidget host;
    dbusdraw_adaptor adaptor(&host);

    bool result = adaptor.openFile("/nonexistent/path/missing.ddf");

    EXPECT_FALSE(result);
    EXPECT_EQ(host.callCount, 0);
    EXPECT_TRUE(host.isVisible());  // parent still activated
}

TEST(DbusdrawAdaptorTest, OpenFileRemoteUrlReturnsFalse)
{
    AdaptorHostWidget host;
    dbusdraw_adaptor adaptor(&host);

    bool result = adaptor.openFile("http://example.com/remote.png");

    EXPECT_FALSE(result);
    EXPECT_EQ(host.callCount, 0);
}

TEST(DbusdrawAdaptorTest, OpenFileNonWidgetParentReturnsFalse)
{
    QTemporaryDir dir;
    QString filePath = dir.path() + "/hostless.ddf";
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.close();

    AdaptorHost host;
    dbusdraw_adaptor adaptor(&host);

    bool result = adaptor.openFile(filePath);

    EXPECT_FALSE(result);
    EXPECT_EQ(host.callCount, 0);
    EXPECT_EQ(host.lastOpen, false);
}

#include "test_dbusdraw_adaptor.moc"
