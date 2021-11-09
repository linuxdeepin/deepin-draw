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
#include "idrawtool.h"

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
bool blocked = false;

DrawToolManager::DrawToolManager(DrawBoard *parent)
    : DFrame(parent)
{
    this->setFrameRounded(false);
    this->setFrameShape(QFrame::NoFrame);

    initUI();

    //must init tools later,because some ui may not be created.
    QMetaObject::invokeMethod(this, [ = ]() {
        initDrawTools();
    }, Qt::DirectConnection);

}

DrawToolManager::~DrawToolManager()
{

}

bool DrawToolManager::setCurrentTool(int tool, bool force)
{
    return setCurrentTool(this->tool(tool), force);
}

bool DrawToolManager::setCurrentTool(IDrawTool *tool, bool force)
{
    if (blocked)
        return false;

    if (tool == nullptr)
        return false;

    bool ret = true;

    bool currentEnable = (tool->currentPage() != nullptr ?
                          tool->isEnable(tool->currentPage()->view())
                          : false);
    if (/*tool->status() == IDrawTool::EDisAbled*/!currentEnable) {
        return false;
    }
    //qWarning() << "_currentTool == " << (_currentTool == nullptr ? 0 : _currentTool->getDrawToolMode()) << "want to tool == " << tool->getDrawToolMode();
    auto current = _currentTool;
    if (tool != current) {
        bool doChange = true;
        if (current != nullptr && current->status() == IDrawTool::EWorking) {
            if (force) {
                current->interrupt();
            } else {
                qWarning() << "can not active another tool when one tool is working!";
                doChange = false;
            }
        }
        if (doChange) {
            _currentTool = tool;
            emit currentToolChanged(current != nullptr ? current->getDrawToolMode() : 0, tool->getDrawToolMode());
            if (current != nullptr && current->toolButton() != nullptr) {
                blocked = true;
                current->toolButton()->setChecked(false);
                blocked = false;
                current->changeStatusFlagTo(IDrawTool::EIdle);
            }

            if (tool->toolButton() != nullptr) {
                blocked = true;
                tool->toolButton()->setChecked(true);
                blocked = false;
                tool->changeStatusFlagTo(IDrawTool::EReady);
            }

        }
        ret = doChange;
    }
    return ret;
}

int DrawToolManager::currentTool() const
{
    //return toolButtonGroup->checkedId();
    if (_currentTool != nullptr) {
        return _currentTool->getDrawToolMode();
    }
    return 0;
}

IDrawTool *DrawToolManager::tool(int tool) const
{
    auto itf = _tools.find(tool);
    if (itf != _tools.end()) {
        return itf.value();
    }
    return nullptr;
}

DrawBoard *DrawToolManager::drawBoard() const
{
    if (parent() == nullptr)
        return nullptr;
    return qobject_cast<DrawBoard *>(parent());
}

void DrawToolManager::installTool(IDrawTool *pTool)
{
    auto itf = _tools.find(pTool->getDrawToolMode());
    if (itf == _tools.end()) {
        auto button = pTool->toolButton();
        button->setCheckable(true);
        toolButtonGroup->addButton(button, pTool->getDrawToolMode());
        m_layout->addWidget(button);
        pTool->setParent(this);
        pTool->setDrawBoard(drawBoard());
        _tools.insert(pTool->getDrawToolMode(), pTool);
    }
}

//void DrawToolManager::removeTool(IDrawTool *tool)
//{
//    auto button = tool->toolButton();
//    toolButtonGroup->removeButton(button);
//    m_layout->removeWidget(button);
//    _tools.remove(tool->getDrawToolMode());
//}

void DrawToolManager::registerAllTools()
{
    foreach (auto p, _tools) {
        p->registerAttributionWidgets();
    }
}

void DrawToolManager::mouseMoveEvent(QMouseEvent *event)
{
    //禁止拖动
    Q_UNUSED(event)
}

void DrawToolManager::enterEvent(QEvent *event)
{
    DWidget::enterEvent(event);
}

void DrawToolManager::paintEvent(QPaintEvent *event)
{
    DFrame::paintEvent(event);
}

void DrawToolManager::initUI()
{
    setWgtAccesibleName(this, "LeftTool bar");
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

void DrawToolManager::initDrawTools()
{
    //_tools = CDrawToolFactory::allTools();
    toolButtonGroup = new QButtonGroup(this);
//    for (auto it = _tools.begin(); it != _tools.end(); ++it) {

//        auto pTool = it.value();
//        auto button = pTool->toolButton();
//        button->setCheckable(true);
//        toolButtonGroup->addButton(button, it.value()->getDrawToolMode());
//        m_layout->addWidget(button);

//        pTool->setParent(this);
//        pTool->setDrawBoard(drawBoard());
//        connect(button, &QAbstractButton::toggled, button, [ = ](bool checked) {
//            if (checked) {
//                setCurrentTool(pTool->getDrawToolMode());
//            }
//        });
//    }
    toolButtonGroup->setExclusive(true);

    //setCurrentTool(selection);

    connect(toolButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [ = ](int id) {
        setCurrentTool(id);
    });
}

