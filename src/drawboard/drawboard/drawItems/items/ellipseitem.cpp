/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "ellipseitem.h"
#include "pageview.h"

#include <QPainter>
#include <QPainterPath>

REGISTITEMCLASS(EllipseItem, EllipseType)
EllipseItem::EllipseItem(PageItem *parent)
    : RectBaseItem(parent)
{

}

EllipseItem::EllipseItem(qreal x, qreal y, qreal w, qreal h, PageItem *parent)
    : RectBaseItem(x, y, w, h, parent)
{

}

QPainterPath EllipseItem::calOrgShapeBaseRect(const QRectF &rect) const
{
    QPainterPath path;
    path.addEllipse(rect); //添加矩形的内椭圆
    path.closeSubpath();
    return path;
}

int EllipseItem::type() const
{
    return EllipseType;
}

SAttrisList EllipseItem::attributions()
{
    SAttrisList result;
    result <<  SAttri(EBrushColor, brush().color())
           <<  SAttri(EPenColor, pen().color())
           <<  SAttri(EPenWidth,  pen().width());
    //<<  SAttri(EBorderWidth,  pen().width());
    return result;
}

void EllipseItem::loadUnit(const Unit &data)
{
//    if (data.data.pCircle != nullptr)
//        loadGraphicsRectUnit(data.data.pCircle->rect);
    EllipseUnitData i = data.data.data.value<EllipseUnitData>();
    loadVectorData(data.head);
    loadGraphicsRectUnit(i.rect);



    //updateShape();
}

Unit EllipseItem::getUnit(int reson) const
{
    Unit unit;

    unit.usage = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(EllipseUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

//    unit.data.pCircle = new SGraphicsCircleUnitData();
//    unit.data.pCircle->rect.topLeft = this->rect().topLeft();
//    unit.data.pCircle->rect.bottomRight = this->rect().bottomRight();
    EllipseUnitData itemData;
    itemData.rect.topLeft = this->orgRect().topLeft();
    itemData.rect.bottomRight = this->orgRect().bottomRight();
    unit.data.data.setValue<EllipseUnitData>(itemData);

    return unit;
}
