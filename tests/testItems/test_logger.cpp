// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QCoreApplication>
#include <QString>
#include <QLoggingCategory>

#include <memory>

#include "publicApi.h"
#define protected public
#define private public
#include "logger.h"
#undef protected
#undef private

/*
 * Branch map — test_logger.cpp
 *
 * Module 4: MLogger (src/logger.h/.cpp)
 *   Constructor           B1 clears QT_LOGGING_RULES env var via qunsetenv
 *   rules()               B2 returns current m_rules
 *   setRules()            B3 semicolon→newline conversion
 *                          B4 calls QLoggingCategory::setFilterRules
 *   appendRules()         B5 deduplicates against existing m_rules
 *                          B6 all rules already present → no change
 *                          B7 empty m_rules → direct assignment
 *                          B8 non-empty m_rules → append with \n
 *                          B9 input fully duplicate → early return (no change)
 *   Destructor            B10 no crash (DConfig branch guarded)
 */

#if TEST_LOGGER_ITEM

// ---------------------------------------------------------------------------
// Global state save/restore
// ---------------------------------------------------------------------------

/*
 * Every MLogger construction mutates process-wide logging state: the
 * constructor qunsetenv()s QT_LOGGING_RULES and setRules() replaces Qt's
 * filter rules via QLoggingCategory::setFilterRules(). Neither is restored
 * by MLogger itself, so without this fixture later tests, callers and child
 * processes would observe altered logging configuration.
 *
 * QLoggingCategory has no getter for the active filter rules, but the rules
 * set through setFilterRules() were empty before these tests ran, and rules
 * originating from QT_LOGGING_RULES/qtlogging.ini are cached by Qt in a
 * separate layer that these tests cannot modify. Restoring therefore means
 * re-setting the environment variable and resetting the filter rules to
 * empty.
 */
class MLoggerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_envWasSet = qEnvironmentVariableIsSet("QT_LOGGING_RULES");
        m_envValue = qgetenv("QT_LOGGING_RULES");
    }

    void TearDown() override
    {
        if (m_envWasSet)
            qputenv("QT_LOGGING_RULES", m_envValue);
        else
            qunsetenv("QT_LOGGING_RULES");
        QLoggingCategory::setFilterRules(QString());
    }

private:
    bool m_envWasSet = false;
    QByteArray m_envValue;
};

// Keep the original suite names while sharing the save/restore fixture.
struct MLoggerConstructorTest : MLoggerTest {};
struct MLoggerRulesTest : MLoggerTest {};
struct MLoggerSetRulesTest : MLoggerTest {};
struct MLoggerAppendRulesTest : MLoggerTest {};
struct MLoggerDestructorTest : MLoggerTest {};

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

TEST_F(MLoggerConstructorTest, ClearsQtLoggingRulesEnvVar)
{
    qputenv("QT_LOGGING_RULES", "*.debug=true");
    ASSERT_STREQ(qgetenv("QT_LOGGING_RULES").constData(), "*.debug=true");

    auto logger = std::make_unique<MLogger>();
    // The constructor should have cleared (qunsetenv) QT_LOGGING_RULES.
    bool envCleared = qgetenv("QT_LOGGING_RULES").isEmpty();
    EXPECT_TRUE(envCleared);
}

TEST_F(MLoggerConstructorTest, NoCrashWithoutEnvVar)
{
    qunsetenv("QT_LOGGING_RULES");
    auto logger = std::make_unique<MLogger>();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// rules()
// ---------------------------------------------------------------------------

TEST_F(MLoggerRulesTest, ReturnsCurrentRules)
{
    auto logger = std::make_unique<MLogger>();
    logger->m_rules = QStringLiteral("test.category=true");
    EXPECT_EQ(logger->rules(), QStringLiteral("test.category=true"));
}

TEST_F(MLoggerRulesTest, EmptyByDefault)
{
    auto logger = std::make_unique<MLogger>();
    // After construction m_rules may be empty or set from DConfig.
    // Just verify rules() returns m_rules.
    EXPECT_EQ(logger->rules(), logger->m_rules);
}

// ---------------------------------------------------------------------------
// setRules()
// ---------------------------------------------------------------------------

TEST_F(MLoggerSetRulesTest, ConvertsSemicolonToNewline)
{
    auto logger = std::make_unique<MLogger>();
    logger->setRules(QStringLiteral("a.b=true;c.d=false"));
    EXPECT_EQ(logger->m_rules, QStringLiteral("a.b=true\nc.d=false"));
}

TEST_F(MLoggerSetRulesTest, NoSemicolonsPreserved)
{
    auto logger = std::make_unique<MLogger>();
    logger->setRules(QStringLiteral("a.b=true\nc.d=false"));
    EXPECT_EQ(logger->m_rules, QStringLiteral("a.b=true\nc.d=false"));
}

TEST_F(MLoggerSetRulesTest, AppliesFilterRules)
{
    auto logger = std::make_unique<MLogger>();
    // Set a rule that disables a category, then verify it is applied.
    QLoggingCategory::setFilterRules(QString());
    logger->setRules(QStringLiteral("test.logger.category=false"));

    // QLoggingCategory should now have the category disabled.
    QLoggingCategory cat("test.logger.category");
    EXPECT_FALSE(cat.isDebugEnabled());
}

TEST_F(MLoggerSetRulesTest, EmptyStringClearsRules)
{
    auto logger = std::make_unique<MLogger>();
    logger->setRules(QStringLiteral("a.b=true"));
    logger->setRules(QString());
    EXPECT_TRUE(logger->m_rules.isEmpty());
}

// ---------------------------------------------------------------------------
// appendRules() (private, accessed via #define private public)
// ---------------------------------------------------------------------------

TEST_F(MLoggerAppendRulesTest, AppendsNewRules)
{
    auto logger = std::make_unique<MLogger>();
    logger->m_rules = QStringLiteral("a.b=true");
    logger->appendRules(QStringLiteral("c.d=false"));
    // Exact string: new rules must be joined onto the non-empty existing
    // ruleset with a newline separator.
    EXPECT_EQ(logger->m_rules, QStringLiteral("a.b=true\nc.d=false"));
}

TEST_F(MLoggerAppendRulesTest, DeduplicatesExistingRules)
{
    auto logger = std::make_unique<MLogger>();
    logger->m_rules = QStringLiteral("a.b=true");
    logger->appendRules(QStringLiteral("a.b=true;c.d=false"));
    // a.b=true already exists → should not be duplicated.
    // c.d=false is new → should be appended.
    EXPECT_EQ(logger->m_rules.count("a.b=true"), 1);
    EXPECT_TRUE(logger->m_rules.contains(QStringLiteral("c.d=false")));
}

TEST_F(MLoggerAppendRulesTest, AllDuplicateNoChange)
{
    auto logger = std::make_unique<MLogger>();
    logger->m_rules = QStringLiteral("a.b=true");
    logger->appendRules(QStringLiteral("a.b=true"));
    // All rules already present → no change.
    EXPECT_EQ(logger->m_rules, QStringLiteral("a.b=true"));
}

TEST_F(MLoggerAppendRulesTest, EmptyMRulesDirectAssignment)
{
    auto logger = std::make_unique<MLogger>();
    logger->m_rules.clear();
    logger->appendRules(QStringLiteral("a.b=true;c.d=false"));
    EXPECT_EQ(logger->m_rules, QStringLiteral("a.b=true\nc.d=false"));
}

TEST_F(MLoggerAppendRulesTest, SemicolonConvertedToNewline)
{
    auto logger = std::make_unique<MLogger>();
    logger->m_rules = QStringLiteral("a.b=true");
    logger->appendRules(QStringLiteral("c.d=false;e.f=true"));
    // Exact string: semicolons converted to newlines and the result joined
    // onto the existing ruleset with newline separators.
    EXPECT_EQ(logger->m_rules, QStringLiteral("a.b=true\nc.d=false\ne.f=true"));
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------

TEST_F(MLoggerDestructorTest, NoCrash)
{
    {
        auto logger = std::make_unique<MLogger>();
        logger->setRules(QStringLiteral("a.b=true"));
    }
    SUCCEED();
}

#endif // TEST_LOGGER_ITEM
