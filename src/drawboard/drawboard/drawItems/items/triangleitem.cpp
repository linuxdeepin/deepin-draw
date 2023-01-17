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
#include "triangleitem.h"
#include "pageview.h"
#include "attributewidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QtMath>

REGISTITEMCLASS(TriangleItem, TriangleType)
TriangleItem::TriangleItem(PageItem *parent)
    : RectBaseItem(parent)
{

}

TriangleItem::TriangleItem(const QRectF &rect, PageItem *parent)
    : RectBaseItem(rect, parent)
{

}

TriangleItem::TriangleItem(qreal x, qreal y, qreal w, qreal h, PageItem *parent)
    : RectBaseItem(x, y, w, h, parent)
{

}

SAttrisList TriangleItem::attributions()
{
    SAttrisList result;
    result <<  SAttri(EBrushColor, brush().color())
           <<  SAttri(EPenColor, pen().color())
           <<  SAttri(EBorderWidth,  pen().width());
    return result;
}

void TriangleItem::loadUnit(const Unit &data)
{
    TriangleUnitData i = data.data.data.value<TriangleUnitData>();
    loadGraphicsRectUnit(i.rect);
    loadVectorData(data.head);
    updateShape();
}

Unit TriangleItem::getUnit(int reson) const
{
    Q_UNUSED(reson)

    Unit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(TriangleUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    TriangleUnitData i;
    i.rect.topLeft = itemRect().topLeft();
    i.rect.bottomRight = itemRect().bottomRight();
    unit.data.data.setValue<TriangleUnitData>(i);

    return  unit;
}

QPainterPath TriangleItem::calOrgShapeBaseRect(const QRectF &rect) const
{
    QPolygonF ply;
    //calcPoints_helper(ply);
    ply.append(QPointF(rect.center().x(), rect.top()));
    ply.append(rect.bottomLeft());
    ply.append(rect.bottomRight());

    QPainterPath path;
    path.addPolygon(ply);
    path.closeSubpath();
    return path;
}

int TriangleItem::type() const
{
    return TriangleType;
}

void TriangleItem::calcPoints_helper(QVector<QPointF> &outVector, qreal offset) const
{
//    QRectF rc = rect();
//    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());
//    const QPen pen = this->paintPen();

//    outVector.clear();
//    //获取填充区域路径
//    qreal offsetWidth = offset;
//    QLineF line1(top, rc.bottomLeft());
//    QLineF line2(rc.bottomLeft(), rc.bottomRight());
//    QLineF line3(rc.bottomRight(), top);
//    QVector<QLineF> lines;
//    lines << line3 << line1 << line2;
//    for (int i = 0; i < lines.size(); ++i) {
//        QLineF ln1  = lines.at(i);
//        QLineF ln2  = (i == lines.size() - 1 ? lines[0] : lines[i + 1]);
//        qreal angle = 180 - ln1.angleTo(ln2);

//        qreal offsetLen = offsetWidth / qSin(qDegreesToRadians(angle / 2.0));
//        QLineF tempLine(ln2);
//        tempLine.setAngle(tempLine.angle() + angle / 2.0);
//        tempLine.setLength(offsetLen);

//        outVector.append(tempLine.p2());
//    }

    outVector.append(QPointF(rect().center().x(), rect().top()));
    outVector.append(rect().bottomLeft());
    outVector.append(rect().bottomRight());
}
