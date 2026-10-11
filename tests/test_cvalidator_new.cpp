// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CIntValidator methods (issue V-5575)
 *
 * Source: src/utils/cvalidator.cpp
 *
 * Target methods:
 *   CIntValidator::validate, fixup, setEmptyStrToBottom
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QIntValidator>
#include <QString>

#define protected public
#define private public
#include "cvalidator.h"
#undef protected
#undef private

// =========================================================================
// CIntValidator::validate
// =========================================================================

TEST(CIntValidatorValidateTest, EmptyStringDefaultReturnsIntermediate)
{
    CIntValidator validator(0, 100);
    QString s;
    int n = 0;
    // With m_emptyStrIsBottom=true (default), empty string delegates
    // to QIntValidator::validate which returns Intermediate.
    EXPECT_EQ(validator.validate(s, n), QValidator::Intermediate);
}

TEST(CIntValidatorValidateTest, EmptyStringNotBottomReturnsAcceptable)
{
    CIntValidator validator(0, 100);
    validator.setEmptyStrToBottom(false);
    QString s;
    int n = 0;
    // When m_emptyStrIsBottom is false, empty string returns
    // Acceptable directly (see CIntValidator::validate).
    EXPECT_EQ(validator.validate(s, n), QValidator::Acceptable);
}

TEST(CIntValidatorValidateTest, ValidInRangeReturnsAcceptable)
{
    CIntValidator validator(0, 100);
    QString s = "50";
    int n = 0;
    EXPECT_EQ(validator.validate(s, n), QValidator::Acceptable);
}

TEST(CIntValidatorValidateTest, OutOfRangeReturnsIntermediate)
{
    CIntValidator validator(0, 100);
    QString s = "200";
    int n = 0;
    // QIntValidator::validate returns Intermediate for "200" with
    // range(0,100) in Qt6 (value may become valid with editing).
    EXPECT_EQ(validator.validate(s, n), QValidator::Intermediate);
}

TEST(CIntValidatorValidateTest, ValidAtBottomReturnsAcceptable)
{
    CIntValidator validator(10, 100);
    QString s = "10";
    int n = 0;
    EXPECT_EQ(validator.validate(s, n), QValidator::Acceptable);
}

TEST(CIntValidatorValidateTest, ValidAtTopReturnsAcceptable)
{
    CIntValidator validator(0, 100);
    QString s = "100";
    int n = 0;
    EXPECT_EQ(validator.validate(s, n), QValidator::Acceptable);
}

// =========================================================================
// CIntValidator::fixup
// =========================================================================

TEST(CIntValidatorFixupTest, EmptyStringDefaultSetsToBottom)
{
    CIntValidator validator(10, 100);
    QString s;
    validator.fixup(s);
    EXPECT_EQ(s, QStringLiteral("10"));
}

TEST(CIntValidatorFixupTest, EmptyStringNotBottomNoChange)
{
    CIntValidator validator(10, 100);
    validator.setEmptyStrToBottom(false);
    QString s;
    validator.fixup(s);
    EXPECT_TRUE(s.isEmpty());
}

TEST(CIntValidatorFixupTest, NonEmptyStringSetsToBottom)
{
    CIntValidator validator(5, 100);
    QString s = "abc";
    validator.fixup(s);
    EXPECT_EQ(s, QStringLiteral("5"));
}

TEST(CIntValidatorFixupTest, NonEmptyStringNotBottomStillSetsToBottom)
{
    CIntValidator validator(7, 100);
    validator.setEmptyStrToBottom(false);
    QString s = "xyz";
    validator.fixup(s);
    EXPECT_EQ(s, QStringLiteral("7"));
}

// =========================================================================
// CIntValidator::setEmptyStrToBottom
// =========================================================================

TEST(CIntValidatorSetEmptyStrToBottomTest, DefaultIsTrue)
{
    CIntValidator validator(0, 100);
    EXPECT_TRUE(validator.m_emptyStrIsBottom);
}

TEST(CIntValidatorSetEmptyStrToBottomTest, SetFalse)
{
    CIntValidator validator(0, 100);
    validator.setEmptyStrToBottom(false);
    EXPECT_FALSE(validator.m_emptyStrIsBottom);
}

TEST(CIntValidatorSetEmptyStrToBottomTest, SetTrueAfterFalse)
{
    CIntValidator validator(0, 100);
    validator.setEmptyStrToBottom(false);
    EXPECT_FALSE(validator.m_emptyStrIsBottom);
    validator.setEmptyStrToBottom(true);
    EXPECT_TRUE(validator.m_emptyStrIsBottom);
}
