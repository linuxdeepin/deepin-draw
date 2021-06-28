/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
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
#include "clefttoolbar.h"
#include "utils/baseutils.h"
#include "drawTools/cdrawtoolfactory.h"
#include "drawTools/cdrawtoolmanagersigleton.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "application.h"
#include "toptoolbar.h"
#include "ccutwidget.h"
#include "blurwidget.h"

#include <DFileDialog>
#include <DWidget>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QVBoxLayout>
#include <QDebug>
#include <QAction>
#include <DApplicationHelper>
#include <QButtonGroup>

DGUI_USE_NAMESPACE

const int BTN_SPACING = 12;

CLeftToolBar::CLeftToolBar(QWidget *parent)
    : DFrame(parent)
{
    this->setFrameRounded(false);
    this->setFrameShape(QFrame::NoFrame);

    initUI();
    initDrawTools();
}

CLeftToolBar::~CLeftToolBar()
{

}

void CLeftToolBar::mouseMoveEvent(QMouseEvent *event)
{
    //禁止拖动
    Q_UNUSED(event)
}

void CLeftToolBar::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    drawApp->setApplicationCursor(Qt::ArrowCursor);
    DWidget::enterEvent(event);
}

void CLeftToolBar::paintEvent(QPaintEvent *event)
{
    DFrame::paintEvent(event);
}

void CLeftToolBar::initUI()
{
    drawApp->setWidgetAccesibleName(this, "LeftTool bar");
    //设置颜色
    DPalette pa = this->palette();
    pa.setColor(DPalette::Background, DPalette::Base);
    this->setPalette(pa);
    this->setAutoFillBackground(true);

    setFixedWidth(58);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addStretch();

    m_layout = new QVBoxLayout;
    m_layout->setSpacing(BTN_SPACING);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(10, 24, 0, 0);
    mainLayout->addLayout(m_layout);
    mainLayout->addStretch();
}

void CLeftToolBar::initDrawTools()
{
    auto tools = CDrawToolFactory::allTools();
    toolButtonGroup = new QButtonGroup(this);
    for (auto it = tools.begin(); it != tools.end(); ++it) {
        auto button = it.value()->toolButton();
        button->setCheckable(true);
        toolButtonGroup->addButton(button, it.value()->getDrawToolMode());
        m_layout->addWidget(button);
    }
    toolButtonGroup->setExclusive(true);
}

