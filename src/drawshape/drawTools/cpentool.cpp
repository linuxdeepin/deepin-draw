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

            //QPainter painter(event->view()->viewport());
            //painter.drawPoint(event->pos(CDrawToolEvent::EViewportPos));

            QPixmap &pix = event->view()->cachPixMap();
            QPainter painter(&pix);
            //QTime ti;
            //ti.start();
            painter.setPen(pPenIem->pen());
            painter.drawPath(event->view()->mapFromScene(pPenIem->mapToScene(pPenIem->getPath())));
            //qDebug() << "used ==== " << ti.elapsed();
            event->view()->update();
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
                pInfo->businessItem = nullptr;
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
        //1.取消缓存，恢复到正常绘制
        event->view()->setPaintEnable(true);
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CPenTool::creatItem(CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        //为绘制效率做准备工作
        //1.准备一块缓存画布并且禁止自动刷新
        event->view()->setPaintEnable(false);

        CGraphicsPenItem *pPenItem = new CGraphicsPenItem(event->pos());
        pPenItem->setDrawFlag(true);

        CGraphicsView *pView = event->scene()->drawView();
        QPen pen = pView->getDrawParam()->getPen();
        pPenItem->setPen(pen);
        pPenItem->setBrush(pView->getDrawParam()->getBrush());
        pPenItem->setPenStartType(pView->getDrawParam()->getPenStartType());
        pPenItem->setPenEndType(pView->getDrawParam()->getPenEndType());
        //pPenItem->setPixmap();
        qreal newZ = event->scene()->getMaxZValue() + 1;
        pPenItem->setZValue(newZ);
        event->scene()->setMaxZValue(newZ);
        event->scene()->addItem(pPenItem);

        return pPenItem;
    }
    return nullptr;
}

int CPenTool::allowedMaxTouchPointCount()
{
    return 10;
}
