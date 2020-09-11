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

#include "../getMainWindow.h"
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
#include <qobjectdefs.h>

TEST(ItemTool, TestOpenDDF)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // 打开保存绘制的 ddf
    QString ddfpath = QApplication::applicationDirPath() + "/test_save.ddf";
    QFileInfo info(ddfpath);
    ASSERT_TRUE(info.exists());

    QMimeData mimedata;
    QList<QUrl> li;
    li.append(QUrl(ddfpath));
    mimedata.setUrls(li);

    const QPoint pos = view->viewport()->rect().center();
    QDragEnterEvent eEnter(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &eEnter);

    QDropEvent e(pos, Qt::IgnoreAction, &mimedata, Qt::LeftButton, Qt::NoModifier);
    dApp->sendEvent(view->viewport(), &e);
    QTest::qWait(300);

    view = getCurView();
    ASSERT_NE(view, nullptr);
    int addedCount = view->drawScene()->getBzItems(view->drawScene()->items()).count();
    ASSERT_EQ(true, addedCount == 11 ? true : false);
}

TEST(ItemTool, TestItemsProperty)
{
    CGraphicsView *view = getCurView();
    ASSERT_NE(view, nullptr);

    // [0] 获取当前ddf中的所有业务图元
    QList<QGraphicsItem *> m_items = getCurView()->drawScene()->getBzItems();
    ASSERT_NE(m_items.count(), 0);

    // [1] 循环判断当前图元的类型并进行设置相应的属性
    DComboBox *sideComBox = dApp->topToolbar()->findChild<DComboBox *>("SideWidth");
    for (int i = 0; i < m_items.count(); i++) {
        CGraphicsItem *item = dynamic_cast<CGraphicsItem *>(m_items.at(i));
        view->drawScene()->clearMrSelection();
        view->drawScene()->selectItem(item);

        if (item->type() == PictureType) {
            // 左旋转
            QPushButton *btn = dApp->topToolbar()->findChild<QPushButton *>("PicLeftRotateBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);

            // 右旋转
            btn = dApp->topToolbar()->findChild<QPushButton *>("PicRightRotateBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);

            // 水平翻转
            btn = dApp->topToolbar()->findChild<QPushButton *>("PicFlipHBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);

            // 水平翻转
            btn = dApp->topToolbar()->findChild<QPushButton *>("PicFlipVBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);

            // 自适应scence
            btn = dApp->topToolbar()->findChild<QPushButton *>("PicFlipAdjustmentBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);

            view->drawScene()->setSceneRect(QRectF(0, 0, 1366, 768));
            QTest::qWait(100);
        } else if (item->type() == RectType) {
            // pen width
            sideComBox->setCurrentIndex(4); // 0 1 2 4 8 10 px
            QTest::qWait(100);
            ASSERT_EQ(item->pen().width(), 8);

            // Rect Radius
            QSpinBox *sp = dApp->topToolbar()->findChild<QSpinBox *>("RectRadius");
            ASSERT_NE(sp, nullptr);
            int value = sp->value();
            sp->setValue(value * 2);
            CGraphicsRectItem *rect = dynamic_cast<CGraphicsRectItem *>(item);
            ASSERT_NE(rect, nullptr);
            QTest::qWait(100);
            ASSERT_EQ(rect->getXRedius(), sp->value());
        } else if (item->type() == EllipseType || item->type() == TriangleType) {
            // pen width
            sideComBox->setCurrentIndex(5); // 0 1 2 4 8 10 px
            QTest::qWait(100);
            ASSERT_EQ(item->pen().width(), 10);
        } else if (item->type() == PolygonalStarType) {
            // pen width
            sideComBox->setCurrentIndex(1); // 0 1 2 4 8 10 px
            ASSERT_EQ(item->pen().width(), 1);

            // Start anchor
            CSpinBox *sp = dApp->topToolbar()->findChild<CSpinBox *>("StartAnchorNumber");
            ASSERT_NE(sp, nullptr);
            int value = sp->value();
            sp->setValue(value * 2);
            CGraphicsPolygonalStarItem *start = dynamic_cast<CGraphicsPolygonalStarItem *>(item);
            QTest::qWait(100);
            ASSERT_NE(start, nullptr);
            ASSERT_EQ(start->anchorNum(), sp->value());

            // Start Radius
            sp = dApp->topToolbar()->findChild<CSpinBox *>("StartRadiusNumber");
            ASSERT_NE(sp, nullptr);
            value = sp->value();
            sp->setValue(value + 10);
            QTest::qWait(100);
            ASSERT_EQ(start->innerRadius(), sp->value());
        } else if (item->type() == PolygonType) {
            // pen width
            sideComBox->setCurrentIndex(1); // 0 1 2 4 8 10 px
            ASSERT_EQ(item->pen().width(), 1);

            // Polygon Side
            CSpinBox *sp = dApp->topToolbar()->findChild<CSpinBox *>("PolygonSideNumber");
            ASSERT_NE(sp, nullptr);
            int value = sp->value();
            sp->setValue(value + 3);
            QTest::qWait(100);
            CGraphicsPolygonItem *polygon = dynamic_cast<CGraphicsPolygonItem *>(item);
            ASSERT_NE(polygon, nullptr);
            ASSERT_EQ(polygon->nPointsCount(), sp->value());
        } else if (item->type() == LineType) {
            // pen width
            sideComBox->setCurrentIndex(5); // 0 1 2 4 8 10 px
            ASSERT_EQ(item->pen().width(), 10);

            // Start Type
            DComboBox *typeCombox = dApp->topToolbar()->findChild<DComboBox *>("LineOrPenStartType");
            ASSERT_NE(typeCombox, nullptr);
            CGraphicsLineItem *line = dynamic_cast<CGraphicsLineItem *>(item);
            ASSERT_NE(line, nullptr);
            for (int i = 0; i < typeCombox->count(); i++) {
                typeCombox->setCurrentIndex(i);
                QTest::qWait(100);
                ASSERT_EQ(line->getLineStartType(), i);
            }

            // End Type
            typeCombox = dApp->topToolbar()->findChild<DComboBox *>("LineOrPenEndType");
            ASSERT_NE(typeCombox, nullptr);
            for (int i = 0; i < typeCombox->count(); i++) {
                typeCombox->setCurrentIndex(i);
                QTest::qWait(100);
                ASSERT_EQ(line->getLineEndType(), i);
            }
        } else if (item->type() == PenType) {
            // pen width
            sideComBox->setCurrentIndex(5); // 0 1 2 4 8 10 px
            ASSERT_EQ(item->pen().width(), 10);

            // Start Type
            DComboBox *typeCombox = dApp->topToolbar()->findChild<DComboBox *>("LineOrPenStartType");
            ASSERT_NE(typeCombox, nullptr);
            CGraphicsPenItem *pen = dynamic_cast<CGraphicsPenItem *>(item);
            ASSERT_NE(pen, nullptr);
            for (int i = 0; i < typeCombox->count(); i++) {
                typeCombox->setCurrentIndex(i);
                QTest::qWait(100);
                ASSERT_EQ(pen->getPenStartType(), i);
            }

            // End Type
            typeCombox = dApp->topToolbar()->findChild<DComboBox *>("LineOrPenEndType");
            ASSERT_NE(typeCombox, nullptr);
            for (int i = 0; i < typeCombox->count(); i++) {
                typeCombox->setCurrentIndex(i);
                QTest::qWait(100);
                ASSERT_EQ(pen->getPenEndType(), i);
            }
        } else if (item->type() == TextType) {
            // Font Family Type
            DComboBox *typeCombox = dApp->topToolbar()->findChild<DComboBox *>("TextFontFamily");
            ASSERT_NE(typeCombox, nullptr);
            QString family = "Bitstream Charter";//Andale Mono
            typeCombox->activated(family);
            CGraphicsTextItem *text = dynamic_cast<CGraphicsTextItem *>(item);
            ASSERT_NE(text, nullptr);
            QTest::qWait(100);
            ASSERT_EQ(text->getFontFamily(), family);

            // Font Style Type
            typeCombox = dApp->topToolbar()->findChild<DComboBox *>("TextFontStyle");
            ASSERT_NE(typeCombox, nullptr);
            QString style = "Bold";
            typeCombox->setCurrentText(style);
            QTest::qWait(100);
            ASSERT_EQ(text->getTextFontStyle(), style);

            // Font Size Type
            typeCombox = dApp->topToolbar()->findChild<DComboBox *>("TextFontSize");
            ASSERT_NE(typeCombox, nullptr);
            QString size = "61p";
            typeCombox->lineEdit()->setText(size);
            typeCombox->lineEdit()->returnPressed();
            QTest::qWait(100);
            ASSERT_EQ(text->getFontSize(), 61);
        } else if (item->type() == BlurType) {
            // Blur Blur Type
            DToolButton *btn = dApp->topToolbar()->findChild<DToolButton *>("BlurBlurBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);
            CGraphicsMasicoItem *blur = dynamic_cast<CGraphicsMasicoItem *>(item);
            ASSERT_NE(blur, nullptr);
            ASSERT_EQ(blur->getBlurEffect(), 0);

            // Blur Masic Type
            btn = dApp->topToolbar()->findChild<DToolButton *>("BlurMasicBtn");
            ASSERT_NE(btn, nullptr);
            btn->released();
            QTest::qWait(100);
            ASSERT_EQ(blur->getBlurEffect(), 1);

            // Blur width
            CSpinBox *sp = dApp->topToolbar()->findChild<CSpinBox *>("BlurPenWidth");
            ASSERT_NE(sp, nullptr);
            sp->setValue(100);
            QTest::qWait(100);
            ASSERT_EQ(blur->getBlurWidth(), sp->value());
            sp->lineEdit()->setText("300");
            sp->lineEdit()->editingFinished();
            QTest::qWait(100);
            ASSERT_EQ(blur->getBlurWidth(), sp->value());
        }
    }

    // [2] 裁剪图元需要单独进行处理才可以
    CCentralwidget *c = getMainWindow()->getCCentralwidget();
    ASSERT_NE(c, nullptr);
    QToolButton *tool = nullptr;
    tool = c->getLeftToolBar()->findChild<QToolButton *>("CropTool");
    ASSERT_NE(tool, nullptr);
    tool->clicked();
    QTest::qWait(100);

    // 获取确认裁剪按钮
    DPushButton  *cutDoneBtn = dApp->topToolbar()->findChild<DPushButton *>("CutDoneBtn");
    ASSERT_NE(cutDoneBtn, nullptr);

    // [2.1] 手动设置裁剪大小(自由模式)
    DLineEdit *widthLineEdit = dApp->topToolbar()->findChild<DLineEdit *>("CutWidthLineEdit");
    ASSERT_NE(widthLineEdit, nullptr);
    widthLineEdit->setText("1000");
    widthLineEdit->editingFinished();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());

    tool->clicked();
    QTest::qWait(100);

    DLineEdit *heightLineEdit = dApp->topToolbar()->findChild<DLineEdit *>("CutHeightLineEdit");
    ASSERT_NE(heightLineEdit, nullptr);
    heightLineEdit->setText("500");
    heightLineEdit->editingFinished();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.2] 1:1 模式
    tool->clicked();
    QTest::qWait(100);
    DPushButton *btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate1_1Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.3] 2:3 模式
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate2_3Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(int(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());

    // [2.4] 8:5 模式
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate8_5Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().height()), heightLineEdit->text().toInt());

    // [2.5] 16:9 模式
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRate16_9Btn");
    ASSERT_NE(btn, nullptr);
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().width()), widthLineEdit->text().toInt());
    ASSERT_EQ(qRound(view->drawScene()->sceneRect().height()), heightLineEdit->text().toInt());

    // [2.6] 原始恢复
    tool->clicked();
    QTest::qWait(100);
    btn = dApp->topToolbar()->findChild<DPushButton *>("CutRateOriginalBtn");
    ASSERT_NE(btn, nullptr);
    view->drawScene()->setSceneRect(QRectF(0, 0, 400, 400));
    btn->toggle();
    cutDoneBtn->clicked();
    QTest::qWait(100);
    ASSERT_EQ(view->drawScene()->sceneRect().width(), widthLineEdit->text().toInt());
    ASSERT_EQ(view->drawScene()->sceneRect().height(), heightLineEdit->text().toInt());
}

