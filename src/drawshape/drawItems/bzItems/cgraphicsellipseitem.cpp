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
#include "cgraphicsellipseitem.h"
#include <QPainter>
#include "cgraphicsmasicoitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

CGraphicsEllipseItem::CGraphicsEllipseItem(CGraphicsItem *parent)
    : CGraphicsRectItem(parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem(rect, parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem(x, y, w, h, parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(const SGraphicsCircleUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem(data->rect, head, parent)
{

}

QPainterPath CGraphicsEllipseItem::inSideShape() const
{
    QPainterPath path;
    path.addEllipse(rect()); //添加矩形的内椭圆
    path.closeSubpath();
    return path;
}

QPainterPath CGraphicsEllipseItem::shape() const
{
    return qt_graphicsItem_shapeFromPath(inSideShape(), pen(), false, 5);
}

int CGraphicsEllipseItem::type() const
{
    return EllipseType;
}

CGraphicsItem *CGraphicsEllipseItem::duplicateCreatItem()
{
    return new CGraphicsEllipseItem;
}

void CGraphicsEllipseItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);
}

void CGraphicsEllipseItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pCircle != nullptr)
        loadGraphicsRectUnit(data.data.pCircle->rect);

    loadHeadData(data.head);
}

CGraphicsUnit CGraphicsEllipseItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsCircleUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();


    unit.data.pCircle = new SGraphicsCircleUnitData();
    unit.data.pCircle->rect.topLeft = this->rect().topLeft();
    unit.data.pCircle->rect.bottomRight = this->rect().bottomRight();

    return unit;
}

QPainterPath CGraphicsEllipseItem::getHighLightPath()
{
    QPainterPath path;
    path.addEllipse(rect());
    return path;
}

void CGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    beginCheckIns(painter);

    QPen curPen = this->pen();
    qreal penWidthOffset = curPen.widthF() / 2.0;
    QRectF rectIn = QRectF(rect().topLeft() + QPointF(penWidthOffset, penWidthOffset),
                           rect().size() - QSizeF(2 * penWidthOffset, 2 * penWidthOffset));

    painter->setPen(Qt::NoPen);
    painter->setBrush(brush());
    painter->drawEllipse(rectIn);

    painter->setPen(pen().width() == 0 ? Qt::NoPen : pen());
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(rect());

    endCheckIns(painter);

    paintMutBoundingLine(painter, option);
}
