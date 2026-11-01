// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Unit tests for CSizeHandleRect::setVisible
 * (issue V-7262)
 *
 * Source: src/drawshape/drawItems/csizehandlerect.cpp
 *
 * Target method:
 *   void setVisible(bool flag) — sets m_bVisible and calls show()/hide()
 */

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

class CSizeHandleRectVisibleTest : public ::testing::Test
{
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
// setVisible with parent
// =========================================================================

TEST_F(CSizeHandleRectVisibleTest, SetVisibleTrueWithParent)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::LeftTop, QString());
    handle->setVisible(true);

    EXPECT_EQ(handle->isVisible(), true);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectVisibleTest, SetVisibleFalseWithParent)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::Right, QString());
    handle->setVisible(false);

    EXPECT_EQ(handle->isVisible(), false);

    delete handle;
    delete parent;
}

TEST_F(CSizeHandleRectVisibleTest, ToggleVisible)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::Top, QString());

    handle->setVisible(true);
    EXPECT_EQ(handle->isVisible(), true);

    handle->setVisible(false);
    EXPECT_EQ(handle->isVisible(), false);

    handle->setVisible(true);
    EXPECT_EQ(handle->isVisible(), true);

    delete handle;
    delete parent;
}

// =========================================================================
// setVisible without parent (no show/hide call, just sets m_bVisible)
// =========================================================================

TEST_F(CSizeHandleRectVisibleTest, SetVisibleTrueWithoutParent)
{
    CSizeHandleRect *handle = new CSizeHandleRect(nullptr, CSizeHandleRect::LeftTop, QString());
    handle->setVisible(true);

    // Without parent, show()/hide() is skipped but m_bVisible is set
    EXPECT_EQ(handle->m_bVisible, true);

    delete handle;
}

TEST_F(CSizeHandleRectVisibleTest, SetVisibleFalseWithoutParent)
{
    CSizeHandleRect *handle = new CSizeHandleRect(nullptr, CSizeHandleRect::Right, QString());
    handle->setVisible(false);

    EXPECT_EQ(handle->m_bVisible, false);

    delete handle;
}

TEST_F(CSizeHandleRectVisibleTest, ToggleVisibleWithoutParent)
{
    CSizeHandleRect *handle = new CSizeHandleRect(nullptr, CSizeHandleRect::Bottom, QString());

    handle->setVisible(true);
    EXPECT_EQ(handle->m_bVisible, true);

    handle->setVisible(false);
    EXPECT_EQ(handle->m_bVisible, false);

    handle->setVisible(true);
    EXPECT_EQ(handle->m_bVisible, true);

    delete handle;
}

// =========================================================================
// Default visibility
// =========================================================================

TEST_F(CSizeHandleRectVisibleTest, DefaultVisibilityIsTrue)
{
    CGraphicsRectItem *parent = new CGraphicsRectItem(0, 0, 100, 100, nullptr);
    m_view->drawScene()->addItem(parent);

    CSizeHandleRect *handle = new CSizeHandleRect(parent, CSizeHandleRect::Left, QString());

    // Default m_bVisible should be true
    EXPECT_EQ(handle->m_bVisible, true);

    delete handle;
    delete parent;
}
