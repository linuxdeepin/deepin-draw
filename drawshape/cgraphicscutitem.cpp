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


#include <QPushButton>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>



const int CORNER_WITH = 20;
//const QString PenColorName = "#B5B5B5";
const QString PenColorName = "#00BFFF";
//const QString PenColorName = "#ECECF8";

CGraphicsCutItem::CGraphicsCutItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    initPenAndBrush();
    initRect();
}

CGraphicsCutItem::CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
//    this->setRect(rect);

    m_originalRect = QRectF(0, 0, 0, 0);
    m_originalRect.setSize(CDrawParamSigleton::GetInstance()->getCutDefaultSize());

    initPenAndBrush();
    initRect();
    CDrawParamSigleton::GetInstance()->setCutSize(rect.size().toSize());
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
    m_originalRect.setSize(CDrawParamSigleton::GetInstance()->getCutDefaultSize());
    initPenAndBrush();
    initRect();
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

    CDrawParamSigleton::GetInstance()->setCutSize(rect.size().toSize());
    //this->scene()->setSceneRect(rect);
}

void CGraphicsCutItem::initRect()
{

//    m_cancelBtn = new QPushButton();
//    m_cancelBtn->setText("Cancel");
//    m_cancelBtn->setFixedSize(40, 20);

//    m_proxy = new QGraphicsProxyWidget(this);
//    m_proxy->setWidget(m_cancelBtn);
    // handles
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Left; ++i) {
        CSizeHandleRect *shr = nullptr;
        shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        m_handles.push_back(shr);
    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsCutItem::initPenAndBrush()
{
    QPen pen;
    pen.setColor(QColor(PenColorName));
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);

    setPen(pen);
    setBrush(Qt::NoBrush);
}

void CGraphicsCutItem::updateGeometry()
{
    const QRectF &geom = this->boundingRect();

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
    QRectF rect = this->rect();
    QRectF bounding = QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
                             rect.width() + pen().width(), rect.height() + pen().width());
    return bounding;
}

void CGraphicsCutItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    bool shiftKeyPress = false;//CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
    bool altKeyPress = false;//CDrawParamSigleton::GetInstance()->getAltKeyStatus();
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
            if (local.x() - rect.left() > 0.1 ) {
                rect.setRight(local.x());
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                rect.setTopRight(local);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomRight(local);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomLeft(local);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                rect.setBottom(local.y());
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                rect.setTopLeft(local);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                rect.setLeft(local.x());
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                rect.setTop(local.y());
            }
            break;
        default:
            break;
        }
    }
    //按住SHIFT等比拉伸
    else if ((shiftKeyPress && !altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                //变换后的宽度和高度
                qreal w2 = local.x() - rect.left();
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

                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
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
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
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
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                topLeft.setX(topRight.x() - w2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);

            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                //rect.setBottom(local.y());
                qreal h2 = local.y() - topLeft.y();
                qreal w2 = h2 * scale;
                topLeft.setX(topLeft.x() - (w2 - w) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                //rect.setTopLeft(local);
                QPointF bottomRight = rect.bottomRight();
                qreal w2 = bottomRight.x() - local.x();
                qreal h2 = bottomRight.y() - local.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                topLeft = bottomRight - QPointF(w2, h2);
                rect.setTopLeft(topLeft);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                //rect.setLeft(local.x());
                qreal w2 = rect.right() - local.x();
                qreal h2 = w2 / scale;
                topLeft.setX(local.x());
                topLeft.setY(topLeft.y() - (h2 - h) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                //rect.setTop(local.y());

                qreal h2 = rect.bottom() - local.y();
                qreal w2 = h2 * scale;
                topLeft.setX(topLeft.x() - (w2 - w) / 2);
                topLeft.setY(local.y());
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
    else if ((!shiftKeyPress && altKeyPress) ) {
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
    }
    //等比中心拉伸
    else if ((shiftKeyPress && altKeyPress) ) {
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
    }

    rect = rect.normalized();
    centerPoint = mapToScene(rect.center());
    rect.setRect(centerPoint.rx() - rect.width() * 0.5, centerPoint.ry() - rect.height() * 0.5, rect.width(), rect.height());
    prepareGeometryChange();
    setTransformOriginPoint(centerPoint);
    this->setPos(0, 0);

    this->setRect(rect);
    updateGeometry();
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
    showControlRects(isFreeMode);
}

void CGraphicsCutItem::duplicate(CGraphicsItem *item)
{

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
    return value;
}

void CGraphicsCutItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateGeometry();

    painter->setClipping(false);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(PenColorName));
    painter->setPen(pen);

    QPainterPath path;
    ///画4个角
    pen.setWidth(5);
    painter->setPen(pen);
    drawFourConner(painter, path, pen.width());

    QPainterPath path1;
    ///画三等分矩形的直线
//    pen.setColor(QColor(PenColorName));
    painter->setPen(pen);
    pen.setWidth(1);
    painter->setPen(pen);
    drawTrisectorRect(painter, path1);

    ///画矩形
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(boundingRect());

    painter->setClipping(true);
}

void CGraphicsCutItem::drawTrisectorRect(QPainter *painter, QPainterPath &path)
{
    path.moveTo(rect().x(), rect().y() + rect().height() / 3);
    path.lineTo(rect().x() + rect().width(), rect().y() + rect().height() / 3);

    path.moveTo(rect().x(), rect().y() + rect().height() / 3 * 2);
    path.lineTo(rect().x() + rect().width(), rect().y() + rect().height() / 3 * 2);

    path.moveTo(rect().x() + rect().width() / 3, rect().y() );
    path.lineTo(rect().x() + rect().width() / 3, rect().y() + rect().height() );

    path.moveTo(rect().x() + rect().width() / 3 * 2, rect().y() );
    path.lineTo(rect().x() + rect().width() / 3 * 2, rect().y() + rect().height() );

    painter->drawPath(path);
}

void CGraphicsCutItem::drawFourConner(QPainter *painter, QPainterPath &path, const int penWidth)
{
    //左上角
    path.moveTo(rect().x() + penWidth / 2, rect().y() + CORNER_WITH);
    path.lineTo(rect().x() + penWidth / 2, rect().y()  + penWidth / 2);
    path.lineTo(rect().x() + CORNER_WITH, rect().y()  + penWidth / 2);
    //右上角
    path.moveTo(rect().x() + rect().width() - CORNER_WITH, rect().y() + penWidth / 2);
    path.lineTo(rect().x() + rect().width() - penWidth / 2, rect().y()  + penWidth / 2);
    path.lineTo(rect().x() + rect().width() - penWidth / 2, rect().y() + CORNER_WITH);
    //右下角
    path.moveTo(rect().x() + rect().width() - penWidth / 2, rect().y() + rect().height() - CORNER_WITH);
    path.lineTo(rect().x() + rect().width() - penWidth / 2, rect().y()  + rect().height() - penWidth / 2);
    path.lineTo(rect().x() + rect().width() - CORNER_WITH, rect().y() + rect().height() - penWidth / 2);

    //左下角
    path.moveTo(rect().x() + CORNER_WITH + penWidth / 2, rect().y() + rect().height() - penWidth / 2);
    path.lineTo(rect().x() + penWidth / 2, rect().y()  + rect().height() - penWidth / 2);
    path.lineTo(rect().x() + penWidth / 2, rect().y() + rect().height() - CORNER_WITH);

    painter->drawPath(path);
}
