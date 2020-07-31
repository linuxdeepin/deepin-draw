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
#include "crecttool.h"
#include "cdrawscene.h"
#include "cgraphicsrectitem.h"
#include "frame/cgraphicsview.h"

#include <QtMath>

CRectTool::CRectTool()
    : IDrawTool(rectangle)
{

}

CRectTool::~CRectTool()
{

}

void CRectTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsRectItem *pRectItem = dynamic_cast<CGraphicsRectItem *>(pInfo->businessItem);
        if (nullptr != pRectItem) {
            QPointF pointMouse = event->pos();
            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
            bool altKeyPress = event->keyboardModifiers() & Qt::AltModifier;
            QRectF resultRect;

            if (shiftKeyPress && !altKeyPress) {
                QPointF resultPoint = pointMouse;
                qreal w = resultPoint.x() - pInfo->_startPos.x();
                qreal h = resultPoint.y() - pInfo->_startPos.y();
                qreal abslength = abs(w) - abs(h);
                if (abslength >= 0.1) {
                    if (h >= 0) {
                        resultPoint.setY(pInfo->_startPos.y() + abs(w));
                    } else {
                        resultPoint.setY(pInfo->_startPos.y() - abs(w));
                    }

                } else {
                    if (w >= 0) {
                        resultPoint.setX(pInfo->_startPos.x() + abs(h));
                    } else {
                        resultPoint.setX(pInfo->_startPos.x() - abs(h));
                    }
                }
                QRectF rectF(pInfo->_startPos, resultPoint);
                resultRect = rectF.normalized();

            }
            //按下ALT键
            else if (!shiftKeyPress && altKeyPress) {

                QPointF point1 = pointMouse;
                QPointF centerPoint = pInfo->_startPos;
                QPointF point2 = 2 * centerPoint - point1;
                QRectF rectF(point1, point2);
                resultRect = rectF.normalized();
            }
            //ALT SHIFT都按下
            else if (shiftKeyPress && altKeyPress) {
                QPointF resultPoint = pointMouse;
                qreal w = resultPoint.x() - pInfo->_startPos.x();
                qreal h = resultPoint.y() - pInfo->_startPos.y();
                qreal abslength = abs(w) - abs(h);
                if (abslength >= 0.1) {
                    if (h >= 0) {
                        resultPoint.setY(pInfo->_startPos.y() + abs(w));
                    } else {
                        resultPoint.setY(pInfo->_startPos.y() - abs(w));
                    }

                } else {
                    if (w >= 0) {
                        resultPoint.setX(pInfo->_startPos.x() + abs(h));
                    } else {
                        resultPoint.setX(pInfo->_startPos.x() - abs(h));
                    }
                }
                QPointF point1 = resultPoint;
                QPointF centerPoint = pInfo->_startPos;
                QPointF point2 = 2 * centerPoint - point1;
                QRectF rectF(point1, point2);
                resultRect = rectF.normalized();
            } else {
                QPointF resultPoint = pointMouse;
                QRectF rectF(pInfo->_startPos, resultPoint);
                resultRect = rectF.normalized();
            }

            pRectItem->setRect(resultRect);
            event->setAccepted(true);
        }
    }
}

void CRectTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsRectItem *pRectItem = dynamic_cast<CGraphicsRectItem *>(pInfo->businessItem);
        if (nullptr != pRectItem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeItem(pRectItem);
                delete pRectItem;
            } else {
                if (pRectItem->scene() == nullptr) {
                    emit event->scene()->itemAdded(pRectItem);
                }
                pRectItem->setSelected(true);
            }
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CRectTool::creatItem(IDrawTool::CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        CGraphicsRectItem *m_pRectItem =  new CGraphicsRectItem(event->pos().x(), event->pos().y(), 0, 0);

        CGraphicsView *pView = event->scene()->drawView();
        int raduis = pView->getDrawParam()->getRectXRedius();
        m_pRectItem->setXYRedius(raduis, raduis);
        m_pRectItem->setPen(pView->getDrawParam()->getPen());
        m_pRectItem->setBrush(pView->getDrawParam()->getBrush());
        //m_pRectItem->setZValue(event->scene()->getMaxZValue() + 1);
        qreal newZ = event->scene()->getMaxZValue() + 1;
        m_pRectItem->setZValue(newZ);
        event->scene()->setMaxZValue(newZ);
        event->scene()->addItem(m_pRectItem);
        return m_pRectItem;
    }
    return nullptr;
}

