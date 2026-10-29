// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for dynamic registration helpers (issue V-7155)
 *
 * Source: src/service/dyncreatobject.h/.cpp
 *         src/drawshape/drawItems/bzItems/cgraphicsitem.h/.cpp
 *
 * Branch map:
 *   B1  creatObject<T> with registered name → returns non-null T*
 *   B2  creatObject<T> with unregistered name → returns nullptr
 *   B3  ClassRegisterHelper constructor registers class (creatClassObject finds it)
 *   B4  RegistItemHelper constructor registers item (no crash)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QObject>
#include <QString>

#include "dyncreatobject.h"
#include "drawshape/drawItems/bzItems/cgraphicsitem.h"
#include "drawshape/globaldefine.h"

#if TEST_DYNREG_ITEM

// Factory function returning a pointer to a static QObject.
static void *createQObjectFactory()
{
    static QObject s_obj;
    return &s_obj;
}

// =========================================================================
// ClassObjectFactory::creatObject<T>
// =========================================================================

TEST(CreatObjectTemplateTest, RegisteredName_ReturnsNonNull)
{
    // B1: creatObject<T> with registered name → returns non-null T*
    ClassObjectFactory::registerClass(
        QStringLiteral("CreatObjTest_V7155"), createQObjectFactory);

    QObject *obj = ClassObjectFactory::creatObject<QObject>(
        QStringLiteral("CreatObjTest_V7155"));
    EXPECT_NE(obj, nullptr);
}

TEST(CreatObjectTemplateTest, UnregisteredName_ReturnsNull)
{
    // B2: creatObject<T> with unregistered name → returns nullptr
    QObject *obj = ClassObjectFactory::creatObject<QObject>(
        QStringLiteral("NonExistentCreatObj_V7155"));
    EXPECT_EQ(obj, nullptr);
}

// =========================================================================
// ClassRegisterHelper
// =========================================================================

TEST(ClassRegisterHelperTest, Constructor_RegistersClass)
{
    // B3: ClassRegisterHelper constructor registers class
    //     After construction, creatClassObject finds the registered name
    {
        ClassRegisterHelper helper(
            QStringLiteral("RegisterHelperTest_V7155"), createQObjectFactory);
    }
    void *obj = ClassObjectFactory::creatClassObject(
        QStringLiteral("RegisterHelperTest_V7155"));
    EXPECT_NE(obj, nullptr);
}

// =========================================================================
// RegistItemHelper
// =========================================================================

// Arbitrary item type used only for registration; any non-conflicting value
// works because the test only checks the constructor completes.
static constexpr int kTestItemType = 77777;

TEST(RegistItemHelperTest, Constructor_RegistersItem_NoCrash)
{
    // B4: RegistItemHelper constructor calls CGraphicsItem::registerItem
    //     CGraphicsItem_private is defined in the .cpp file and not accessible
    //     from the test, so we verify the constructor completes without crash.
    RegistItemHelper helper(
        QStringLiteral("RegistItemHelperTest_V7155"), kTestItemType);
    SUCCEED();
}

#endif // TEST_DYNREG_ITEM
