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
#include "cgraphicspolygonalstaritem.h"

#include <QPainter>
#include <QtMath>


CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
    , m_anchorNum(anchorNum)
    , m_innerRadius(innerRadius)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
    , m_anchorNum(anchorNum)
    , m_innerRadius(innerRadius)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
    , m_anchorNum(anchorNum)
    , m_innerRadius(innerRadius)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(const SGraphicsPolygonStarUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem (data->rect, head, parent)
    , m_anchorNum(data->anchorNum)
    , m_innerRadius(data->radius)
{
    updatePolygonalStar(m_anchorNum, m_innerRadius);
}

int CGraphicsPolygonalStarItem::type() const
{
    return PolygonalStarType;
}

QPainterPath CGraphicsPolygonalStarItem::shape() const
{
    QPainterPath path;

    path.addPolygon(m_polygon);
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path, pen());
}

QRectF CGraphicsPolygonalStarItem::boundingRect() const
{
    QRectF rect = shape().controlPointRect();
    return rect.united(this->rect());
}

void CGraphicsPolygonalStarItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    CGraphicsRectItem::resizeTo(dir, point, bShiftPress, bAltPress);
    calcPolygon();
}

void CGraphicsPolygonalStarItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);
    static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(m_anchorNum, m_innerRadius);
}

CGraphicsUnit CGraphicsPolygonalStarItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPolygonStarUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pPolygonStar = new SGraphicsPolygonStarUnitData();
    unit.data.pPolygonStar->rect.topLeft = this->rect().topLeft();
    unit.data.pPolygonStar->rect.bottomRight = this->rect().bottomRight();
    unit.data.pPolygonStar->anchorNum = this->m_anchorNum;
    unit.data.pPolygonStar->radius = this->m_innerRadius;

    return  unit;
}

void CGraphicsPolygonalStarItem::setRect(const QRectF &rect)
{
    CGraphicsRectItem::setRect(rect);

    calcPolygon();
}

void CGraphicsPolygonalStarItem::updatePolygonalStar(int anchorNum, int innerRadius)
{
    m_anchorNum = anchorNum;
    m_innerRadius = innerRadius;

    calcPolygon();
}

void CGraphicsPolygonalStarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    updateGeometry();

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(m_polygon);

    if (this->isSelected()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CDrawParamSigleton::GetInstance()->getScale());
        pen.setColor(QColor(224, 224, 224));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->setPen(pen);
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}

void CGraphicsPolygonalStarItem::setPolygon(const QPolygonF &polygon)
{
    m_polygon = polygon;
}

int CGraphicsPolygonalStarItem::innerRadius() const
{
    return m_innerRadius;
}

int CGraphicsPolygonalStarItem::anchorNum() const
{
    return m_anchorNum;
}

void CGraphicsPolygonalStarItem::calcPolygon()
{
    prepareGeometryChange();
    m_polygon.clear();

    qreal angle = qDegreesToRadians(90.);
    QPointF pointCenter = this->rect().center();
    qreal outer_w = this->rect().width() / 2.;
    qreal outer_h = this->rect().height() / 2.;

    qreal inner_w = outer_w * m_innerRadius / 100.;
    qreal inner_h = outer_h * m_innerRadius / 100.;



    if (m_anchorNum > 0) {
        qreal preAngle = qDegreesToRadians(360. / m_anchorNum);
        qreal innerAngle = angle + preAngle / 2;
        for (int i = 0; i != m_anchorNum; i++) {

            qreal outer_Angle = angle + preAngle * i;
            qreal inner_Angle = innerAngle + preAngle * i;

            qreal outer_x = pointCenter.x() + outer_w * qCos(outer_Angle);
            qreal outer_y = pointCenter.y() - outer_h  * qSin(outer_Angle);

            m_polygon.push_back(QPointF(outer_x, outer_y));

            qreal inner_x = pointCenter.x() + inner_w * qCos(inner_Angle);
            qreal inner_y = pointCenter.y() - inner_h  * qSin(inner_Angle);

            m_polygon.push_back(QPointF(inner_x, inner_y));
        }
    }
}

//QPolygonF CGraphicsPolygonalStarItem::getPolygon(const QPointF &centerPoint, const qreal &radius) const
//{
//    QPolygonF polygon;

//    double angle = 360. / m_anchorNum;

//    QPointF out_Top(centerPoint.x(), centerPoint.y() - radius);
//    QPointF inner_First(centerPoint.x(), centerPoint.y() - radius * m_innerRadius / 100);

//    inner_First = rotationPoint(inner_First, centerPoint, angle / 2);

//    qreal tmpAngle = 0.;

//    for (int i = 0; i < m_anchorNum; i++) {

//        QPointF point_out = rotationPoint(out_Top, centerPoint, tmpAngle);
//        QPointF point_inner = rotationPoint(inner_First, centerPoint, tmpAngle);

//        polygon.push_back(point_out);
//        polygon.push_back(point_inner);

//        tmpAngle += angle;
//    }

//    return  polygon;
//}

//QPointF CGraphicsPolygonalStarItem::rotationPoint(const QPointF &beforPoint, const QPointF &centerPoint, double angle) const
//{
//    QPointF afterPoint;

//    double x = (beforPoint.x() - centerPoint.x()) * qCos(qDegreesToRadians(angle)) - (beforPoint.y() - centerPoint.y()) * qSin(qDegreesToRadians(angle)) + centerPoint.x() ;

//    double y = (beforPoint.x() - centerPoint.x()) * qSin(qDegreesToRadians(angle)) + (beforPoint.y() - centerPoint.y()) * qCos(qDegreesToRadians(angle)) +  centerPoint.y();

//    afterPoint.setX(x);
//    afterPoint.setY(y);

//    return afterPoint;
//}
