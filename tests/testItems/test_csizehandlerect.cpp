// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "cgraphicsview.h"
#include "cgraphicsrectitem.h"
#include "csizehandlerect.h"
#include "globaldefine.h"
#include "application.h"
#include "mainwindow.h"
#include "cdrawscene.h"
#undef protected
#undef private

#include "publicApi.h"

// =========================================================================
// Test fixture for getCursor tests — provides a fresh view per test
// =========================================================================
class CSizeHandleRectGetCursorTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        createNewViewByShortcutKey();
        m_view = getCurView();
        ASSERT_NE(m_view, nullptr);
    }

    PageView *m_view = nullptr;
};

// =========================================================================
// CSizeHandleRect::getCursor tests
// =========================================================================

TEST_F(CSizeHandleRectGetCursorTest, LeftTopDirection)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::LeftTop, QString());
    QCursor cursor = handle->getCursor();
    EXPECT_NE(cursor.shape(), Qt::BlankCursor);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectGetCursorTest, RightDirection)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::Right, QString());
    QCursor cursor = handle->getCursor();
    EXPECT_NE(cursor.shape(), Qt::BlankCursor);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectGetCursorTest, TopDirection)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::Top, QString());
    QCursor cursor = handle->getCursor();
    EXPECT_NE(cursor.shape(), Qt::BlankCursor);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectGetCursorTest, RotationDirection)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::Rotation, QString());
    QCursor cursor = handle->getCursor();
    EXPECT_NE(cursor.shape(), Qt::BlankCursor);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectGetCursorTest, InRectDirectionReturnsArrowCursor)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::InRect, QString());
    QCursor cursor = handle->getCursor();
    // InRect falls to default case, cursorResult stays as ArrowCursor
    EXPECT_EQ(cursor.shape(), Qt::ArrowCursor);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectGetCursorTest, NullParentNoCrash)
{
    // Create handle with null parent - should not crash
    CSizeHandleRect *handle = new CSizeHandleRect(nullptr, CSizeHandleRect::Left, QString());
    QCursor cursor = handle->getCursor();
    // Should not crash, returns some cursor
    EXPECT_NE(cursor.shape(), Qt::BlankCursor);

    delete handle;
}

// =========================================================================
// CSizeHandleRect::getTransBlockFlag tests
// =========================================================================

TEST(CSizeHandleRectGetTransBlockFlag, LeftDirection)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::Left, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_TRUE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlag, RightDirection)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::Right, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_TRUE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlag, TopDirection)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::Top, blockX, blockY);
    EXPECT_TRUE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlag, BottomDirection)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::Bottom, blockX, blockY);
    EXPECT_TRUE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlag, CornerDirectionsBothFalse)
{
    CSizeHandleRect::EDirection corners[] = {
        CSizeHandleRect::LeftTop,
        CSizeHandleRect::RightTop,
        CSizeHandleRect::RightBottom,
        CSizeHandleRect::LeftBottom
    };

    for (auto dir : corners) {
        bool blockX = false, blockY = false;
        CSizeHandleRect::getTransBlockFlag(dir, blockX, blockY);
        EXPECT_FALSE(blockX) << "blockX should be false for corner direction " << dir;
        EXPECT_FALSE(blockY) << "blockY should be false for corner direction " << dir;
    }
}

TEST(CSizeHandleRectGetTransBlockFlag, RotationDirectionBothFalse)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::Rotation, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlag, InRectDirectionBothFalse)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::InRect, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_FALSE(blockY);
}

TEST(CSizeHandleRectGetTransBlockFlag, NoneDirectionBothFalse)
{
    bool blockX = false, blockY = false;
    CSizeHandleRect::getTransBlockFlag(CSizeHandleRect::None, blockX, blockY);
    EXPECT_FALSE(blockX);
    EXPECT_FALSE(blockY);
}
