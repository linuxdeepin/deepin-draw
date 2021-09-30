/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
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
#include "cgraphicsview.h"
#include <qaction.h>

#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#include "cmasicotool.h"
#include "cpentool.h"
#include "cpolygonalstartool.h"
#include "cpolygontool.h"
#include "ctexttool.h"
#include "ctriangletool.h"

#include <DFloatingButton>
#include <DComboBox>
#include <dzoommenucombobox.h>
#include "cspinbox.h"

#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicscutitem.h"
#include "blurwidget.h"

#include <QDebug>
#include <DLineEdit>

#include "publicApi.h"

#undef protected
#undef private

#if TEST_LAYER_ITEM


TEST(LayerItem, TestLayerItem)
{
    createNewViewByShortcutKey();

    PageView *view = getCurView();
    ASSERT_NE(view, nullptr);
    Page *c = getMainWindow()->drawBoard()->currentPage();
    ASSERT_NE(c, nullptr);

    {
        auto layer = c->scene()->m_currentLayer;
        layer->layerImage();
        layer->rect();
        layer->updateShape();
        QImage img(500, 500, QImage::Format_ARGB32);
        QPainter painter(&img);
        QStyleOptionGraphicsItem option;
        layer->paintSelf(&painter, &option);
        EXPECT_NE(layer, nullptr);
    }
    c->close(true);
}
TEST(LayerItem, TestJActivedPaintInfo)
{
    JActivedPaintInfo info;
    QPainterPath path;
    path.moveTo(QPointF(0, 0));
    path.lineTo(QPointF(20, 20));
    auto picture = JActivedPaintInfo::getPathPicture(path, QPen(Qt::red), QBrush(Qt::blue));
    EXPECT_EQ(picture.isNull(), false);
}
TEST(LayerItem, TestJDynamicLayer)
{
    auto item = new JDynamicLayer(QImage(":/test.png"));
    QPainterPath path;
    path.moveTo(QPointF(0, 0));
    path.lineTo(QPointF(20, 20));
    item->addPenPath(path, QPen(Qt::red), 1, false);

    auto picture = JActivedPaintInfo::getPathPicture(path, QPen(Qt::red), QBrush(Qt::blue));
    JPaintCommand *cmd1 = new JPaintCommand(picture, false, item);
    item->appendComand(cmd1, true, false);
    EXPECT_EQ(item->commands().size(), 1);

    JBlurCommand *cmd2 = new JBlurCommand(path, 0, item);
    item->appendComand(cmd2, true, false);
    EXPECT_EQ(item->commands().size(), 2);

    delete item;
}

#endif
