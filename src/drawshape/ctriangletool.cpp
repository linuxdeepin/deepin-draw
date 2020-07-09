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
#include "ctriangletool.h"
#include "cdrawscene.h"
#include "cgraphicstriangleitem.h"
#include "frame/cgraphicsview.h"

#include <QtMath>

CTriangleTool::CTriangleTool()
    : IDrawTool(ellipse)
{

}

CTriangleTool::~CTriangleTool()
{

}

void CTriangleTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsTriangleItem *pItem = dynamic_cast<CGraphicsTriangleItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            QPointF pointMouse = event->pos();
            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
            bool altKeyPress = event->keyboardModifiers() & Qt::AltModifier;
            QRectF resultRect;
            //按下SHIFT键
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
            }
            //都没按下
            else {
                QPointF resultPoint = pointMouse;
                QRectF rectF(pInfo->_startPos, resultPoint);
                resultRect = rectF.normalized();
            }
            pItem->setRect(resultRect);
        }
    }
}

void CTriangleTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsTriangleItem *m_pItem = dynamic_cast<CGraphicsTriangleItem *>(pInfo->businessItem);
        if (nullptr != m_pItem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeItem(m_pItem);
                delete m_pItem;
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

CGraphicsItem *CTriangleTool::creatItem(IDrawTool::CDrawToolEvent *event)
{
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        CGraphicsTriangleItem *m_pItem =  new CGraphicsTriangleItem(event->pos().x(), event->pos().y(), 0, 0);
        CGraphicsView *pView = event->scene()->drawView();
        m_pItem->setPen(pView->getDrawParam()->getPen());
        m_pItem->setBrush(pView->getDrawParam()->getBrush());
        m_pItem->setZValue(event->scene()->getMaxZValue() + 1);
        event->scene()->addItem(m_pItem);
        return m_pItem;
    }
    return nullptr;
}

