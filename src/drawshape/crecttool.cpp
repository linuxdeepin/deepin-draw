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
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtMath>
#include <QGraphicsBlurEffect>

CRectTool::CRectTool ()
    : IDrawTool (rectangle)
    , m_pRectItem(nullptr)
{

}

CRectTool::~CRectTool()
{

}

void CRectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();

        m_sPointPress = event->scenePos();
        m_pRectItem = new CGraphicsRectItem(m_sPointPress.x(), m_sPointPress.y(), 0, 0);
        int redius = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRectXRedius();
        m_pRectItem->setXYRedius(redius, redius);
        m_pRectItem->setPen(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getPen());
        m_pRectItem->setBrush(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBrush());
//    QGraphicsBlurEffect *e0 = new QGraphicsBlurEffect();

//    e0->setBlurRadius(20);

//    m_pRectItem->setGraphicsEffect(e0);
        m_pRectItem->setZValue(scene->getMaxZValue() + 1);
        scene->addItem(m_pRectItem);

        m_bMousePress = true;
    } /*else if (event->button() == Qt::RightButton) {
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
        emit scene->signalChangeToSelect();
    }*/ else {
        scene->mouseEvent(event);
    }
}

void CRectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
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
        m_pRectItem->setRect(resultRect);
    }
}

void CRectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_sPointRelease = event->scenePos();
        //如果鼠标没有移动
        if ( m_pRectItem != nullptr) {
            if ( event->scenePos() == m_sPointPress ) {

                scene->removeItem(m_pRectItem);
                delete m_pRectItem;

            } else {
                emit scene->itemAdded(m_pRectItem);
                m_pRectItem->setSelected(true);
            }
        }
        m_pRectItem = nullptr;
        m_bMousePress = false;
    }
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
    emit scene->signalChangeToSelect();
    //TODO 如果没有拖动的功能   是否删除矩形
}

