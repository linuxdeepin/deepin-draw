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
#include <QPainterPath>
//#include "cgraphicsmasicoitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

CGraphicsEllipseItem::CGraphicsEllipseItem(CGraphicsItem *parent)
    : CGraphicsRectItem(parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem(x, y, w, h, parent)
{

}

QPainterPath CGraphicsEllipseItem::getSelfOrgShape() const
{
    QPainterPath path;
    path.addEllipse(rect()); //添加矩形的内椭圆
    path.closeSubpath();
    return path;
}

int CGraphicsEllipseItem::type() const
{
    return EllipseType;
}

void CGraphicsEllipseItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pCircle != nullptr)
        loadGraphicsRectUnit(data.data.pCircle->rect);

    loadHeadData(data.head);

    updateShape();
}

CGraphicsUnit CGraphicsEllipseItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;

    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsCircleUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    unit.data.pCircle = new SGraphicsCircleUnitData();
    unit.data.pCircle->rect.topLeft = this->rect().topLeft();
    unit.data.pCircle->rect.bottomRight = this->rect().bottomRight();

    return unit;
}
