// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CGraphicsItemGroup childrenChanged signal (issue V-7618 group 3)
 *
 * Source: src/drawshape/drawItems/cgraphicsitemselectedmgr.h/.cpp
 *
 * Target signal:
 *   void childrenChanged(const QList<CGraphicsItem *> &children)
 */

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QSignalSpy>

#define protected public
#define private public
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "drawshape/drawItems/bzItems/cgraphicsrectitem.h"
#include "drawshape/cdrawscene.h"
#include "publicApi.h"
#undef protected
#undef private

// =========================================================================
// CGraphicsItemGroup childrenChanged signal
// =========================================================================

TEST(CGraphicsItemGroupChildrenChanged, AddingChild_EmitsSignal)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_TRUE(scene != nullptr);

    CGraphicsItemGroup group;

    QSignalSpy spy(&group, &CGraphicsItemGroup::childrenChanged);
    ASSERT_TRUE(spy.isValid());

    CGraphicsRectItem *rect = new CGraphicsRectItem(QRectF(0, 0, 100, 50));
    scene->addItem(rect);

    group.add(rect);

    // add() emits childrenChanged(m_listItems) exactly once
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.size(), 1);
}

TEST(CGraphicsItemGroupChildrenChanged, RemovingChild_EmitsSignal)
{
    createNewViewByShortcutKey();
    PageScene *scene = getCurView()->drawScene();
    ASSERT_TRUE(scene != nullptr);

    CGraphicsItemGroup group;

    CGraphicsRectItem *rect1 = new CGraphicsRectItem(QRectF(0, 0, 100, 50));
    CGraphicsRectItem *rect2 = new CGraphicsRectItem(QRectF(10, 10, 80, 40));
    scene->addItem(rect1);
    scene->addItem(rect2);

    group.add(rect1);
    group.add(rect2);

    QSignalSpy spy(&group, &CGraphicsItemGroup::childrenChanged);
    ASSERT_TRUE(spy.isValid());

    group.remove(rect1);

    // remove() emits childrenChanged(m_listItems) exactly once
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.size(), 1);
}

TEST(CGraphicsItemGroupChildrenChanged, EmptyList_NoCrash)
{
    CGraphicsItemGroup group;

    QSignalSpy spy(&group, &CGraphicsItemGroup::childrenChanged);
    ASSERT_TRUE(spy.isValid());

    // clear on empty group should not crash
    group.clear();

    SUCCEED();
}
