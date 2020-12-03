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
#include "cgraphicstriangleitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QtMath>

CGraphicsTriangleItem::CGraphicsTriangleItem(CGraphicsItem *parent)
    : CGraphicsRectItem(parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem(rect, parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem(x, y, w, h, parent)
{

}

void CGraphicsTriangleItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pTriangle != nullptr) {
        loadGraphicsRectUnit(data.data.pTriangle->rect);
    }
    loadHeadData(data.head);
    updateShape();
}

CGraphicsUnit CGraphicsTriangleItem::getGraphicsUnit(EDataReason reson) const
{
    Q_UNUSED(reson)

    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsTriangleUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = /*this->rotation()*/this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    unit.data.pTriangle = new SGraphicsTriangleUnitData();
    unit.data.pTriangle->rect.topLeft = this->rect().topLeft();
    unit.data.pTriangle->rect.bottomRight = this->rect().bottomRight();

    return  unit;
}

QPainterPath CGraphicsTriangleItem::getSelfOrgShape() const
{
    QPainterPath path;
    QRectF rc = rect();

    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());

    QPolygonF item;
    item << rc.bottomLeft() << top << rc.bottomRight();
    path.addPolygon(item);
    path.closeSubpath();

    return path;
}

int CGraphicsTriangleItem::type() const
{
    return TriangleType;
}

void CGraphicsTriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    beginCheckIns(painter);

    QRectF rc = rect();

    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());

    const QPen pen = this->paintPen();
    //先绘制填充区域
    QPolygonF polyForBrush;
    qreal offsetWidth = pen.widthF() / 2.0;
    QLineF line1(top, rc.bottomLeft());
    QLineF line2(rc.bottomLeft(), rc.bottomRight());
    QLineF line3(rc.bottomRight(), top);
    QVector<QLineF> lines;
    lines << line3 << line1 << line2;
    for (int i = 0; i < lines.size(); ++i) {
        QLineF ln1  = lines.at(i);
        QLineF ln2  = (i == lines.size() - 1 ? lines[0] : lines[i + 1]);
        qreal angle = 180 - ln1.angleTo(ln2);

        qreal offsetLen = offsetWidth / qSin(qDegreesToRadians(angle / 2.0));
        QLineF tempLine(ln2);
        tempLine.setAngle(tempLine.angle() + angle / 2.0);
        tempLine.setLength(offsetLen);

        polyForBrush.append(tempLine.p2());
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(paintBrush());

    painter->save();
    painter->setClipRect(rect(), Qt::IntersectClip);
    painter->drawPolygon(polyForBrush);
    painter->restore();



    //再绘制描边
    QPolygonF polyForPen;
    polyForPen << rc.bottomLeft() << top << rc.bottomRight();

    painter->setPen(pen.width() == 0 ? Qt::NoPen : pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(polyForPen);

    endCheckIns(painter);

    paintMutBoundingLine(painter, option);
}
