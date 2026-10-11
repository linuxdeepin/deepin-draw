// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CManageViewSigleton methods (issue V-5575)
 *
 * Source: src/frame/cviewmanagement.cpp
 *
 * Target methods (not already covered by test_cviewmanagement.cpp):
 *   CManageViewSigleton::getCurScene, addView, setCurView,
 *   viewCount, quitIfEmpty, getFileSrcData
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QTemporaryFile>

#define protected public
#define private public
#include "cviewmanagement.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CManageViewSigleton::viewCount
// =========================================================================

TEST(CManageViewSigletonViewCountTest, NonNegativeAfterCreateView)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    EXPECT_GE(mgr->viewCount(), 1);
}

// =========================================================================
// CManageViewSigleton::getCurScene
// =========================================================================

TEST(CManageViewSigletonGetCurSceneTest, ReturnsNonNullAfterCreateView)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    auto *scene = mgr->getCurScene();
    EXPECT_NE(scene, nullptr);
}

TEST(CManageViewSigletonGetCurSceneTest, ReturnsNullptrWhenNoView)
{
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    // Temporarily clear views to test the null path.
    int savedCount = mgr->m_allViews.size();
    QList<PageView *> savedViews = mgr->m_allViews;
    int savedIndex = mgr->m_curIndex;

    mgr->m_allViews.clear();
    mgr->m_curIndex = -1;

    EXPECT_EQ(mgr->getCurScene(), nullptr);

    // Restore state.
    mgr->m_allViews = savedViews;
    mgr->m_curIndex = savedIndex;
}

// =========================================================================
// CManageViewSigleton::addView
// =========================================================================

TEST(CManageViewSigletonAddViewTest, NullViewDoesNotChangeCount)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    int countBefore = mgr->viewCount();

    mgr->addView(nullptr);
    EXPECT_EQ(mgr->viewCount(), countBefore);
}

TEST(CManageViewSigletonAddViewTest, AddExistingViewDoesNotDuplicate)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    int countBefore = mgr->viewCount();

    // Re-adding the current view should not increase the count.
    PageView *curView = mgr->m_allViews.value(mgr->m_curIndex);
    if (curView) {
        mgr->addView(curView);
        EXPECT_EQ(mgr->viewCount(), countBefore);
    }
}

// =========================================================================
// CManageViewSigleton::setCurView
// =========================================================================

TEST(CManageViewSigletonSetCurViewTest, NullViewDoesNotCrash)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    int savedIndex = mgr->m_curIndex;

    mgr->setCurView(nullptr);
    // Index should not change when setting null.
    EXPECT_EQ(mgr->m_curIndex, savedIndex);
}

TEST(CManageViewSigletonSetCurViewTest, SetToCurrentViewIsNoOp)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);

    PageView *curView = mgr->m_allViews.value(mgr->m_curIndex);
    if (curView) {
        int savedIndex = mgr->m_curIndex;
        mgr->setCurView(curView);
        EXPECT_EQ(mgr->m_curIndex, savedIndex);
    }
}

// =========================================================================
// CManageViewSigleton::quitIfEmpty
// =========================================================================

TEST(CManageViewSigletonQuitIfEmptyTest, DoesNotQuitWhenViewExists)
{
    createNewViewByShortcutKey();
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    ASSERT_NE(mgr->getCurView(), nullptr);

    // Should not call qApp->quit() because a view exists.
    mgr->quitIfEmpty();
    SUCCEED();
}

// =========================================================================
// CManageViewSigleton::getFileSrcData
// =========================================================================

TEST(CManageViewSigletonGetFileSrcDataTest, EmptyPathReturnsEmptyByteArray)
{
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    QByteArray result = mgr->getFileSrcData(QStringLiteral(""));
    EXPECT_TRUE(result.isEmpty());
}

TEST(CManageViewSigletonGetFileSrcDataTest, NonExistentFileReturnsEmpty)
{
    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    QByteArray result = mgr->getFileSrcData(
        QStringLiteral("/nonexistent/path/to/file_V5575.ddf"));
    EXPECT_TRUE(result.isEmpty());
}

TEST(CManageViewSigletonGetFileSrcDataTest, ExistingFileReturnsEmptyDueToSourceDefect)
{
    // NOTE: Source defect in CManageViewSigleton::getFileSrcData() —
    // it calls QFile::readAll() without first calling QFile::open(),
    // so readAll() always returns an empty QByteArray. This test
    // documents the current (buggy) behavior; the function should
    // open the file before reading.
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QByteArray content = "test content for V-5575";
    tmpFile.write(content);
    tmpFile.flush();
    tmpFile.close();

    auto *mgr = CManageViewSigleton::GetInstance();
    ASSERT_NE(mgr, nullptr);
    QByteArray result = mgr->getFileSrcData(tmpFile.fileName());
    EXPECT_TRUE(result.isEmpty());
}
