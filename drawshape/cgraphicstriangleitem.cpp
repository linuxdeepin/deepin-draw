/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include "cgraphicstriangleitem.h"

#include <QPainter>

#include <QDebug>

CGraphicsTriangleItem::CGraphicsTriangleItem(CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(const SGraphicsTriangleUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem (data->rect, head, parent)
{

}

CGraphicsUnit CGraphicsTriangleItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsTriangleUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pTriangle = new SGraphicsTriangleUnitData();
    unit.data.pTriangle->rect.topLeft = this->rect().topLeft();
    unit.data.pTriangle->rect.bottomRight = this->rect().bottomRight();

    return  unit;
}


QPainterPath CGraphicsTriangleItem::shape() const
{
    QPainterPath path;
    QRectF rc = rect();

    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());

    QPolygonF item;
    item << rc.bottomLeft() << top << rc.bottomRight();
    path.addPolygon(item);

    return path;
}

int CGraphicsTriangleItem::type() const
{
    return TriangleType;
}

CGraphicsItem *CGraphicsTriangleItem::duplicate() const
{
    CGraphicsTriangleItem *item = new CGraphicsTriangleItem(rect());

    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
    return item;
}

void CGraphicsTriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    updateGeometry();
    QRectF rc = rect();

    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());

    QPolygonF item;
    item << rc.bottomLeft() << top << rc.bottomRight();

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(item);

    if (this->isSelected()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(224, 224, 224));
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}
