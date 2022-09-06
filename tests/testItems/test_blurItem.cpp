// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#define protected public
#define private public
#include "cgraphicsview.h"
#include <qaction.h>
#undef protected
#undef private
#include "ccentralwidget.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"

#include "crecttool.h"
#include "ccuttool.h"
#include "cellipsetool.h"
#define protected public
#define private public
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

#include <QDebug>
#include <DLineEdit>

#include "publicApi.h"

#if TEST_BLUR_ITEM

TEST(BlurItem, TestBlurItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(BlurItem, TestBlurItemOtherFunction)
{
    //模糊工具的标准使用步骤已经在pictureitem的测试中存在，这里仅测试未覆盖到的函数
    PageView *view = getCurView();
    drawApp->setCurrentTool(blur);
    QTest::qWait(200);

    auto tool = dynamic_cast<IBlurTool *>(drawApp->drawBoard()->tool(blur));

    //event filter
    QObject *o = view;
    QKeyEvent e1(QEvent::Type::ShortcutOverride, Qt::Key::Key_0, Qt::KeyboardModifier::ControlModifier);
    tool->eventFilter(o, &e1);
    QTest::qWait(2000);

    QKeyEvent e2(QEvent::Type::ShortcutOverride, Qt::Key::Key_Z, Qt::KeyboardModifier::ControlModifier);
    tool->eventFilter(o, &e2);
    QTest::qWait(2000);
}

#endif
