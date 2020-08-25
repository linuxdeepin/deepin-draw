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
#include "cgraphicscutitem.h"
#include "csizehandlerect.h"
#include "frame/ccutwidget.h"
#include "globaldefine.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QPushButton>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

//const int CORNER_WITH = 20;

CGraphicsCutItem::CGraphicsCutItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    initHandle();
}

CGraphicsCutItem::CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();

    m_originalRect = QRectF(0, 0, 0, 0);
    m_originalRect.setSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutDefaultSize());

    initHandle();
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutSize(rect.size().toSize());
}

CGraphicsCutItem::CGraphicsCutItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    m_originalRect = QRectF(0, 0, 0, 0);
    m_originalRect.setSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutDefaultSize());
    initHandle();
}

CGraphicsCutItem::~CGraphicsCutItem()
{

}

int CGraphicsCutItem::type() const
{
    return CutType;
}

void CGraphicsCutItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    updateGeometry();

    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutSize(rect.size().toSize());
}

void CGraphicsCutItem::initHandle()
{
    clearHandle();
    // 子handles 用于处理重设大小
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Left; ++i) {
        CSizeHandleRect *shr = nullptr;
        shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        shr->setJustExitLogicAbility(true);
        m_handles.push_back(shr);
    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsCutItem::updateGeometry()
{
    const QRectF &geom = this->rect();

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;
        qreal w = hndl->boundingRect().width();
        qreal h = hndl->boundingRect().height();
        switch (hndl->dir()) {
        case CSizeHandleRect::LeftTop:
            hndl->move(geom.x() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Top:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::RightTop:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Right:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::RightBottom:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Bottom:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::LeftBottom:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Left:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        default:
            break;
        }
    }
}

QRectF CGraphicsCutItem::rect() const
{
    return QRectF(m_topLeftPoint, m_bottomRightPoint);
}

QRectF CGraphicsCutItem::boundingRect() const
{
//    QRectF rect = this->rect();
//    QRectF bounding = QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
//                             rect.width() + pen().width(), rect.height() + pen().width());
//    return bounding;

    if (scene() != nullptr) {
        return scene()->sceneRect();
    }
    return QRectF(0, 0, 0, 0);
}

void CGraphicsCutItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    bool shiftKeyPress = false;//CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
    bool altKeyPress = false;//CDrawParamSigleton::GetInstance()->getAltKeyStatus();

    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutType() == cut_original) {
        return;
    } else if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutType() != cut_free) {
        shiftKeyPress = true;
    }


    //resizeCutSize(dir, point);
    resizeTo(dir, point, shiftKeyPress, altKeyPress);
}

void CGraphicsCutItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    QPointF local = mapFromScene(point);
    QRectF rect = this->rect();
    QPointF topLeft = rect.topLeft();
    QPointF centerPoint = rect.center();
    qreal w = rect.width();
    qreal h = rect.height();
    qreal scale = w / h;//长宽比

    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;

    if (!shiftKeyPress && !altKeyPress) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() >= 10) {
                rect.setRight(local.x());
            } else {
                rect.setRight(rect.left() + 10);
            }
            break;
        case CSizeHandleRect::RightTop:
            /*if (local.x() - rect.left() >= 10 &&  rect.bottom() - local.y() >= 10) {
                rect.setTopRight(local);
            } else if (local.x() - rect.left() < 10) {
                rect.setRight(local.x());
            }*/
            if (local.x() - rect.left() >= 10) {
                rect.setRight(local.x());
            } else {
                rect.setRight(rect.left() + 10);
            }

            if (rect.bottom() - local.y() >= 10) {
                rect.setTop(local.y());
            } else {
                rect.setTop(rect.bottom() - 10);
            }

            break;
        case CSizeHandleRect::RightBottom:
//            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
//                rect.setBottomRight(local);
//            }
            if (local.x() - rect.left() >= 10) {
                rect.setRight(local.x());
            } else {
                rect.setRight(rect.left() + 10);
            }

            if (local.y() - rect.top() >= 10) {
                rect.setBottom(local.y());
            } else {
                rect.setBottom(rect.top() + 10);
            }
            break;
        case CSizeHandleRect::LeftBottom:
//            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
//                rect.setBottomLeft(local);
//            }
            if (rect.right() - local.x() >= 10) {
                rect.setLeft(local.x());
            } else {
                rect.setLeft(rect.right() - 10);
            }

            if (local.y() - rect.top() >= 10) {
                rect.setBottom(local.y());
            } else {
                rect.setBottom(rect.top() + 10);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() >= 10) {
                rect.setBottom(local.y());
            } else {
                rect.setBottom(rect.top() + 10);
            }
            break;
        case CSizeHandleRect::LeftTop:
//            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
//                rect.setTopLeft(local);
//            }
            if (rect.right() - local.x() >= 10) {
                rect.setLeft(local.x());
            } else {
                rect.setLeft(rect.right() - 10);
            }

            if (rect.bottom() - local.y() >= 10) {
                rect.setTop(local.y());
            } else {
                rect.setTop(rect.bottom() - 10);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() >= 10) {
                rect.setLeft(local.x());
            } else {
                rect.setLeft(rect.right() - 10);
            }
            break;
        case CSizeHandleRect::Top:
            if (rect.bottom() - local.y() >= 10) {
                rect.setTop(local.y());
            } else {
                rect.setTop(rect.bottom() - 10);
            }
            break;
        default:
            break;
        }
    }
    //按住SHIFT等比拉伸
    else if ((shiftKeyPress && !altKeyPress)) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() >= 0.1) {
                //变换后的宽度和高度
                qreal w2 = local.x() - rect.left();
                if (w2 < 10) {
                    w2 = 10;
                }
                qreal h2 = w2 / scale;
                topLeft.setY(topLeft.y() - (h2 - h) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                //rect.setTopRight(local);
                QPointF bottomLeft = rect.bottomLeft();
                qreal w2 = local.x() - bottomLeft.x();
                qreal h2 = bottomLeft.y() - local.y();

//                if (h2 * scale >= w2) {
//                    w2 = h2 * scale;
//                } else {
//                    h2 = w2 / scale;
//                }
                if (scale >= 1) {
                    if (h2 < 10) {
                        h2 = 10;
                    }
                    w2 = h2 * scale;
                } else {
                    if (w2 < 10) {
                        w2 = 10;
                    }
                    h2 = w2 / scale;
                }
                topLeft.setY(topLeft.y() - (h2 - h));

                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                //rect.setBottomRight(local);

                qreal w2 = local.x() - topLeft.x();
                qreal h2 = local.y() - topLeft.y();
                if (scale >= 1) {
                    if (h2 < 10) {
                        h2 = 10;
                    }
                    w2 = h2 * scale;
                } else {
                    if (w2 < 10) {
                        w2 = 10;
                    }
                    h2 = w2 / scale;
                }
//                if (h2 * scale >= w2) {
//                    if (h2 < 10) {
//                        h2 = 10;
//                    }
//                    w2 = h2 * scale;
//                } else {

//                    if (w2 < 10) {
//                        w2 = 10;
//                    }
//                    h2 = w2 / scale;
//                }

                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
                //rect.setBottomLeft(local);
                QPointF topRight = rect.topRight();
                qreal w2 = topRight.x() - local.x();
                qreal h2 = local.y() - topRight.y();
//                if (h2 * scale >= w2) {
//                    w2 = h2 * scale;
//                } else {
//                    h2 = w2 / scale;
//                }
                if (scale >= 1) {
                    if (h2 < 10) {
                        h2 = 10;
                    }
                    w2 = h2 * scale;
                } else {
                    if (w2 < 10) {
                        w2 = 10;
                    }
                    h2 = w2 / scale;
                }
                topLeft.setX(topRight.x() - w2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);

            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1) {
                //rect.setBottom(local.y());
                qreal h2 = local.y() - topLeft.y();
                qreal w2 = 0;

                if (scale >= 1) {
                    if (h2 < 10) {
                        h2 = 10;
                    }
                    w2 = h2 * scale;
                } else {
                    if (h2 * scale < 10) {
                        h2 = 10 / scale;
                    }
                    w2 = h2 * scale;
                }

                topLeft.setX(topLeft.x() - (w2 - w) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1) {
                //rect.setTopLeft(local);
                QPointF bottomRight = rect.bottomRight();
                qreal w2 = bottomRight.x() - local.x();
                qreal h2 = bottomRight.y() - local.y();
//                if (h2 * scale >= w2) {
//                    w2 = h2 * scale;
//                } else {
//                    h2 = w2 / scale;
//                }
                if (scale >= 1) {
                    if (h2 < 10) {
                        h2 = 10;
                    }
                    w2 = h2 * scale;
                } else {
                    if (w2 < 10) {
                        w2 = 10;
                    }
                    h2 = w2 / scale;
                }
                topLeft = bottomRight - QPointF(w2, h2);
                rect.setTopLeft(topLeft);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1) {
                //rect.setLeft(local.x());
                qreal w2 = rect.right() - local.x();
                if (w2 < 10) {
                    w2 = 10;
                }
                qreal h2 = w2 / scale;
                topLeft.setX(rect.right() - w2);
                topLeft.setY(topLeft.y() - (h2 - h) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1) {
                //rect.setTop(local.y());

                qreal h2 = rect.bottom() - local.y();
                qreal w2 = 0;

                if (scale >= 1) {
                    if (h2 < 10) {
                        h2 = 10;
                    }
                    w2 = h2 * scale;
                } else {
                    if (h2 * scale < 10) {
                        h2 = 10 / scale;
                    }
                    w2 = h2 * scale;
                }

                topLeft.setX(topLeft.x() - (w2 - w) / 2);
                topLeft.setY(rect.bottom() - h2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        default:
            break;
        }
    }
    //中心拉伸
    /*else if ((!shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                //rect.setRight(local.x());
                rect.setRight(local.x());
                rect.setLeft(centerPoint.x() * 2 - local.x());
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                rect.setTopRight(local);
                rect.setBottomLeft(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomRight(local);
                rect.setTopLeft(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomLeft(local);
                rect.setTopRight(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                //rect.setBottom(local.y());
                rect.setBottom(local.y());
                rect.setTop(centerPoint.y() * 2 - local.y());
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                rect.setTopLeft(local);
                rect.setBottomRight(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                rect.setLeft(local.x());
                rect.setRight(2 * centerPoint.x() - local.x());
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                rect.setTop(local.y());
                rect.setBottom(2 * centerPoint.y() - local.y());
            }
            break;
        default:
            break;
        }
    }*/
    //等比中心拉伸
    /*else if ((shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                //变换后的宽度和高度
                qreal h2 = local.x() - centerPoint.x();
                qreal w2 = h2 * scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                qreal w2 = local.x() - centerPoint.x();
                qreal h2 = centerPoint.y() - local.y();

                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                //rect.setBottomRight(local);

                qreal w2 = local.x() - centerPoint.x();
                qreal h2 = local.y() - centerPoint.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {

                qreal w2 = centerPoint.x() - local.x();
                qreal h2 = local.y() - centerPoint.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                qreal h2 = local.y() - centerPoint.y();
                qreal w2 = h2 * scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                qreal w2 = centerPoint.x() - local.x();
                qreal h2 = centerPoint.y() - local.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                qreal w2 = centerPoint.x() - local.x();
                qreal h2 = w2 / scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                qreal h2 = centerPoint.y() - local.y();
                qreal w2 = h2 * scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        default:
            break;
        }
    }*/

    rect = rect.normalized();
    centerPoint = mapToScene(rect.center());
    rect.setRect(centerPoint.rx() - rect.width() * 0.5, centerPoint.ry() - rect.height() * 0.5, rect.width(), rect.height());
    prepareGeometryChange();
    setTransformOriginPoint(centerPoint);
    this->setPos(0, 0);

    this->setRect(rect);
    updateGeometry();
}

void CGraphicsCutItem::move(QPointF beginPoint, QPointF movePoint)
{
    QPointF adjust = movePoint - beginPoint;
    setRect(rect().adjusted(adjust.x(), adjust.y(), adjust.x(), adjust.y()));
}

CSizeHandleRect::EDirection CGraphicsCutItem::hitTest(const QPointF &point) const
{
    const Handles::const_iterator hend =  m_handles.end();
    for (Handles::const_iterator it = m_handles.begin(); it != hend; ++it) {
        if ((*it)->hitTest(point)) {
            return (*it)->dir();
        }
    }
    //检测是否在矩形内
    QPointF pt = mapFromScene(point);
    if (this->rect().contains(pt)) {
        return CSizeHandleRect::InRect;
    }

    return CSizeHandleRect::None;
}

void CGraphicsCutItem::showControlRects(bool flag)
{
    setState(flag ? SelectionHandleActive : SelectionHandleOff);
}

bool CGraphicsCutItem::isFreeMode() const
{
    return m_isFreeMode;
}

void CGraphicsCutItem::setIsFreeMode(bool isFreeMode)
{
    m_isFreeMode = isFreeMode;
    showControlRects(true/*isFreeMode*/);
}

void CGraphicsCutItem::duplicate(CGraphicsItem *item)
{
    Q_UNUSED(item)
}

void CGraphicsCutItem::resizeCutSize(CSizeHandleRect::EDirection dir,
                                     const QPointF &prePoint,
                                     const QPointF &point, QPointF *outAcceptPos)
{
    //得到在自身坐标系内的当前鼠标位置
    QPointF preLocalPos = mapFromScene(prePoint);
    QPointF curLocalPos = mapFromScene(point);
    QRectF  curRect     = rect();
    qreal   qWHRadio    = getWHRadio();

    qreal moveX = (curLocalPos - preLocalPos).x();
    qreal moveY = (curLocalPos - preLocalPos).y();
    qreal adjust[4] = {0, 0, 0, 0};


    bool getBorder = false;

    qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
    qreal minH = (qWHRadio < 1.0 && qWHRadio > 0.0) ? 10 / qWHRadio : 10;

    //根据dir的位置判断应该改变到的大小
    switch (dir) {
    case CSizeHandleRect::Top: {
        adjust[1] = moveY;

        //边界检查
        if (curRect.height() - adjust[1] < minH) {
            adjust[1] = curRect.height() - minH;
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[2] = -adjust[1] * qWHRadio;
        }
        break;
    }

    case CSizeHandleRect::Bottom: {
        adjust[3] = moveY;

        //边界检查
        if (curRect.height() + adjust[3] < minH) {
            adjust[3] = minH - curRect.height();
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[2] = adjust[3] * qWHRadio;
        }
        break;
    }
    case CSizeHandleRect::Left: {
        adjust[0] = moveX;

        //边界检查
        if (curRect.width() - adjust[0] < minW) {
            adjust[0] = curRect.width() - minW;
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[3] = -adjust[0] / qWHRadio;
        }

        break;
    }
    case CSizeHandleRect::Right: {
        adjust[2] = moveX;

        //边界检查
        if (curRect.width() + adjust[2] < minW) {
            adjust[2] = minW - curRect.width();
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[3] = adjust[2] / qWHRadio;
            getBorder = true;
        }
        break;
    }
    case CSizeHandleRect::RightTop: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[2] = moveX;

                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() + adjust[2] < minW) {
                    adjust[2] = minW - curRect.width();
                    getBorder = true;
                }

                adjust[1] = -adjust[2] / qWHRadio;
            } else {
                adjust[1] = moveY;

                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() - adjust[1] < minH) {
                    adjust[1] = curRect.height() - minH;
                    getBorder = true;
                }

                adjust[2] = -adjust[1] * qWHRadio;
            }
        } else {

            adjust[2] = moveX;
            adjust[1] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() + adjust[2] < minW) {
                adjust[2] = minW - curRect.width();
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
            if (curRect.height() - adjust[1] < minH) {
                adjust[1] = curRect.height() - minH;
                getBorder = true;
            }
        }

        break;
    }
    case CSizeHandleRect::LeftTop: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[0] = moveX;
                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() - adjust[0] < minW) {
                    adjust[0] = curRect.width() - minW;
                    getBorder = true;
                }
                adjust[1] = adjust[0] / qWHRadio;
            } else {
                adjust[1] = moveY;
                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() - adjust[1] < minH) {
                    adjust[1] = curRect.height() - minH;
                    getBorder = true;
                }
                adjust[0] = adjust[1] * qWHRadio;
            }
        } else {
            adjust[0] = moveX;
            adjust[1] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() - adjust[0] < minW) {
                adjust[0] = curRect.width() - minW;
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
            if (curRect.height() - adjust[1] < minH) {
                adjust[1] = curRect.height() - minH;
                getBorder = true;
            }
        }
        break;
    }

    case CSizeHandleRect::RightBottom: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[2] = moveX;

                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() + adjust[2] < minW) {
                    adjust[2] = minW - curRect.width();
                    getBorder = true;
                }

                adjust[3] = adjust[2] / qWHRadio;
            } else {
                adjust[3] = moveY;

                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() + adjust[3] < minH) {
                    adjust[3] = minH - curRect.height();
                    getBorder = true;
                }

                adjust[2] = adjust[3] * qWHRadio;
            }
        } else {
            adjust[2] = moveX;
            adjust[3] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() + adjust[2] < minW) {
                adjust[2] = minW - curRect.width();
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
            if (curRect.height() + adjust[3] < minH) {
                adjust[3] = minH - curRect.height();
                getBorder = true;
            }
        }

        break;
    }

    case CSizeHandleRect::LeftBottom: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[0] = moveX;

                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() - adjust[0] < minW) {
                    adjust[0] = curRect.width() - minW;
                    getBorder = true;
                }

                adjust[3] = -adjust[0] / qWHRadio;
            } else {
                adjust[3] = moveY;

                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() + adjust[3] < minH) {
                    adjust[3] = minH - curRect.height();
                    getBorder = true;
                }

                adjust[0] = -adjust[3] * qWHRadio;
            }
        } else {
            adjust[0] = moveX;
            adjust[3] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() - adjust[0] < minW) {
                adjust[0] = curRect.width() - minW;
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 * qWHRadio : 10;
            if (curRect.height() + adjust[3] < minH) {
                adjust[3] = minH - curRect.height();
                getBorder = true;
            }

        }
        break;
    }
    default:
        break;
    }

    if (outAcceptPos != nullptr) {
        if (getBorder) {
            *outAcceptPos = (prePoint + QPointF(adjust[0] + adjust[2], adjust[1] + adjust[3]));
        } else {
            *outAcceptPos = point;
        }
    }

    curRect.adjust(adjust[0], adjust[1], adjust[2], adjust[3]);

    prepareGeometryChange();

    this->setRect(curRect);

    updateGeometry();
}

qreal CGraphicsCutItem::getWHRadio()
{
    qreal   qwhRadio    = -1;
    CGraphicsView *pView = curView();
    if (pView != nullptr) {
        if (!isFreeMode()) {
            ECutType cutTp = pView->getDrawParam()->getCutType();
            switch (cutTp) {
            case cut_1_1:
                qwhRadio = 1.0;
                break;
            case cut_2_3:
                qwhRadio = 2.0 / 3.0;
                break;
            case cut_8_5:
                qwhRadio = 8.0 / 5.0;
                break;
            case cut_16_9:
                qwhRadio = 16.0 / 9.0;
                break;
            case cut_original:
                qwhRadio = scene()->sceneRect().width() / scene()->sceneRect().height();
                break;
            default:
                break;
            }
        }
    }
    return qwhRadio;
}

void CGraphicsCutItem::doChangeType(int type)
{
    QRectF rect = this->scene()->sceneRect();
    QPointF centerPos = rect.center();
    qreal w = rect.width();
    qreal h = rect.height();
    QPointF topLeft;
    QPointF rightBottom;
    qreal bigW = w;
    qreal bigH = h;

    if (type == cut_original) {
        bigW = m_originalRect.width();
        bigH = m_originalRect.height();
    } else {
        switch (type) {
        case cut_1_1:
            bigH = bigW;
            break;
        case cut_2_3:
            bigH = bigW * 3 / 2;
            break;
        case cut_8_5:
            bigH = bigW * 5 / 8;
            break;
        case cut_16_9:
            bigH = bigW * 9 / 16;
            break;
        default:
            break;

        }


        qreal scale = (bigW / bigH);


        if (bigW > w || bigH > h) {
            if (scale >= (w / h)) {
                bigW = w;
                bigH = (bigW / scale);
            } else {
                bigH = h;
                bigW = (bigH * scale);
            }

        }

    }

    topLeft = centerPos - QPointF(bigW / 2, bigH / 2);
    rightBottom = centerPos + QPointF(bigW / 2, bigH / 2);
    this->setRect(QRectF(topLeft, rightBottom));

    setIsFreeMode(false);
}

void CGraphicsCutItem::doChangeSize(int w, int h)
{
    //    if (w > m_originalRect.width() || h > m_originalRect.height()) {
    //        return;
    //    }
    setRect(QRectF(rect().x(), rect().y(), w, h));
}

QVariant CGraphicsCutItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    Q_UNUSED(change);
    if (scene() != nullptr && !scene()->views().isEmpty()) {
        scene()->views().first()->viewport()->update();
    }
    return value;
}

void CGraphicsCutItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateGeometry();

    QColor penColor = QColor("#ffffff");
    penColor.setAlpha(qRound(255 * 0.7));
    //    int themValue = CManageViewSigleton::GetInstance()->getThemeType();
    //    if (themValue == 1) {
    //        //浅色主题
    //        penColor = QColor("#979797");
    //    } else if (themValue == 2) {
    //        //深色主题
    //        penColor = QColor("#FFFFFF");
    //    }

    //先绘制一层阴影
    QColor bgColor(0, 0, 0, int(255.0 * 40.0 / 100.0));
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);

    painter->translate(-pos());

    QPainterPath fillPath;
    QRectF itemRect = mapRectToScene(rect());
    QRectF unitRct  = boundingRect();
    fillPath.addRect(unitRct);
    if (unitRct.intersects(itemRect)) {
        fillPath.addRect(itemRect);
    }
    painter->drawPath(fillPath);

    painter->restore();

    qreal penWidth = 1.0 / painter->worldTransform().m11();
    QRectF rct = rect().adjusted(penWidth, penWidth, -penWidth, -penWidth);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(penColor);
    pen.setWidthF(penWidth);
    painter->setPen(pen);

    //画三等分矩形的直线
    drawTrisectorRect(painter);

    //画矩形
    painter->save();
    QPen rectPen(pen);
    rectPen.setStyle(Qt::DashLine);
    painter->setPen(rectPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rct);
    painter->restore();

    drawFourConner(painter);
}

void CGraphicsCutItem::drawTrisectorRect(QPainter *painter)
{
    qreal penWidth = 1.0 / painter->worldTransform().m11();
    QPainterPath path;
    QRectF rct = rect().adjusted(penWidth, penWidth, -penWidth, -penWidth);
    path.moveTo(rct.x(), rct.y() + rct.height() / 3);
    path.lineTo(rct.x() + rct.width(), rct.y() + rct.height() / 3);

    path.moveTo(rct.x(), rct.y() + rct.height() / 3 * 2);
    path.lineTo(rct.x() + rct.width(), rct.y() + rct.height() / 3 * 2);

    path.moveTo(rct.x() + rct.width() / 3, rct.y());
    path.lineTo(rct.x() + rct.width() / 3, rct.y() + rct.height());

    path.moveTo(rct.x() + rct.width() / 3 * 2, rct.y());
    path.lineTo(rct.x() + rct.width() / 3 * 2, rct.y() + rct.height());

    painter->drawPath(path);
}

void CGraphicsCutItem::drawFourConner(QPainter *painter/*, QPainterPath &path, const int penWidth*/)
{
    qreal penWidth = 1.0 / painter->worldTransform().m11();
    QRectF rct = rect().adjusted(penWidth, penWidth, -penWidth, -penWidth);
    //QRectF rct = boundingRect();
    bool isMinSize = (qFuzzyIsNull(rect().width() - 10.0) && qFuzzyIsNull(rect().height() - 10.0));
    if (isMinSize) {
        QPen pen(painter->pen());
        pen.setWidthF(1.0);
        painter->setPen(pen);
        pen.setStyle(Qt::SolidLine);
        painter->setBrush(Qt::gray);
        painter->drawRect(rct);

        QLineF topLine   = QLineF(rct.topLeft(), rct.topRight());
        QLineF botLine   = QLineF(rct.bottomLeft(), rct.bottomRight());
        QLineF leftLine  = QLineF(rct.topLeft(), rct.bottomLeft());
        QLineF rightLine = QLineF(rct.topRight(), rct.bottomRight());

        painter->setPen(Qt::gray);
        painter->drawLine(topLine.p1(), topLine.center());
        painter->drawLine(rightLine.p1(), rightLine.center());
        painter->drawLine(botLine.center(), botLine.p2());
        painter->drawLine(leftLine.center(), leftLine.p2());

        painter->setPen(Qt::black);
        painter->drawLine(topLine.center(), topLine.p2());
        painter->drawLine(rightLine.center(), rightLine.p2());
        painter->drawLine(botLine.center(), botLine.p1());
        painter->drawLine(leftLine.center(), leftLine.p1());
    } else {
        QPainterPath path;
        QPen pen(painter->pen());
        pen.setStyle(Qt::SolidLine);
        pen.setWidthF(2.0 / painter->worldTransform().m11());
        pen.setColor(Qt::white);
        painter->setPen(pen);
        qreal penWidth = 0;
        const qreal sameLen = 6.0 / painter->worldTransform().m11();
        qreal CORNER_W = sameLen;
        qreal CORNER_H = sameLen;

        if (rct.width() < 2 * CORNER_W) {
            CORNER_W = rct.width() / 2.0;
        }
        if (rct.height() < 2 * CORNER_H) {
            CORNER_H = rct.height() / 2.0;
        }
        qreal CORNER_WITH = qMin(CORNER_W, CORNER_H);

        //左上角
        path.moveTo(rct.x() + penWidth / 2, rct.y() + CORNER_WITH);
        path.lineTo(rct.x() + penWidth / 2, rct.y()  + penWidth / 2);
        path.lineTo(rct.x() + CORNER_WITH, rct.y()  + penWidth / 2);
        //右上角
        path.moveTo(rct.x() + rct.width() - CORNER_WITH, rct.y() + penWidth / 2);
        path.lineTo(rct.x() + rct.width() - penWidth / 2, rct.y()  + penWidth / 2);
        path.lineTo(rct.x() + rct.width() - penWidth / 2, rct.y() + CORNER_WITH);
        //右下角
        path.moveTo(rct.x() + rct.width() - penWidth / 2, rct.y() + rct.height() - CORNER_WITH);
        path.lineTo(rct.x() + rct.width() - penWidth / 2, rct.y()  + rct.height() - penWidth / 2);
        path.lineTo(rct.x() + rct.width() - CORNER_WITH, rct.y() + rct.height() - penWidth / 2);

        //左下角
        path.moveTo(rct.x() + CORNER_WITH + penWidth / 2, rct.y() + rct.height() - penWidth / 2);
        path.lineTo(rct.x() + penWidth / 2, rct.y()  + rct.height() - penWidth / 2);
        path.lineTo(rct.x() + penWidth / 2, rct.y() + rct.height() - CORNER_WITH);

        painter->drawPath(path);
    }
}
