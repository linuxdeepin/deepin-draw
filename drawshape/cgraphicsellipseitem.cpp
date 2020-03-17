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
    : CGraphicsRectItem (parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(const SGraphicsCircleUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem (data->rect, head, parent)
{

}

QPainterPath CGraphicsEllipseItem::shape() const
{
    QPainterPath path;
    path.addEllipse(rect());
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path, pen());
}

int CGraphicsEllipseItem::type() const
{
    return EllipseType;
}

void CGraphicsEllipseItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);
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

void CGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateGeometry();
    painter->setPen(pen().width() == 0 ? Qt::NoPen : pen());
    painter->setBrush(brush());
    painter->drawEllipse(rect());
//    painter->drawRect(rect());
    if (this->getMutiSelect()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale());
        if ( CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            pen.setColor(QColor(224, 224, 224));
        } else {
            pen.setColor(QColor(69, 69, 69));
        }
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}
