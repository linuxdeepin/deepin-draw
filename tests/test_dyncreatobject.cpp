// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for ClassObjectFactory methods (issue V-5575)
 *
 * Source: src/service/dyncreatobject.cpp
 *
 * Target methods:
 *   ClassObjectFactory::registerClass, creatClassObject
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "dyncreatobject.h"
#undef protected
#undef private

// Test factory function that returns a unique marker address.
static void *createTestObject()
{
    static int marker = 42;
    return &marker;
}

// Another test factory function.
static void *createAnotherObject()
{
    static double marker = 3.14;
    return &marker;
}

// =========================================================================
// ClassObjectFactory::registerClass / creatClassObject
// =========================================================================

TEST(ClassObjectFactoryTest, RegisterAndCreate)
{
    ClassObjectFactory::registerClass(
        QStringLiteral("TestObject_V5575"), createTestObject);

    void *obj = ClassObjectFactory::creatClassObject(
        QStringLiteral("TestObject_V5575"));
    EXPECT_NE(obj, nullptr);
    EXPECT_EQ(obj, createTestObject());
}

TEST(ClassObjectFactoryTest, CreateUnknownReturnsNull)
{
    void *obj = ClassObjectFactory::creatClassObject(
        QStringLiteral("NonExistentClass_V5575"));
    EXPECT_EQ(obj, nullptr);
}

TEST(ClassObjectFactoryTest, RegisterMultipleAndCreate)
{
    ClassObjectFactory::registerClass(
        QStringLiteral("TestObjectA_V5575"), createTestObject);
    ClassObjectFactory::registerClass(
        QStringLiteral("TestObjectB_V5575"), createAnotherObject);

    void *objA = ClassObjectFactory::creatClassObject(
        QStringLiteral("TestObjectA_V5575"));
    void *objB = ClassObjectFactory::creatClassObject(
        QStringLiteral("TestObjectB_V5575"));

    EXPECT_NE(objA, nullptr);
    EXPECT_NE(objB, nullptr);
    EXPECT_NE(objA, objB);
}

TEST(ClassObjectFactoryTest, RegisterOverwriteAndCreate)
{
    ClassObjectFactory::registerClass(
        QStringLiteral("OverwriteTest_V5575"), createTestObject);
    ClassObjectFactory::registerClass(
        QStringLiteral("OverwriteTest_V5575"), createAnotherObject);

    void *obj = ClassObjectFactory::creatClassObject(
        QStringLiteral("OverwriteTest_V5575"));
    EXPECT_NE(obj, nullptr);
    EXPECT_EQ(obj, createAnotherObject());
}
