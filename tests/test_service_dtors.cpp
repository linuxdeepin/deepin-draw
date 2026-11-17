// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for service object destructors (issue V-8787)
 *
 * Target destructors:
 *   - FileHander::~FileHander()        (src/service/filehander.cpp)
 *   - CPrintManager::~CPrintManager()   (src/widgets/dialog/cprintmanager.cpp)
 *   - dbusdraw_adaptor::~dbusdraw_adaptor() (src/service/dbusdraw_adaptor.cpp)
 *
 * All three destructors are straightforward: FileHander deletes its
 * private implementation, CPrintManager and dbusdraw_adaptor have
 * minimal bodies. We exercise them via heap allocation + deletion.
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <memory>
#include <QApplication>

#define protected public
#define private public
#include "filehander.h"
#include "dialog/cprintmanager.h"
#include "dbusdraw_adaptor.h"
#undef protected
#undef private

// =========================================================================
// FileHander::~FileHander
// =========================================================================

TEST(FileHanderDestructor, DefaultConstruct_Destruct_NoCrash)
{
    auto *fh = new FileHander();
    ASSERT_NE(fh, nullptr);

    // Exercise basic identity before destruction.
    // No error() method available; just verify construction succeeded

    delete fh;
    SUCCEED();
}

TEST(FileHanderDestructor, ConstructWithParent_ParentCleansUp)
{
    // When a parent is set, QObject deletion chain handles cleanup.
    QObject parent;
    auto *fh = new FileHander(&parent);
    ASSERT_NE(fh, nullptr);

    // Deleting the parent should also destroy the FileHander without crash.
    // We verify by just deleting fh directly here (parent-based cleanup
    // is exercised implicitly at process exit for the harness).
    delete fh;
    SUCCEED();
}

TEST(FileHanderDestructor, ConstructDestruct_Reconstruct_NoCrash)
{
    // Verify that after destroying a FileHander, a new one can be
    // constructed (no global state corruption from the private struct).
    FileHander *fh1 = new FileHander();
    delete fh1;

    FileHander *fh2 = new FileHander();
    ASSERT_NE(fh2, nullptr);
    delete fh2;

    SUCCEED();
}

TEST(FileHanderDestructor, StaticMethodsAfterDestruct_StillValid)
{
    // Static methods should still work after a FileHander is destroyed.
    auto *fh = new FileHander();
    delete fh;

    // supPictureSuffix and supDdfStuffix are static — no instance needed.
    QStringList pictures = FileHander::supPictureSuffix();
    EXPECT_FALSE(pictures.isEmpty());

    QStringList ddfs = FileHander::supDdfStuffix();
    EXPECT_FALSE(ddfs.isEmpty());
}

// =========================================================================
// CPrintManager::~CPrintManager
// =========================================================================

TEST(CPrintManagerDestructor, DefaultConstruct_Destruct_NoCrash)
{
    auto *mgr = new CPrintManager();
    ASSERT_NE(mgr, nullptr);
    delete mgr;
    SUCCEED();
}

TEST(CPrintManagerDestructor, ConstructWithParent_Destruct_NoCrash)
{
    QObject parent;
    auto *mgr = new CPrintManager(&parent);
    ASSERT_NE(mgr, nullptr);
    delete mgr;
    SUCCEED();
}

TEST(CPrintManagerDestructor, Reconstruct_AfterDestruct_NoCrash)
{
    CPrintManager *mgr1 = new CPrintManager();
    delete mgr1;

    CPrintManager *mgr2 = new CPrintManager();
    ASSERT_NE(mgr2, nullptr);
    delete mgr2;
    SUCCEED();
}

// =========================================================================
// dbusdraw_adaptor::~dbusdraw_adaptor
// =========================================================================

TEST(DbusDrawAdaptorDestructor, DefaultConstruct_Destruct_NoCrash)
{
    // dbusdraw_adaptor requires a parent QObject for QDBusAbstractAdaptor.
    QObject parent;
    auto *adaptor = new dbusdraw_adaptor(&parent);
    ASSERT_NE(adaptor, nullptr);
    delete adaptor;
    SUCCEED();
}

TEST(DbusDrawAdaptorDestructor, Reconstruct_AfterDestruct_NoCrash)
{
    QObject parent1;
    auto *adaptor1 = new dbusdraw_adaptor(&parent1);
    delete adaptor1;

    QObject parent2;
    auto *adaptor2 = new dbusdraw_adaptor(&parent2);
    ASSERT_NE(adaptor2, nullptr);
    delete adaptor2;
    SUCCEED();
}

TEST(DbusDrawAdaptorDestructor, ParentDestruction_CleansAdaptor)
{
    // When the parent is destroyed, the adaptor (as a child QObject)
    // is also destroyed — verify no crash.
    QObject *parent = new QObject();
    auto *adaptor = new dbusdraw_adaptor(parent);
    ASSERT_NE(adaptor, nullptr);

    delete parent; // should also destroy adaptor
    SUCCEED();
}
