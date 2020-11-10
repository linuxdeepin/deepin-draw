/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zhanghao<zhanghao@uniontech.com>
* Maintainer: zhanghao<zhanghao@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

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
#include "textcolorbutton.h"

#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicscutitem.h"

#include <QDebug>
#include <QtTest>
#include <QTestEventList>
#include <DLineEdit>

#include "publicApi.h"

#if TEST_TEXT_ITEM

TEST(TextItem, TestTextItemCreateView)
{
    createNewViewByShortcutKey();
}

TEST(TextItem, TestDrawTextItem)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("TextTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();

    int addedCount = view->drawScene()->getBzItems().count();
    createItemByMouse(view);
    ASSERT_EQ(view->drawScene()->getBzItems().count(), addedCount + 1);
    ASSERT_EQ(view->drawScene()->getBzItems().first()->type(), TextType);
}

TEST(TextItem, TestCopyTextItem)
{
    drawApp->topMainWindow()->setFocus();
    keyShortCutCopyItem();
}

TEST(TextItem, TestTextItemProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CGraphicsTextItem *text = dynamic_cast<CGraphicsTextItem *>(view->drawScene()->getBzItems().first());
    ASSERT_NE(text, nullptr);

    // Font color
    QColor color(Qt::red);
    TextColorButton *stroke = drawApp->topToolbar()->findChild<TextColorButton *>("TextColorButton");
    stroke->setColor(color);
    QTest::qWait(100);
    ASSERT_EQ(text->getTextColor(), color);

    // Font Family Type
    DComboBox *typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("TextFontFamily");
    ASSERT_NE(typeCombox, nullptr);
    QString family = "Bitstream Charter";//Andale Mono
    typeCombox->activated(family);
    QTest::qWait(100);
    ASSERT_EQ(text->getFontFamily(), family);

    // Font Style Type
    typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("TextFontStyle");
    ASSERT_NE(typeCombox, nullptr);
    QString style = "Bold";
    typeCombox->setCurrentText(style);
    QTest::qWait(100);
    ASSERT_EQ(text->getTextFontStyle(), style);

    // Font Size Type
    typeCombox = drawApp->topToolbar()->findChild<DComboBox *>("TextFontSize");
    ASSERT_NE(typeCombox, nullptr);
    QString size = "61p";
    typeCombox->lineEdit()->setText(size);
    typeCombox->lineEdit()->returnPressed();
    QTest::qWait(100);
    ASSERT_EQ(text->getFontSize(), 61);
}

TEST(TextItem, TestResizeTextItem)
{
    resizeItem();
}

TEST(TextItem, TestSelectAllTextItem)
{
    selectAllItem();
}

TEST(TextItem, TestSaveTextItemToFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);

    // save ddf file
    QString TextItemPath = QApplication::applicationDirPath() + "/test_text.ddf";
    QFile file(TextItemPath);
    file.open(QIODevice::ReadWrite);
    file.close();
    view->getDrawParam()->setDdfSavePath(TextItemPath);
    c->slotSaveToDDF(true);
    QTest::qWait(100);

    QFileInfo info(TextItemPath);
    ASSERT_TRUE(info.exists());
}

TEST(TextItem, TestOpenTextItemFromFile)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString TextItemPath = QApplication::applicationDirPath() + "/test_text.ddf";

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(TextItemPath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(100);

    view = getCurView();
    ASSERT_NE(view, nullptr);
    int addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    ASSERT_EQ(true, addedCount == 2 ? true : false);
}

#endif
