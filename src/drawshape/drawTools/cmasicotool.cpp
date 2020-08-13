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

#include "cmasicotool.h"
#include "cgraphicsmasicoitem.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

CMasicoTool::CMasicoTool()
    : IDrawTool(blur)
{

}

CMasicoTool::~CMasicoTool()
{

}

void CMasicoTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsMasicoItem *pItem = dynamic_cast<CGraphicsMasicoItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            QPointF pointMouse = event->pos();
            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
            pItem->updatePenPath(pointMouse, shiftKeyPress);
            pItem->updateBlurPath();
            pItem->updateMasicPixmap();
            event->setAccepted(true);
        }
    }
}

void CMasicoTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsMasicoItem *pItem = dynamic_cast<CGraphicsMasicoItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeItem(pItem);
                delete pItem;
                pInfo->businessItem = nullptr;
            } else {
                if (pItem->scene() == nullptr) {
                    emit event->scene()->itemAdded(pItem);
                }
                pItem->setSelected(true);
            }
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CMasicoTool::creatItem(IDrawTool::CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {
        CGraphicsMasicoItem *pItem = new CGraphicsMasicoItem(event->pos());

        CGraphicsView *pView = event->scene()->drawView();
        QPen pen;
        QColor color(255, 255, 255, 0);
        pen.setColor(color);
        pen.setWidth(pView->getDrawParam()->getBlurWidth());
        pItem->setPen(pen);
        pItem->setBrush(Qt::NoBrush);
        qreal newZ = event->scene()->getMaxZValue() + 1;
        pItem->setZValue(newZ);
        event->scene()->setMaxZValue(newZ);
        event->scene()->addItem(pItem);
        pItem->updateMasicPixmap();
        return pItem;
    }
    return nullptr;
}
