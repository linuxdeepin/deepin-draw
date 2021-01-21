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
#include "cgraphicspolygonitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QDebug>

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, CGraphicsItem *parent)
    : CGraphicsRectItem(parent)
{
    setPointCount(count);
}

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem(rect, parent)
{
    setPointCount(count);
}

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem(x, y, w, h, parent)
{
    setPointCount(count);
}

int CGraphicsPolygonItem::type() const
{
    return PolygonType;
}

void CGraphicsPolygonItem::updateShape()
{
    calcPoints();
    CGraphicsRectItem::updateShape();
}

void CGraphicsPolygonItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pPolygon != nullptr) {
        loadGraphicsRectUnit(data.data.pPolygon->rect);
        m_nPointsCount[0] = data.data.pPolygon->pointNum;
        m_isPreviewPointCount = false;
    }

    updateShape();

    loadHeadData(data.head);
}

CGraphicsUnit CGraphicsPolygonItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;

    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPolygonUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = /*this->rotation()*/this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    unit.data.pPolygon = new SGraphicsPolygonUnitData();
    unit.data.pPolygon->rect.topLeft = this->rect().topLeft();
    unit.data.pPolygon->rect.bottomRight = this->rect().bottomRight();
    unit.data.pPolygon->pointNum = this->m_nPointsCount[0];

    return  unit;
}

void CGraphicsPolygonItem::setPointCount(int num, bool preview)
{
    bool changed = (m_isPreviewPointCount != preview || m_nPointsCount[m_isPreviewPointCount] != num);
    m_isPreviewPointCount = preview;

    m_nPointsCount[m_isPreviewPointCount] = num;

    if (changed)
        updateShapeRecursion();
}

void CGraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    beginCheckIns(painter);

#if 1
    painter->save();
    painter->setBrush(brush());
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(m_listPointsForBrush);
    painter->restore();

    painter->save();
    painter->setBrush(pen().color());
    painter->setPen(Qt::NoPen);
    painter->setClipRect(rect(), Qt::IntersectClip);
    painter->drawPath(m_pathForRenderPenLine.simplified());
    painter->restore();

#else

    painter->setPen(pen().width() == 0 ? Qt::NoPen : paintPen());
    painter->setBrush(paintBrush());
    painter->drawPolygon(m_listPoints);

#endif

    endCheckIns(painter);

    paintMutBoundingLine(painter, option);
}

void CGraphicsPolygonItem::calcPoints()
{
    prepareGeometryChange();
    calcPoints_helper(m_listPointsForBrush, nPointsCount(), this->rect(), -(paintPen().widthF()));
    calcPoints_helper(m_listPoints, nPointsCount(), this->rect());
    //获取高亮区域
    // m_hightlightPath
    calcPoints_helper(m_hightlightPath, nPointsCount(), this->rect(), -(paintPen().widthF() / 2));

    m_pathForRenderPenLine = QPainterPath();

    for (int i = 0; i < m_listPoints.size(); ++i) {
        if (i == 0) {
            m_pathForRenderPenLine.moveTo(m_listPoints.at(i));
        } else {
            m_pathForRenderPenLine.lineTo(m_listPoints.at(i));
        }
    }
    for (int i = 0; i < m_listPointsForBrush.size(); ++i) {
        if (i == 0) {
            m_pathForRenderPenLine.moveTo(m_listPointsForBrush.at(i));
        } else {
            m_pathForRenderPenLine.lineTo(m_listPointsForBrush.at(i));
        }
    }

}

void CGraphicsPolygonItem::calcPoints_helper(QVector<QPointF> &outVector, int n, const QRectF &rect, qreal offset)
{
    if (n <= 0)return;
    outVector.clear();

    QList<QLineF> lines;

    qreal angle = 90. * M_PI / 180.;
    QPointF pointCenter = rect.center();
    qreal w = rect.width();
    qreal h = rect.height();
    if (n) {
        qreal preAngle = 360. / n * M_PI / 180.;
        for (int i = 0; i != n; i++) {

            qreal curAngleDgree = angle + preAngle * i;
            qreal x = pointCenter.x() + w / 2 * cos(curAngleDgree);
            qreal y = pointCenter.y() - h / 2 * sin(curAngleDgree);

            outVector.push_back(QPointF(x, y));

            if (i != 0) {
                QLineF line(outVector.at(i - 1), outVector.at(i));
                lines.append(line);
            }
        }
    }
    lines.push_front(QLineF(outVector.last(), outVector.first()));

    if (!qFuzzyIsNull(offset)) {
        for (int i = 0; i < lines.size(); ++i) {
            QLineF curLine  = lines[i];
            QLineF nextLine = (i == lines.size() - 1 ? lines[0] : lines[i + 1]);

            qreal finalDegree   =  180 - curLine.angleTo(nextLine);   //两条线相交的交角*/

            qreal offLen = offset / qSin(qDegreesToRadians(finalDegree / 2.));

            QLineF tempLine(nextLine);
            qreal newAngle = tempLine.angle() + finalDegree / 2.0;
            tempLine.setAngle(newAngle);
            tempLine.setLength(qAbs(offLen));
            outVector[i] = tempLine.p2();
        }
    }
}

//void CGraphicsPolygonItem::setListPoints(const QVector<QPointF> &listPoints)
//{
//    m_listPoints = listPoints;
//}

QPainterPath CGraphicsPolygonItem::getSelfOrgShape() const
{
    QPainterPath path;
    path.addPolygon(m_hightlightPath);
    path.closeSubpath();
    return path;
}

QPainterPath CGraphicsPolygonItem::getTrulyShape() const
{
    QPainterPath path;
    path.addPolygon(rect());
    path.closeSubpath();
    return path;
}

QPainterPath CGraphicsPolygonItem::getPenStrokerShape() const
{
    return m_pathForRenderPenLine;
}

QPainterPath CGraphicsPolygonItem::getShape() const
{
    QPainterPath path;
    path.addPolygon(m_listPoints);
    path.closeSubpath();
    return path;
}

int CGraphicsPolygonItem::nPointsCount() const
{
    return m_nPointsCount[m_isPreviewPointCount];
}
