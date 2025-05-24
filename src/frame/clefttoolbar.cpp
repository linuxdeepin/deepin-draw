// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <DGuiApplicationHelper>
#include <QButtonGroup>

DGUI_USE_NAMESPACE

const int BTN_SPACING = 12;
bool blocked = false;
const int TOOL_MANAGER_WIDTH_NORMAL = 68;
const int TOOL_MANAGER_WIDTH_COMPACT = 48;
const int TOOL_BTN_SIZE_NORMAL = 37;
const int TOOL_BTN_SIZE_COMPACT = 28;

DrawToolManager::DrawToolManager(DrawBoard *parent)
    : DFrame(parent), m_drawBoard(parent)
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

void DrawToolManager::setScrollArea(DScrollArea *area)
{
    m_scrollArea = area;
}

bool DrawToolManager::setCurrentTool(int tool, bool force)
{
    return setCurrentTool(this->tool(tool), force);
}

bool DrawToolManager::setCurrentTool(IDrawTool *tool, bool force)
{
    if (blocked) {
        qDebug() << "Tool change blocked";
        return false;
    }

    if (tool == nullptr) {
        qWarning() << "Attempted to set null tool";
        return false;
    }

    bool ret = true;

    bool currentEnable = (tool->currentPage() != nullptr ?
                          tool->isEnable(tool->currentPage()->view())
                          : false);
    if (!currentEnable) {
        qWarning() << "Tool is not enabled for current page";
        return false;
    }

    auto current = _currentTool;
    if (tool != current) {
        bool doChange = true;
        if (current != nullptr && current->status() == IDrawTool::EWorking) {
            if (force) {
                qDebug() << "Forcing tool change while current tool is working";
                current->interrupt();
            } else {
                qWarning() << "Cannot activate another tool when one tool is working";
                doChange = false;
            }
        }
        if (doChange) {
            qInfo() << "Changing tool from" << (current != nullptr ? current->getDrawToolMode() : 0) 
                    << "to" << tool->getDrawToolMode();
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
    if (m_drawBoard == nullptr)
        return nullptr;
    return m_drawBoard;
}

void DrawToolManager::installTool(IDrawTool *pTool)
{
    qDebug() << "Installing tool - tool ID:" << pTool->getDrawToolMode();
    auto itf = _tools.find(pTool->getDrawToolMode());
    if (itf == _tools.end()) {
        auto button = pTool->toolButton();
        button->setCheckable(true);
        toolButtonGroup->addButton(button, pTool->getDrawToolMode());
        m_layout->addWidget(button);
        pTool->setParent(this);
        pTool->setDrawBoard(drawBoard());
        _tools.insert(pTool->getDrawToolMode(), pTool);

#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::CompactMode)
        button->setFixedSize(QSize(TOOL_BTN_SIZE_COMPACT, TOOL_BTN_SIZE_COMPACT));
    else
        button->setFixedSize(QSize(TOOL_BTN_SIZE_NORMAL, TOOL_BTN_SIZE_NORMAL));
#endif
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


#if (QT_VERSION_MAJOR == 5)
void DrawToolManager::enterEvent(QEvent *event)
{
    DWidget::enterEvent(event);
}
#elif (QT_VERSION_MAJOR == 6)
void DrawToolManager::enterEvent(QEnterEvent *event)
{
    DWidget::enterEvent(event);
}
#endif

void DrawToolManager::paintEvent(QPaintEvent *event)
{
    DFrame::paintEvent(event);
}

void DrawToolManager::initUI()
{
    qDebug() << "Initializing DrawToolManager UI";
    setWgtAccesibleName(this, "LeftTool bar");
    //设置颜色
    DPalette pa = this->palette();
    // TODO: dtk问题，在 Qt6 中，DPalette::Background已经被移除或重命名，用DPalette::Window做代替
#if (QT_VERSION_MAJOR == 5)
    pa.setColor(DPalette::Background, DPalette::Base);
#elif (QT_VERSION_MAJOR == 6)
    pa.setColor(DPalette::Window, DPalette::Base);
#endif
    this->setPalette(pa);
    this->setAutoFillBackground(true);

    setMinimumHeight(460);//设置最小高度保证最小化显示正常
    setFixedWidth(TOOL_MANAGER_WIDTH_NORMAL);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addStretch();

    m_layout = new QVBoxLayout;
    m_layout->setSpacing(BTN_SPACING);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setContentsMargins(14, 24, 0, 24);
    mainLayout->addLayout(m_layout);
    mainLayout->addStretch();

#ifdef DTKWIDGET_CLASS_DSizeMode
    if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::CompactMode) {
        qDebug() << "Setting compact mode UI";
        setFixedWidth(TOOL_MANAGER_WIDTH_COMPACT);
        m_layout->setContentsMargins(9, 24, 0, 24);
    }

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [=](DGuiApplicationHelper::SizeMode sizeMode) {
        qDebug() << "Size mode changed to:" << (sizeMode == DGuiApplicationHelper::CompactMode ? "Compact" : "Normal");
        if (DGuiApplicationHelper::instance()->sizeMode() == DGuiApplicationHelper::CompactMode) {
            this->setFixedWidth(TOOL_MANAGER_WIDTH_COMPACT);
            if (m_scrollArea)
                m_scrollArea->setFixedWidth(TOOL_MANAGER_WIDTH_COMPACT);
            m_layout->setContentsMargins(9, 24, 0, 24);

            if (toolButtonGroup) {
                for (int i = 0; i < toolButtonGroup->buttons().size(); i++)
                    toolButtonGroup->buttons().at(i)->setFixedSize(QSize(TOOL_BTN_SIZE_COMPACT, TOOL_BTN_SIZE_COMPACT));
            }
        } else {
            this->setFixedWidth(TOOL_MANAGER_WIDTH_NORMAL);
            if (m_scrollArea)
                m_scrollArea->setFixedWidth(TOOL_MANAGER_WIDTH_NORMAL);
            m_layout->setContentsMargins(14, 24, 0, 24);

            if (toolButtonGroup) {
                for (int i = 0; i < toolButtonGroup->buttons().size(); i++)
                    toolButtonGroup->buttons().at(i)->setFixedSize(QSize(TOOL_BTN_SIZE_NORMAL, TOOL_BTN_SIZE_NORMAL));
            }
        }
    });

#endif
}

void DrawToolManager::initDrawTools()
{
    toolButtonGroup = new QButtonGroup(this);
    toolButtonGroup->setExclusive(true);

#if (QT_VERSION_MAJOR == 5)
    connect(toolButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [ = ](int id) {
        setCurrentTool(id);
    });
#elif (QT_VERSION_MAJOR == 6)
    connect(toolButtonGroup, &QButtonGroup::idClicked, this, [ = ](int id) {
        setCurrentTool(id);
    });
#endif
}

