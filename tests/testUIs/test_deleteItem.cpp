/*
 *  Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao <zhanghao@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#define protected public
#define private public
#include "qfiledialog.h"
#undef protected
#undef private

#include "application.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>
#include "cspinbox.h"
#include "pageview.h"
#include "pagescene.h"

#include <QDebug>
#include <QTimer>
#include <DFileDialog>
#include <QDialogButtonBox>

#include "../testItems/publicApi.h"

#if TEST_DELETE_ITEM

TEST(DeleteItem, TestDeleteItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(DeleteItem, TestDeleteItem)
{
    // Draw Triangle
    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    drawApp->setCurrentTool(triangle);

    int addedCount = view->pageScene()->allPageItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->pageScene()->allPageItems().count(), addedCount + 1);
    ASSERT_EQ(view->pageScene()->allPageItems().first()->type(), TriangleType);


    DTestEventList e;
    e.addKeyClick(Qt::Key_A, Qt::ControlModifier, 100);
    e.addKeyClick(Qt::Key_Delete, Qt::NoModifier, 100);
    e.simulate(getCurView());

    addedCount = view->pageScene()->allPageItems().count();
    ASSERT_EQ(addedCount, 0);

    view->page()->close(true);
}

#endif
