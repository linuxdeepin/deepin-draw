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
#include "ccuttool.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "qgraphicsview.h"
#include "globaldefine.h"

#include <DApplication>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <DToolButton>

#include "application.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "drawshape/globaldefine.h"
#include "drawshape/cdrawparamsigleton.h"

#include "application.h"
#include "toptoolbar.h"
#include "ccutwidget.h"
#include "cattributeitemwidget.h"
#include "ccutwidget.h"
#include "cattributemanagerwgt.h"
#include "cundoredocommand.h"

DWIDGET_USE_NAMESPACE

CCutTool::CCutTool()
    : IDrawTool(cut)
    , m_pCutItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bModify(false)
{
    //connect(this, &CCutTool::statusChanged, this, &CCutTool::onStatusChanged);
}

CCutTool::~CCutTool()
{
    m_cutItems.clear();
    qWarning() << "CCutTool deleted !!!!!!!!!!!!!";
}

DrawAttribution::SAttrisList CCutTool::attributions()
{
    CGraphicsCutItem *pItem = getCutItem(CManageViewSigleton::GetInstance()->getCurScene());
    if (pItem != nullptr)
        return pItem->attributions();

    return QList<int>() << DrawAttribution::ECutToolAttri;
}

QAbstractButton *CCutTool::initToolButton()
{
    DToolButton *m_cutBtn = new DToolButton;
    m_cutBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_C)));
    drawApp->setWidgetAccesibleName(m_cutBtn, "Crop tool button");
    m_cutBtn->setToolTip(tr("Crop(C)"));
    m_cutBtn->setIconSize(QSize(48, 48));
    m_cutBtn->setFixedSize(QSize(37, 37));
    m_cutBtn->setCheckable(true);
    connect(m_cutBtn, &DToolButton::toggled, m_cutBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_screenshot tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_screenshot tool_active");
        m_cutBtn->setIcon(b ? activeIcon : icon);
    });
    m_cutBtn->setIcon(QIcon::fromTheme("ddc_screenshot tool_normal"));
    return m_cutBtn;
}

void CCutTool::setAttributionVar(int attri, const QVariant &var, int phase, bool autoCmdStack)
{
    if (attri == DrawAttribution::ECutToolAttri) {
        QList<QVariant> vars = var.toList();
        if (vars.count() != 2) {
            return;
        }
        ECutType cuttp = ECutType(vars.first().toInt());
        QSize sz = vars.at(1).toSize();

        auto scene = CManageViewSigleton::GetInstance()->getCurView()->drawScene();

        this->changeCutType(cuttp, scene);
        this->changeCutSize(scene, sz);

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutType(cuttp);
    }
}

void CCutTool::registerAttributionWidgets()
{
    CCutWidget *pCutWidget = new CCutWidget;
    drawApp->setWidgetAccesibleName(pCutWidget, "scene cut attribution widget");
    _pCutWidget = pCutWidget;
    pCutWidget->setAutoCalSizeIfRadioChanged(false);
    pCutWidget->setAttribute(Qt::WA_NoMousePropagation, true);
    connect(pCutWidget, &CCutWidget::cutSizeChanged, this, [ = ](const QSize & sz) {

        QList<QVariant> vars;
        vars << pCutWidget->cutType() << sz;
        emit drawApp->attributionsWgt()->attributionChanged(DrawAttribution::ECutToolAttri, vars);
    });
    connect(pCutWidget, &CCutWidget::cutTypeChanged, this, [ = ](ECutType tp) {
        EDrawToolMode model = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
        CCutTool *pTool = dynamic_cast<CCutTool *>(CDrawToolManagerSigleton::GetInstance()->getDrawTool(model));
        if (pTool != nullptr) {

            QSizeF resultSz = this->changeCutType(tp, CManageViewSigleton::GetInstance()->getCurView()->drawScene());
            pCutWidget->setCutSize(resultSz.toSize(), false);
            QList<QVariant> vars;
            vars << tp << pCutWidget->cutSize();
            emit drawApp->attributionsWgt()->attributionChanged(DrawAttribution::ECutToolAttri, vars);
        }
    });
    connect(pCutWidget, &CCutWidget::finshed, this, [ = ](bool accept) {
        EDrawToolMode model = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
        CCutTool *pTool = dynamic_cast<CCutTool *>(CDrawToolManagerSigleton::GetInstance()->getDrawTool(model));
        if (pTool != nullptr) {
            CCmdBlock block(accept ? CManageViewSigleton::GetInstance()->getCurView()->drawScene() : nullptr);
            pTool->doFinished(accept);
        }
        pCutWidget->hideExpWindow();
    });
    connect(drawApp->attributionsWgt(), &CAttributeManagerWgt::updateWgt, this, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == pCutWidget) {
            QSignalBlocker bloker(pCutWidget);
            QList<QVariant> vars = var.toList();
            if (vars.count() == 2) {
                ECutType cuttp = ECutType(vars.first().toInt());
                pCutWidget->setCutType(cuttp, false, false);
                QSize sz = vars.at(1).toSize();
                pCutWidget->setCutSize(sz, false);
            }
        }
    });
    DrawAttribution::CAttributeManagerWgt::installComAttributeWgt(DrawAttribution::ECutToolAttri, pCutWidget);
}

void CCutTool::toolStart(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    m_pCutItem = getCurCutItem();

    QGraphicsItem *pFirstItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    if (pFirstItem != nullptr) {
        drawApp->setApplicationCursor(Qt::ClosedHandCursor);
    }
}

int CCutTool::decideUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    EOperateType ret = ENothing;

    QGraphicsItem *pStartPosTopQtItem = event->scene()->firstItem(pInfo->_startPos,
                                                                  pInfo->startPosItems, true, true);
    if (event->scene()->isBussizeHandleNodeItem(pStartPosTopQtItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pStartPosTopQtItem);
        pInfo->_etcopeTpUpdate = pHandle->dir();
        pInfo->etcItems.clear();

        pInfo->etcItems.append(m_pCutItem);

        ret = EResizeMove;
    } else if (pStartPosTopQtItem == m_pCutItem) {
        ret = EDragMove;
    }

    pInfo->_opeTpUpdate = ret;

    return pInfo->_opeTpUpdate;
}

void CCutTool::toolUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo->_opeTpUpdate == EDragMove) {
        m_pCutItem->move(pInfo->_prePos, event->pos());
    } else if (pInfo->_opeTpUpdate == EResizeMove) {
        CSizeHandleRect::EDirection direction = CSizeHandleRect::EDirection(pInfo->_etcopeTpUpdate);
        m_pCutItem->resizeCutSize(direction, pInfo->_prePos, event->pos(), &pInfo->_prePos);
        _pCutWidget->setCutSize(m_pCutItem->rect().size().toSize(), false);
    }
    event->view()->viewport()->update();
}

void CCutTool::toolFinish(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    mouseHoverEvent(event);
}

void CCutTool::mouseHoverEvent(CDrawToolEvent *event)
{
    QPointF scenePos = event->pos();

    QList<QGraphicsItem *> items = event->scene()->items(scenePos);

    QGraphicsItem *pItem = event->scene()->firstItem(scenePos, items, true, true, false, false);

    if (event->scene()->isBussizeHandleNodeItem(pItem)) {
        CSizeHandleRect *pHandle = dynamic_cast<CSizeHandleRect *>(pItem);
        drawApp->setApplicationCursor(pHandle->getCursor());
    } else if (pItem != nullptr && pItem->type() == CutType) {
        drawApp->setApplicationCursor(Qt::OpenHandCursor);
    } else {
        drawApp->setApplicationCursor(Qt::ArrowCursor);
    }
}

void CCutTool::createCutItem(CDrawScene *scene)
{
    if (!m_cutItems.contains(scene)) {
        deleteCutItem(scene);

        scene->clearSelection();

        m_pCutItem = new CGraphicsCutItem(scene->sceneRect());
        scene->addCItem(m_pCutItem);

        m_dragHandle = CSizeHandleRect::None;

        m_pCutItem->setIsFreeMode(true);
        m_pCutItem->setSelected(true);
        m_bModify = false;

        m_cutItems.insert(scene, m_pCutItem);
    }
}

void CCutTool::deleteCutItem(CDrawScene *scene)
{
    drawApp->setApplicationCursor(Qt::ArrowCursor);

//    auto itf = m_cutItems.find(scene);
    if (m_cutItems.contains(scene)) {
        auto itf = m_cutItems[scene];
        qDebug() << "deleteCutItem scene tag name = " << scene->getDrawParam()->viewName();
        CGraphicsCutItem *pCutItem = itf;
        scene->removeCItem(pCutItem);

        if (pCutItem == m_pCutItem) {
            m_pCutItem = nullptr;
        }

        delete pCutItem;
        m_cutItems.remove(scene);
//        m_cutItems.erase(itf);
    }
}

QSizeF CCutTool::changeCutType(int type, CDrawScene *scene)
{
    CGraphicsCutItem *pItem = getCutItem(scene);
    if (pItem != nullptr) {
        if (cut_free == type) {
            pItem->setIsFreeMode(true);
            pItem->setSelected(true);
        } else {
            pItem->setSelected(false);
            pItem->setRatioType(ECutType(type));
        }
        m_bModify = true;
        return pItem->rect().size();
    }
    return QSizeF(0, 0);
}

void CCutTool::changeCutSize(const CDrawScene *scene, const QSize &size)
{
    CGraphicsCutItem *pItem = getCutItem(const_cast<CDrawScene *>(scene));
    if (pItem != nullptr) {
        pItem->setSize(size.width(), size.height());
        m_bModify = true;
    }
}

void CCutTool::doFinished(bool accept)
{
    CGraphicsCutItem *pCutItem = getCurCutItem();

    if (accept) {
        CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
        pView->drawScene()->doAdjustmentScene(pCutItem->mapRectToScene(pCutItem->rect()));
    }

    drawApp->setCurrentTool(selection);
    //CManageViewSigleton::GetInstance()->getCurView()->slotQuitCutMode();
}

bool CCutTool::getCutStatus()
{
    CGraphicsCutItem *pCutItem = getCurCutItem();
    CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();

    if (pCutItem == nullptr || pView == nullptr) {
        qDebug() << "pCutItem or pView is nullptr";
        return false;
    }

    QRectF sceneItemBd = pCutItem->mapRectToScene(pCutItem->rect());

    if (sceneItemBd != pView->sceneRect()) {
        return true;
    } else {
        return false;
    }
}

QRectF CCutTool::getCutRect(CDrawScene *scene)
{
    QRectF rect;

    CGraphicsCutItem *pItem = getCutItem(const_cast<CDrawScene *>(scene));

    if (pItem != nullptr) {
        qDebug() << "pItem ====== " << reinterpret_cast<long long>(pItem) << "size = " << pItem->rect().size();
        rect = pItem->rect();
    }

    return rect;
}

int CCutTool::getCutType(CDrawScene *scene)
{
    CGraphicsCutItem *pItem = getCutItem(scene);
    if (pItem != nullptr) {
        return pItem->getRatioType();
    }
    return 0;
}

//bool CCutTool::getModifyFlag() const
//{
//    return m_bModify;
//}

//void CCutTool::setModifyFlag(bool flag)
//{
//    m_bModify = flag;
//}

CGraphicsCutItem *CCutTool::getCurCutItem()
{
    CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
    if (pView != nullptr) {
        return getCutItem(pView->drawScene());
    }
    return nullptr;
}

CGraphicsCutItem *CCutTool::getCutItem(CDrawScene *scene)
{
    if (scene != nullptr) {
        if (m_cutItems.contains(scene)) {
            return m_cutItems[scene];
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

void CCutTool::onStatusChanged(EStatus oldStatus, EStatus newStatus)
{
    if (oldStatus == EIdle && newStatus == EReady) {
        createCutItem(CManageViewSigleton::GetInstance()->getCurScene());
    } else if (newStatus == EIdle) {
        deleteCutItem(CManageViewSigleton::GetInstance()->getCurScene());
    }
}

bool CCutTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return false;
}
