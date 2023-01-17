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
#include "polygonitem.h"
#include "pageview.h"
#include "polygontool.h"

#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QDebug>

REGISTITEMCLASS(PolygonItem, PolygonType)
PolygonItem::PolygonItem(int count, PageItem *parent)
    : RectBaseItem(parent)
{
    setPointCount(count);
}

PolygonItem::PolygonItem(int count, const QRectF &rect, PageItem *parent)
    : RectBaseItem(rect, parent)
{
    setPointCount(count);
}

PolygonItem::PolygonItem(int count, qreal x, qreal y, qreal w, qreal h, PageItem *parent)
    : RectBaseItem(x, y, w, h, parent)
{
    setPointCount(count);
}

SAttrisList PolygonItem::attributions()
{
    SAttrisList result;
    result <<  SAttri(PolygonTool::EPolygonLineSep)
           <<  SAttri(EPolygonSides,  nPointsCount());
    return result.insected(RectBaseItem::attributions());
}

void PolygonItem::setAttributionVar(int attri, const QVariant &var, int phase)
{
    if (EPolygonSides == attri) {
        bool isPreview = (phase == EChangedBegin || phase == EChangedUpdate);
        setPointCount(var.toInt(), isPreview);
        return;
    }
    RectBaseItem::setAttributionVar(attri, var, phase);
}

int PolygonItem::type() const
{
    return PolygonType;
}

void PolygonItem::loadUnit(const Unit &data)
{
//    if (data.data.pPolygon != nullptr) {
//        loadGraphicsRectUnit(data.data.pPolygon->rect);
//        m_nPointsCount[0] = data.data.pPolygon->pointNum;
//        m_isPreviewPointCount = false;
//    }
    PolygonUnitData i = data.data.data.value<PolygonUnitData>();
    loadGraphicsRectUnit(i.rect);
    m_nPointsCount[0] = i.pointNum;
    m_isPreviewPointCount = false;

    updateShape();

    loadHeadData(data.head);
}

Unit PolygonItem::getUnit(int reson) const
{
    Unit unit;

    unit.usage = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(PolygonUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = /*this->rotation()*/this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

//    unit.data.pPolygon = new SGraphicsPolygonUnitData();
//    unit.data.pPolygon->rect.topLeft = this->rect().topLeft();
//    unit.data.pPolygon->rect.bottomRight = this->rect().bottomRight();
//    unit.data.pPolygon->pointNum = this->m_nPointsCount[0];
    PolygonUnitData i;
    i.rect.topLeft = itemRect().topLeft();
    i.rect.bottomRight = itemRect().bottomRight();
    i.pointNum = m_nPointsCount[0];
    unit.data.data.setValue<PolygonUnitData>(i);

    return  unit;
}

void PolygonItem::setPointCount(int num, bool preview)
{
    //bool changed = (m_isPreviewPointCount != preview || m_nPointsCount[m_isPreviewPointCount] != num);
    m_isPreviewPointCount = preview;
    m_nPointsCount[m_isPreviewPointCount] = num;
}

void PolygonItem::calcPoints_helper(QVector<QPointF> &outVector, int n, const QRectF &rect, qreal offset)
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


QPainterPath PolygonItem::calOrgShapeBaseRect(const QRectF &rect) const
{
    QPolygonF ply;
    calcPoints_helper(ply, nPointsCount(), rect);

    QPainterPath path;
    path.addPolygon(ply);
    path.closeSubpath();
    return path;
}

int PolygonItem::nPointsCount() const
{
    return m_nPointsCount[m_isPreviewPointCount];
}
