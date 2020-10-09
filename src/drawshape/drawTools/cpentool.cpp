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
#include "application.h"

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
            pPenIem->updatePenPath(pPenIem->mapFromScene(pointMouse), shiftKeyPress);
            event->setAccepted(true);

            QPixmap &pix = event->view()->cachPixMap();
            QPainter painter(&pix);
            QPen p = pPenIem->pen();
            qreal penW = p.widthF() * event->view()->getScale();
            p.setWidthF(penW);
            painter.setPen(p);
            painter.drawPath(event->view()->mapFromScene(pPenIem->mapToScene(pPenIem->getPath())));
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
                //pPenIem->setSelected(true);
                pPenIem->setDrawFlag(false);
            }
        }
        //1.取消缓存，恢复到正常绘制
        event->view()->setPaintEnable(true);
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

void CPenTool::drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene)
{
    Q_UNUSED(rect)
    Q_UNUSED(scene)

//    if (!(dApp->keyboardModifiers() & Qt::ShiftModifier)) {
//        return;
//    }

    for (auto it = _allITERecordInfo.begin(); it != _allITERecordInfo.end(); ++it) {
        ITERecordInfo &pInfo = it.value();
        CDrawToolEvent &curEvnt = pInfo._curEvent;
        CGraphicsPenItem *penItem = dynamic_cast<CGraphicsPenItem *>(pInfo.businessItem);
        if (penItem != nullptr) {
            QPen p = penItem->pen();
            //qreal penWMin = qMax(p.widthF(), 1.0);
            qreal penW = p.widthF() * scene->drawView()->getScale();
            p.setWidthF(penW);
            painter->setPen(p);
            if (curEvnt.keyboardModifiers() == Qt::ShiftModifier) {
                //要模拟绘制直线
                QPoint startPos = curEvnt.view()->mapFromScene(penItem->mapToScene(penItem->straightLine().p1()));
                QPoint endPos = curEvnt.view()->mapFromScene(penItem->mapToScene(penItem->straightLine().p2()));
                painter->drawLine(startPos, endPos);
            }

            painter->save();
            if (soildRing == penItem->getPenStartType() || soildArrow == penItem->getPenStartType()) {
                painter->setBrush(penItem->pen().color());
                painter->setPen(penItem->pen());
            }

            if (penItem->getPenStartType() != noneLine)
                painter->drawPath(scene->drawView()->mapFromScene(penItem->mapToScene(penItem->getPenStartpath())));
            painter->restore();


            if (soildRing == penItem->getPenEndType() || soildArrow == penItem->getPenEndType()) {
                painter->setBrush(penItem->pen().color());
                painter->setPen(penItem->pen());
            }

            if (penItem->getPenEndType() != noneLine)
                painter->drawPath(scene->drawView()->mapFromScene(penItem->mapToScene(penItem->getPenEndpath())));
        }
    }
}

CGraphicsItem *CPenTool::creatItem(CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        // 连续画线之前清除之前的选中图元
        event->scene()->clearMrSelection();

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
        qreal newZ = event->scene()->getMaxZValue() + 1;
        pPenItem->setZValue(newZ);
        event->scene()->setMaxZValue(newZ);
        event->scene()->addItem(pPenItem);

        return pPenItem;
    }
    return nullptr;
}

void CPenTool::toolCreatItemStart(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    return IDrawTool::toolCreatItemStart(event, pInfo);
}

int CPenTool::allowedMaxTouchPointCount()
{
    return 10;
}

bool CPenTool::returnToSelectTool(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    return !pInfo->hasMoved();
}
