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
    , m_buttonType(CButtonRect::NoneButton)
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
    m_bMousePress = true;
    //选中图元
    if (m_pCutItem != nullptr) {
        if (CButtonRect::NoneButton != m_buttonType) {
            dApp->setApplicationCursor(getCursor(CSizeHandleRect::None, m_bMousePress));
        } else {
            dApp->setApplicationCursor(getCursor(m_dragHandle, m_bMousePress));
        }
    }
}

void CCutTool::toolUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    m_pCutItem = getCurCutItem();
    if (nullptr != m_pCutItem && m_bMousePress) {
        if (m_dragHandle != CSizeHandleRect::None  && m_dragHandle != CSizeHandleRect::InRect) {
            m_pCutItem->resizeCutSize(m_dragHandle, pInfo->_prePos, event->pos(), &pInfo->_prePos);
            m_bModify = true;
            emit event->scene()->signalUpdateCutSize();
            dApp->topToolbar()->attributWidget()->getCutWidget()->setCutSize(m_pCutItem->rect().size().toSize(), false);
        } else {
            if (m_dragHandle == CSizeHandleRect::InRect) {
                m_bModify = true;
                m_pCutItem->move(pInfo->_prePos, event->pos());
            }
        }
    }
}

void CCutTool::toolFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    Q_UNUSED(event)
    m_bMousePress = false;

    m_pCutItem = getCurCutItem();
    if (m_pCutItem != nullptr) {
        //重绘矩形
        QRectF rect = m_pCutItem->rect();
        QPointF centerPoint = m_pCutItem->mapToScene(m_pCutItem->rect().center());
        rect.setRect(centerPoint.rx() - rect.width() * 0.5, centerPoint.ry() - rect.height() * 0.5, rect.width(), rect.height());
        m_pCutItem->setPos(0, 0);
        m_pCutItem->setRect(rect);
        m_pCutItem->update();

        if (CButtonRect::NoneButton != m_buttonType) {
            dApp->setApplicationCursor(getCursor(CSizeHandleRect::None, m_bMousePress));
        } else {
            dApp->setApplicationCursor(getCursor(m_dragHandle, m_bMousePress));
        }
    }
}

void CCutTool::mouseHoverEvent(IDrawTool::CDrawToolEvent *event)
{
    m_pCutItem = getCurCutItem();
    if (nullptr != m_pCutItem  && !m_bMousePress) {
        CSizeHandleRect::EDirection dragHandle = m_pCutItem->hitTest(event->pos());

        if (dragHandle != m_dragHandle) {
            m_dragHandle = dragHandle;
            dApp->setApplicationCursor(QCursor(getCursor(m_dragHandle, m_bMousePress)));
        }
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
    dApp->setApplicationCursor(Qt::ArrowCursor);

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

void CCutTool::changeCutType(int type, CDrawScene *scene)
{
    CGraphicsCutItem *pItem = getCutItem(scene);
    if (pItem != nullptr) {
        if (cut_free == type) {
            pItem->setIsFreeMode(true);
            pItem->setSelected(true);
        } else {
            pItem->setSelected(false);
            pItem->doChangeType(type);
        }
        m_bModify = true;
    }
}

void CCutTool::changeCutSize(const CDrawScene *scene, const QSize &size)
{
    CGraphicsCutItem *pItem = getCutItem(const_cast<CDrawScene *>(scene));
    if (pItem != nullptr) {
        pItem->doChangeSize(size.width(), size.height());
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

bool CCutTool::getModifyFlag() const
{
    return m_bModify;
}

void CCutTool::setModifyFlag(bool flag)
{
    m_bModify = flag;
}

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



