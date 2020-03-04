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
#include "cpolygonalstartool.h"
#include "cdrawscene.h"
#include "cgraphicspolygonalstaritem.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtMath>

CPolygonalStarTool::CPolygonalStarTool ()
    : IDrawTool (polygonalStar)
    , m_pPolygonalStarItem(nullptr)
{

}

CPolygonalStarTool::~CPolygonalStarTool()
{

}

void CPolygonalStarTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();

        m_sPointPress = event->scenePos();
        m_pPolygonalStarItem = new CGraphicsPolygonalStarItem(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getAnchorNum(),
                                                              CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRadiusNum(),
                                                              m_sPointPress.x(), m_sPointPress.y(), 0, 0);
        m_pPolygonalStarItem->setPen(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getPen());
        m_pPolygonalStarItem->setBrush(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBrush());
        scene->addItem(m_pPolygonalStarItem);


        m_bMousePress = true;
    } /*else if (event->button() == Qt::RightButton) {
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
        emit scene->signalChangeToSelect();
    }*/ else {
        scene->mouseEvent(event);
    }
}

void CPolygonalStarTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        QRectF resultRect;
        bool shiftKeyPress = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getShiftKeyStatus();
        bool altKeyPress = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getAltKeyStatus();
        //按下SHIFT键
        if (shiftKeyPress && !altKeyPress) {
            QPointF resultPoint = pointMouse;
            qreal w = resultPoint.x() - m_sPointPress.x();
            qreal h = resultPoint.y() - m_sPointPress.y();
            qreal abslength = abs(w) - abs(h);
            if (abslength >= 0.1) {
                if (h >= 0) {
                    resultPoint.setY(m_sPointPress.y() + abs(w));
                } else {
                    resultPoint.setY(m_sPointPress.y() - abs(w));
                }

            } else {
                if (w >= 0) {
                    resultPoint.setX(m_sPointPress.x() + abs(h));
                } else {
                    resultPoint.setX(m_sPointPress.x() - abs(h));
                }
            }
            QRectF rectF(m_sPointPress, resultPoint);
            resultRect = rectF.normalized();

        }
        //按下ALT键
        else if (!shiftKeyPress && altKeyPress) {

            QPointF point1 = pointMouse;
            QPointF centerPoint = m_sPointPress;
            QPointF point2 = 2 * centerPoint - point1;
            QRectF rectF(point1, point2);
            resultRect = rectF.normalized();
        }
        //ALT SHIFT都按下
        else if (shiftKeyPress && altKeyPress) {
            QPointF resultPoint = pointMouse;
            qreal w = resultPoint.x() - m_sPointPress.x();
            qreal h = resultPoint.y() - m_sPointPress.y();
            qreal abslength = abs(w) - abs(h);
            if (abslength >= 0.1) {
                if (h >= 0) {
                    resultPoint.setY(m_sPointPress.y() + abs(w));
                } else {
                    resultPoint.setY(m_sPointPress.y() - abs(w));
                }

            } else {
                if (w >= 0) {
                    resultPoint.setX(m_sPointPress.x() + abs(h));
                } else {
                    resultPoint.setX(m_sPointPress.x() - abs(h));
                }
            }

            QPointF point1 = resultPoint;
            QPointF centerPoint = m_sPointPress;
            QPointF point2 = 2 * centerPoint - point1;
            QRectF rectF(point1, point2);
            resultRect = rectF.normalized();

        }
        //都没按下
        else {
            QPointF resultPoint = pointMouse;
            QRectF rectF(m_sPointPress, resultPoint);
            resultRect = rectF.normalized();
        }
        m_pPolygonalStarItem->setRect(resultRect);
    }
}

void CPolygonalStarTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_sPointRelease = event->scenePos();
        //如果鼠标没有移动
        if ( m_pPolygonalStarItem != nullptr) {
            if ( event->scenePos() == m_sPointPress ) {
                scene->removeItem(m_pPolygonalStarItem);
                delete m_pPolygonalStarItem;
            } else {
                emit scene->itemAdded(m_pPolygonalStarItem);
                m_pPolygonalStarItem->setSelected(true);
            }
        }
        m_pPolygonalStarItem = nullptr;
        m_bMousePress = false;
    }

}
