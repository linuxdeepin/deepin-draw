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
    path.addPolygon(m_hightlightPath);
    path.closeSubpath();
    return path;
}

void CGraphicsTriangleItem::updateShape()
{
    calcPoints();
    CGraphicsRectItem::updateShape();
}

QPainterPath CGraphicsTriangleItem::getTrulyShape() const
{
    QPainterPath path;
    path.addPolygon(rect());
    path.closeSubpath();
    return path;
}

QPainterPath CGraphicsTriangleItem::getPenStrokerShape() const
{
    return m_pathForRenderPenLine;
}

QPainterPath CGraphicsTriangleItem::getShape() const
{
    QPainterPath path;
    path.addPolygon(polyForPen);
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
    painter->save();
    //绘制矩形框的极限值进行判断
    if (rect().width() > 1 && rect().height() > 1)
        painter->setBrush(brush());
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(polyForBrush);
    painter->restore();

    painter->save();
    painter->setBrush(pen().color());
    painter->setPen(Qt::NoPen);
    painter->setClipRect(rect(), Qt::IntersectClip);
    painter->drawPath(m_pathForRenderPenLine.simplified());
    painter->restore();
    endCheckIns(painter);

    paintMutBoundingLine(painter, option);

}

void CGraphicsTriangleItem::calcPoints()
{
    //获取填充路径
    calcPoints_helper(polyForBrush, (paintPen().widthF()));
    //获取高亮路径
    calcPoints_helper(m_hightlightPath, (paintPen().widthF() / 2));

    //获取描边区域路径
    polyForPen  = QPolygonF();
    QPointF top = QPointF((rect().x() + rect().width() / 2), rect().y());
    polyForPen << rect().bottomLeft() << top << rect().bottomRight();

    //获取轮廓路径
    m_pathForRenderPenLine = QPainterPath();
    for (int i = 0; i < polyForPen.size(); ++i) {
        if (i == 0) {
            m_pathForRenderPenLine.moveTo(polyForPen.at(i));
        } else {
            m_pathForRenderPenLine.lineTo(polyForPen.at(i));
        }
    }
    for (int i = 0; i < polyForBrush.size(); ++i) {
        if (i == 0) {
            m_pathForRenderPenLine.moveTo(polyForBrush.at(i));
        } else {
            m_pathForRenderPenLine.lineTo(polyForBrush.at(i));
        }
    }
}

void CGraphicsTriangleItem::calcPoints_helper(QVector<QPointF> &outVector, qreal offset)
{
    QRectF rc = rect();
    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());
    const QPen pen = this->paintPen();

    outVector.clear();
    //获取填充区域路径
    qreal offsetWidth = offset;
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

        outVector.append(tempLine.p2());
    }
}
