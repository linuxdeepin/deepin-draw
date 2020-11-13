/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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

#include "application.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "drawshape/globaldefine.h"
#include "drawshape/cdrawparamsigleton.h"

#include "application.h"
#include "toptoolbar.h"
#include "citemattriwidget.h"
#include "ccutwidget.h"

DWIDGET_USE_NAMESPACE

CCutTool::CCutTool()
    : IDrawTool(cut)
    , m_pCutItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bModify(false)
{

}

CCutTool::~CCutTool()
{

}

void CCutTool::toolStart(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    m_pCutItem = getCurCutItem();

    QGraphicsItem *pFirstItem = pInfo->startPosItems.isEmpty() ? nullptr : pInfo->startPosItems.first();
    if (pFirstItem != nullptr) {
        drawApp->setApplicationCursor(Qt::ClosedHandCursor);
    }
}

int CCutTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
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

void CCutTool::toolUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo->_opeTpUpdate == EDragMove) {
        m_pCutItem->move(pInfo->_prePos, event->pos());
    } else if (pInfo->_opeTpUpdate == EResizeMove) {
        CSizeHandleRect::EDirection direction = CSizeHandleRect::EDirection(pInfo->_etcopeTpUpdate);
        m_pCutItem->resizeCutSize(direction, pInfo->_prePos, event->pos(), &pInfo->_prePos);
        drawApp->topToolbar()->attributWidget()->getCutWidget()->setCutSize(m_pCutItem->rect().size().toSize(), false);
    }
}

void CCutTool::toolFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    mouseHoverEvent(event);
}

void CCutTool::mouseHoverEvent(IDrawTool::CDrawToolEvent *event)
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
    deleteCutItem(scene);

    scene->clearSelection();

    m_pCutItem = new CGraphicsCutItem(scene->sceneRect());
    m_pCutItem->setZValue(scene->getMaxZValue() + 1);
    scene->setMaxZValue(scene->getMaxZValue() + 1);
    scene->addItem(m_pCutItem);

    m_dragHandle = CSizeHandleRect::None;

    m_pCutItem->setIsFreeMode(true);
    m_pCutItem->setSelected(true);
    m_bModify = false;

    m_cutItems.insert(scene, m_pCutItem);
}

void CCutTool::deleteCutItem(CDrawScene *scene)
{
    drawApp->setApplicationCursor(Qt::ArrowCursor);

    auto itf = m_cutItems.find(scene);
    if (itf != m_cutItems.end()) {
        qDebug() << "deleteCutItem scene tag name = " << scene->getDrawParam()->viewName();
        CGraphicsCutItem *pCutItem = itf.value();
        scene->removeItem(pCutItem);

        if (pCutItem == m_pCutItem) {
            m_pCutItem = nullptr;
        }

        delete pCutItem;
        m_cutItems.erase(itf);
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

    CManageViewSigleton::GetInstance()->getCurView()->slotQuitCutMode();
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
        auto itf = m_cutItems.find(scene);
        if (itf != m_cutItems.end()) {
            return itf.value();
        }
    }
    return nullptr;
}

bool CCutTool::returnToSelectTool(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return false;
}
