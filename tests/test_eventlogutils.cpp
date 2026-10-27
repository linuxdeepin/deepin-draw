// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for Eventlogutils (issue V-6699).
 *
 * Source: src/utils/eventlogutils.cpp
 *
 * Target methods:
 *   GetInstance, writeLogs, constructor
 *   Enum EventTID constant values
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define private public
#define protected public
#include "eventlogutils.h"
#undef private
#undef protected

#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <string>

// =========================================================================
// GetInstance tests
// =========================================================================

TEST(Eventlogutils, GetInstance_ReturnsNonNull)
{
    Eventlogutils *inst = Eventlogutils::GetInstance();
    ASSERT_NE(inst, nullptr);
}

TEST(Eventlogutils, GetInstance_ReturnsSameInstance)
{
    Eventlogutils *inst1 = Eventlogutils::GetInstance();
    Eventlogutils *inst2 = Eventlogutils::GetInstance();
    EXPECT_EQ(inst1, inst2);
}

// =========================================================================
// Enum EventTID constant values
// =========================================================================

TEST(Eventlogutils, EventTID_ConstantValues)
{
    EXPECT_EQ(static_cast<int>(Eventlogutils::OpenTime), 1000000000);
    EXPECT_EQ(static_cast<int>(Eventlogutils::CloseTime), 1000000001);
    EXPECT_EQ(static_cast<int>(Eventlogutils::StartUp), 1000000003);
    EXPECT_EQ(static_cast<int>(Eventlogutils::Quit), 1000000004);
}

// =========================================================================
// writeLogs tests
// =========================================================================

TEST(EventlogutilsWriteLogs, NullWriteEventLogFunc_NoCrash)
{
    Eventlogutils *inst = Eventlogutils::GetInstance();
    ASSERT_NE(inst, nullptr);

    // In typical headless environment the library is not loaded,
    // so writeEventLogFunc should be nullptr
    auto original = inst->writeEventLogFunc;
    inst->writeEventLogFunc = nullptr;

    QJsonObject data;
    data["tid"] = Eventlogutils::OpenTime;
    data["body"] = QStringLiteral("test event");

    EXPECT_NO_FATAL_FAILURE(inst->writeLogs(data));

    // Restore original
    inst->writeEventLogFunc = original;
}

TEST(EventlogutilsWriteLogs, WithStubFunction_CalledWithCompactJson)
{
    Eventlogutils *inst = Eventlogutils::GetInstance();
    ASSERT_NE(inst, nullptr);

    static int callCount = 0;
    static std::string lastData;
    callCount = 0;
    lastData.clear();

    auto stub = [](const std::string &eventdata) {
        callCount++;
        lastData = eventdata;
    };

    // Save original and install stub
    auto original = inst->writeEventLogFunc;
    inst->writeEventLogFunc = stub;

    QJsonObject data;
    data["tid"] = Eventlogutils::OpenTime;
    data["body"] = QStringLiteral("test event");

    inst->writeLogs(data);

    // Restore original
    inst->writeEventLogFunc = original;

    EXPECT_EQ(callCount, 1);

    // Verify the data passed is compact JSON
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(lastData).toUtf8());
    EXPECT_FALSE(doc.isNull());
    EXPECT_EQ(doc.object().value("tid").toInt(), Eventlogutils::OpenTime);
    EXPECT_EQ(doc.object().value("body").toString(), QStringLiteral("test event"));

    // Compact format: no extra whitespace/newlines
    EXPECT_FALSE(lastData.find('\n') != std::string::npos);
    EXPECT_FALSE(lastData.find("  ") != std::string::npos);
}

TEST(EventlogutilsWriteLogs, EmptyJsonObject_NoCrash)
{
    Eventlogutils *inst = Eventlogutils::GetInstance();
    ASSERT_NE(inst, nullptr);

    auto original = inst->writeEventLogFunc;
    inst->writeEventLogFunc = nullptr;

    QJsonObject empty;
    EXPECT_NO_FATAL_FAILURE(inst->writeLogs(empty));

    // Restore original
    inst->writeEventLogFunc = original;
}

// =========================================================================
// Destructor / cleanup path
// =========================================================================

TEST(Eventlogutils, InstanceAccess_NoCrash)
{
    // Verify getting instance and calling writeLogs with nullptr func
    // does not crash across multiple calls
    Eventlogutils *inst = Eventlogutils::GetInstance();
    ASSERT_NE(inst, nullptr);

    auto original = inst->writeEventLogFunc;
    inst->writeEventLogFunc = nullptr;
    for (int i = 0; i < 5; ++i) {
        QJsonObject data;
        data["tid"] = Eventlogutils::StartUp;
        inst->writeLogs(data);
    }
    // Restore original
    inst->writeEventLogFunc = original;
    SUCCEED();
}
