// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CFileWatcher destructor path and MLogger
 * (issue V-8626)
 *
 * Existing test_cviewmanagement.cpp covers clear/doRun but NOT the destructor.
 * No existing logger tests exist (despite TEST_LOGGER_ITEM define).
 *
 * Source:
 *   src/frame/cviewmanagement.cpp
 *   src/logger.cpp
 */

#include <gtest/gtest.h>
#include <QCoreApplication>

#define protected public
#define private public
#include "cviewmanagement.h"
#include "logger.h"
#undef protected
#undef private

#if TEST_LOGGER_ITEM

// ===========================================================================
// CFileWatcher destructor
// ===========================================================================

TEST(CFileWatcherDestructorTest, DestructorAfterConstructionNoCrash)
{
    // Construct and immediately destruct — exercises the clear() call in ~CFileWatcher.
    // Only test the no-watch path: adding a watch then destructing triggers
    // qFatal in clear() due to inotify/thread lifecycle (source defect, not testable).
    {
        CFileWatcher watcher;
        // Just construct and let it go out of scope
    }
    SUCCEED();
}

// ===========================================================================
// MLogger
// ===========================================================================

TEST(MLoggerTest, ConstructorDoesNotCrash)
{
    MLogger logger;
    // After construction, m_rules should be set (from env or config)
    // rules() may be empty when no env/config rules are set — just verify no crash
}

TEST(MLoggerTest, SetRulesReplacesSemicolons)
{
    MLogger logger;
    logger.setRules("*.debug=true;*.info=false");

    // setRules replaces ';' with '\n'
    EXPECT_TRUE(logger.rules().contains("\n"));
    EXPECT_FALSE(logger.rules().contains(";"));
    EXPECT_TRUE(logger.rules().contains("*.debug=true"));
}

TEST(MLoggerTest, SetRulesEmptyString)
{
    MLogger logger;
    logger.setRules("");
    EXPECT_TRUE(logger.rules().isEmpty());
}

TEST(MLoggerTest, AppendRulesAddsNewRules)
{
    MLogger logger;
    logger.setRules("*.debug=true");

    // appendRules is private — we use #define private public to access it
    logger.appendRules("*.warning=true");

    // The appended rule should be in m_rules
    EXPECT_TRUE(logger.rules().contains("*.warning=true"));
    EXPECT_TRUE(logger.rules().contains("*.debug=true"));
}

TEST(MLoggerTest, AppendRulesSkipsDuplicates)
{
    MLogger logger;
    logger.setRules("*.debug=true");

    // Appending an already-present rule should not duplicate it
    logger.appendRules("*.debug=true");

    // Should contain exactly one occurrence
    int count = logger.rules().count("*.debug=true");
    EXPECT_EQ(count, 1);
}

TEST(MLoggerTest, AppendRulesEmptyNoChange)
{
    MLogger logger;
    logger.setRules("*.debug=true");
    QString before = logger.rules();

    logger.appendRules("");

    // Appending empty should not change rules
    EXPECT_EQ(logger.rules(), before);
}

#endif // TEST_LOGGER_ITEM
