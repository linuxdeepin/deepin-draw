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

#include "cpentool.h"
#include "cdrawscene.h"
#include "cgraphicspenitem.h"
#include "frame/cgraphicsview.h"

CPenTool::CPenTool()
    : IDrawTool(pen)
{

}

CPenTool::~CPenTool()
{

}

void CPenTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsPenItem *pPenIem = dynamic_cast<CGraphicsPenItem *>(pInfo->businessItem);
        if (nullptr != pPenIem) {
            QPointF pointMouse = event->pos();
            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
            pPenIem->updatePenPath(pointMouse, shiftKeyPress);
            event->setAccepted(true);
        }
    }
}

void CPenTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsPenItem *pPenIem = dynamic_cast<CGraphicsPenItem *>(pInfo->businessItem);
        if (nullptr != pPenIem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeItem(pPenIem);
                delete pPenIem;
            } else {
                pPenIem->drawComplete();
                if (pPenIem->scene() == nullptr) {
                    emit event->scene()->itemAdded(pPenIem);
                }
                // [BUG 28087] 所绘制的画笔未默认呈现选中状态
                pPenIem->setSelected(true);
                pPenIem->setDrawFlag(false);
            }
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CPenTool::creatItem(CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {
        CGraphicsPenItem *pPenItem = new CGraphicsPenItem(event->pos());
        pPenItem->setDrawFlag(true);

        CGraphicsView *pView = event->scene()->drawView();
        QPen pen = pView->getDrawParam()->getPen();
        pPenItem->setPen(pen);
        pPenItem->setBrush(pView->getDrawParam()->getBrush());
        pPenItem->setPenStartType(pView->getDrawParam()->getPenStartType());
        pPenItem->setPenEndType(pView->getDrawParam()->getPenEndType());
        pPenItem->setPixmap();
        pPenItem->setZValue(event->scene()->getMaxZValue() + 1);
        event->scene()->addItem(pPenItem);

        return pPenItem;
    }
    return nullptr;
}
