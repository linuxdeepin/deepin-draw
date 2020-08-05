/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "clinetool.h"
#include "cdrawscene.h"
#include "cgraphicslineitem.h"
#include "cdrawparamsigleton.h"
#include "frame/cgraphicsview.h"

CLineTool::CLineTool()
    : IDrawTool(line)
{

}

CLineTool::~CLineTool()
{

}

void CLineTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    if (pInfo != nullptr) {
        CGraphicsLineItem *pItem = dynamic_cast<CGraphicsLineItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            pItem->resizeTo(CSizeHandleRect::RightBottom, pInfo->_prePos);
        }
    }
}

void CLineTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsLineItem *m_pItem = dynamic_cast<CGraphicsLineItem *>(pInfo->businessItem);
        if (nullptr != m_pItem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeItem(m_pItem);
                delete m_pItem;
                pInfo->businessItem = nullptr;
            } else {
                if (m_pItem->scene() == nullptr) {
                    emit event->scene()->itemAdded(m_pItem);
                }
                m_pItem->setSelected(true);
            }
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CLineTool::creatItem(IDrawTool::CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        CGraphicsView *pView = event->scene()->drawView();
        CGraphicsLineItem *m_pItem =  new CGraphicsLineItem(event->pos().x(), event->pos().y(), event->pos().x(), event->pos().y());
        m_pItem->setPen(pView->getDrawParam()->getPen());
        m_pItem->setBrush(pView->getDrawParam()->getBrush());
        m_pItem->setLineStartType(event->view()->getDrawParam()->getLineStartType());
        m_pItem->setLineEndType(event->view()->getDrawParam()->getLineEndType());
        qreal newZ = event->scene()->getMaxZValue() + 1;
        m_pItem->setZValue(newZ);
        event->scene()->setMaxZValue(newZ);
        event->scene()->addItem(m_pItem);
        return m_pItem;
    }
    return nullptr;
}

