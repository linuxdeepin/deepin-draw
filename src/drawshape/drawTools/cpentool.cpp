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

            QPixmap &pix = event->view()->cachedPixmap();
            QPainter painter(&pix);
            QPen p = pPenIem->pen();
            qreal penW = p.widthF() * event->view()->getScale();
            p.setWidthF(penW);
            painter.setPen(p);
            if (event->keyboardModifiers() != Qt::ShiftModifier) {
                auto activeLineIt = _activePaintedLines.find(event->uuid());
                if (activeLineIt != _activePaintedLines.end()) {
                    QLineF activeLine = activeLineIt.value();
                    if (!activeLine.isNull()) {
                        painter.drawLine(activeLine);
                        _activePaintedLines.erase(activeLineIt);
                    }
                }
                painter.drawLine(event->view()->mapFromScene(pInfo->_prePos), event->view()->mapFromScene(event->pos()));
            } else {
                auto activeLineIt = _activePaintedLines.find(event->uuid());
                if (activeLineIt == _activePaintedLines.end()) {
                    QLineF activeLine;
                    activeLine.setP1(event->view()->mapFromScene(pInfo->_prePos));
                    activeLine.setP2(event->view()->mapFromScene(event->pos()));
                    _activePaintedLines.insert(event->uuid(), activeLine);
                } else {
                    QLineF &activeLine = activeLineIt.value();
                    activeLine.setP2(event->view()->mapFromScene(event->pos()));
                }
            }
            event->view()->update();
            event->view()->viewport()->update();
        }
    }
}

void CPenTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsPenItem *pPenIem = dynamic_cast<CGraphicsPenItem *>(pInfo->businessItem);
        if (nullptr != pPenIem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeCItem(pPenIem);
                delete pPenIem;
                pInfo->businessItem = nullptr;
            } else {
                pPenIem->drawComplete();
                if (pPenIem->scene() == nullptr) {
                    pPenIem->drawScene()->addCItem(pPenIem);
                }
                // [BUG 28087] 所绘制的画笔未默认呈现选中状态
                //pPenIem->setSelected(true);
                pPenIem->setDrawFlag(false);
            }
        }
        //1.取消缓存，恢复到正常绘制
        event->view()->setCacheEnable(false);
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

void CPenTool::drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene)
{
    Q_UNUSED(rect)
    Q_UNUSED(scene)
    for (auto it = _allITERecordInfo.begin(); it != _allITERecordInfo.end(); ++it) {
        ITERecordInfo &pInfo = it.value();
        const CDrawToolEvent &curEvnt = pInfo._curEvent;
        CGraphicsPenItem *penItem = dynamic_cast<CGraphicsPenItem *>(pInfo.businessItem);
        if (penItem != nullptr) {
            QPen p = penItem->pen();
            //qreal penWMin = qMax(p.widthF(), 1.0);
            qreal penW = p.widthF() * scene->drawView()->getScale();
            p.setWidthF(penW);
            painter->setPen(p);
            if (curEvnt.keyboardModifiers() & Qt::ShiftModifier) {

                //要绘制悬而未决的直线
                for (auto b = _activePaintedLines.begin(); b != _activePaintedLines.end(); ++b) {
                    QLineF activeLine = b.value();
                    if (!activeLine.isNull())
                        painter->drawLine(activeLine);
                }
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

CGraphicsItem *CPenTool::creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        // 连续画线之前清除之前的选中图元
        event->scene()->clearSelectGroup();

        //启动缓冲绘制(会生成一张位图了替代原先的绘制)
        event->view()->setCacheEnable(true);

        CGraphicsPenItem *pPenItem = new CGraphicsPenItem(event->pos());
        pPenItem->setDrawFlag(true);

        CGraphicsView *pView = event->scene()->drawView();
        QPen pen = pView->getDrawParam()->getPen();
        pPenItem->setPen(pen);
        pPenItem->setBrush(pView->getDrawParam()->getBrush());
        pPenItem->setPenStartType(pView->getDrawParam()->getPenStartType());
        pPenItem->setPenEndType(pView->getDrawParam()->getPenEndType());
        event->scene()->addCItem(pPenItem);

        return pPenItem;
    }
    return nullptr;
}

void CPenTool::toolCreatItemStart(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (event->keyboardModifiers() & Qt::ShiftModifier) {
        QPointF pos = event->view()->mapFromScene(event->pos());
        QLineF line(pos, pos);
        _activePaintedLines.insert(event->uuid(), line);
    }
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
