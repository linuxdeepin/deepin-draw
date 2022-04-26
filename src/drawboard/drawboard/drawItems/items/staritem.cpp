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
#include "staritem.h"
#include "pageview.h"
#include "cgraphicsitemevent.h"
#include "attributewidget.h"
#include "startool.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

REGISTITEMCLASS(StarItem, PolygonalStarType)
StarItem::StarItem(int anchorNum, int innerRadius, PageItem *parent)
    : RectBaseItem(parent)
{
    setAnchorNum(anchorNum);
    setInnerRadius(innerRadius);
}

StarItem::StarItem(int anchorNum, int innerRadius, const QRectF &rect, PageItem *parent)
    : RectBaseItem(rect, parent)
{
    setAnchorNum(anchorNum);
    setInnerRadius(innerRadius);
}

StarItem::StarItem(int anchorNum, int innerRadius, qreal x, qreal y, qreal w, qreal h, PageItem *parent)
    : RectBaseItem(x, y, w, h, parent)
{
    setAnchorNum(anchorNum);
    setInnerRadius(innerRadius);
}

SAttrisList StarItem::attributions()
{
    SAttrisList result;
    result <<  SAttri(EBrushColor, brush().color())
           <<  SAttri(EPenColor, pen().color())
           <<  SAttri(EBorderWidth,  pen().width())
           <<  SAttri(StarTool::EStartLineSep)
           <<  SAttri(EStarAnchor,  anchorNum())
           <<  SAttri(EStarInnerOuterRadio,  innerRadius());
    return result;
}

void StarItem::setAttributionVar(int attri, const QVariant &var, int phase)
{
    bool isPreview = (phase == EChangedBegin || phase == EChangedUpdate);
    if (EStarAnchor == attri) {
        setAnchorNum(var.toInt(), isPreview);
        updateViewport();
    } else if (EStarInnerOuterRadio == attri) {
        setInnerRadius(var.toInt(), isPreview);
    } else {
        RectBaseItem::setAttributionVar(attri, var, phase);
    }
}

int StarItem::type() const
{
    return PolygonalStarType;
}

Unit StarItem::getUnit(int reson) const
{
    Unit unit;

    unit.usage = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(StarUnit);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

//    unit.data.pPolygonStar = new SGraphicsPolygonStarUnitData();
//    unit.data.pPolygonStar->rect.topLeft = this->rect().topLeft();
//    unit.data.pPolygonStar->rect.bottomRight = this->rect().bottomRight();
//    unit.data.pPolygonStar->anchorNum = this->m_anchorNum[0];
//    unit.data.pPolygonStar->radius = this->m_innerRadius[0];
    StarUnit i;
    i.rect.topLeft = rect().topLeft();
    i.rect.bottomRight = rect().bottomRight();
    i.anchorNum = m_anchorNum[0];
    i.radius = m_innerRadius[0];
    unit.data.data.setValue<StarUnit>(i);

    return  unit;
}

QPainterPath StarItem::calOrgShapeBaseRect(const QRectF &rect) const
{
    QPolygonF ply;
    calcPolygon_helper(rect, ply, anchorNum());

    QPainterPath path;
    path.addPolygon(ply);
    path.closeSubpath();
    return path;
}

//QPainterPath StarItem::getShape() const
//{
//    QPolygonF ply;
//    calcPolygon_helper(ply, anchorNum(), paintPen().widthF() / 2);

//    QPainterPath path;
//    path.addPolygon(ply);
//    path.closeSubpath();
//    return path;
//}

//QPainterPath StarItem::getTrulyShape() const
//{
//    QPainterPath path;

//    QPolygonF plyOuter;
//    calcPolygon_helper(plyOuter, anchorNum(), paintPen().widthF() / 2);
//    path.addPolygon(plyOuter);
//    return path;
//}

void StarItem::setAnchorNum(int num, bool preview)
{
    m_preview[0] = preview;
    m_anchorNum[preview] = num;
    updateShape();
}

void StarItem::loadUnit(const Unit &data)
{
    StarUnit i = data.data.data.value<StarUnit>();
    loadGraphicsRectUnit(i.rect);

    setAnchorNum(i.anchorNum);
    setInnerRadius(i.radius);

    loadVectorData(data.head);
}

int StarItem::innerRadius() const
{
    return m_innerRadius[m_preview[1]];
}

int StarItem::anchorNum() const
{
    return m_anchorNum[m_preview[0]];
}

void StarItem::setInnerRadius(int radius, bool preview)
{
    //bool changed = (m_preview[1] != preview || m_innerRadius[preview] != radius);
    m_preview[1] = preview;
    m_innerRadius[preview] = radius;
}

void StarItem::calcPolygon_helper(const QRectF &baseRect, QPolygonF &outPolygon, int n, qreal offset) const
{
    if (n == 0)return;

    outPolygon.clear();

//    //根据垂直于边线的方向偏移offset得到外圆半径的偏移值finalOffset
    qreal angle = qDegreesToRadians(90.);
    QPointF pointCenter = baseRect.center();

    qreal outerEllipseXDiameter = baseRect.width();
    qreal outerEllipseYDiameter = baseRect.height();

    qreal outer_w = outerEllipseXDiameter / 2.0;
    qreal outer_h = outerEllipseYDiameter / 2.0;

    //qWarning() << "innerRadius() = " << innerRadius();

    //根据外圆和内圆的关系计算内圆的半径
    qreal inner_w = outer_w * innerRadius() / 100.0;
    qreal inner_h = outer_h * innerRadius() / 100.0;

    if (n > 0) {
        qreal preAngle = qDegreesToRadians(360. / n);
        qreal innerAngle = angle + preAngle / 2;
        for (int i = 0; i < n; i++) {

            qreal outer_Angle = angle + preAngle * i;
            qreal inner_Angle = innerAngle + preAngle * i;

            qreal outer_x = pointCenter.x() + outer_w * qCos(outer_Angle);
            qreal outer_y = pointCenter.y() - outer_h  * qSin(outer_Angle);


            //偶数是外圈点
            outPolygon.push_back(QPointF(outer_x, outer_y));

            qreal inner_x = pointCenter.x() + inner_w * qCos(inner_Angle);
            qreal inner_y = pointCenter.y() - inner_h  * qSin(inner_Angle);

            //奇数是内圈点
            outPolygon.push_back(QPointF(inner_x, inner_y));

        }
    }

    if (!qFuzzyIsNull(offset)) {
        QList<QLineF> outlines;
        auto fGetLines = [ = ](const QPolygonF & outPolygon, QList<QLineF> &resultLines) {
            resultLines.clear();
            for (int i = 0; i < outPolygon.size(); ++i) {
                if (i != 0) {
                    resultLines.append(QLineF(outPolygon.at(i - 1), outPolygon.at(i)));
                }
            }
            resultLines.push_front(QLineF(outPolygon.last(), outPolygon.first()));
        };

        fGetLines(outPolygon, outlines);

        auto fGetOffsetPos = [ = ](const QList<QLineF> &lines, QVector<QPointF> &result) {
            result.clear();
            for (int i = 0; i < lines.size(); ++i) {
                QLineF curLine  = lines[i];
                QLineF nextLine = (i == lines.size() - 1 ? lines[0] : lines[i + 1]);

                bool isInter = /*(i%2!=0)*/curLine.angleTo(nextLine) > 180;   //是否这个线条是和内圈点相关
                qreal inc    = (isInter ? -1 : 1); //内圈点是相反的角度

                qreal finalDegree = (180 - curLine.angleTo(nextLine)) * inc; //两条线相交的交角*/

                qreal sinValue = qSin(qDegreesToRadians(finalDegree / 2.));

                qreal offLen = qFuzzyIsNull(sinValue) ? 0.0 : offset / sinValue;

                QLineF tempLine(nextLine);
                qreal newAngle = tempLine.angle() + finalDegree / 2.0 + (isInter ? (360 - curLine.angleTo(nextLine)) : 0) + (offset > 0 ? 180 : 0);
                tempLine.setAngle(newAngle);
                qreal finallenth =  qAbs(offLen);

                if (finallenth > nextLine.length()) {
                    finallenth = nextLine.length();
                }
                tempLine.setLength(finallenth);
                result.append(tempLine.p2());
            }
        };

        QVector<QPointF> outecliPos;

        fGetOffsetPos(outlines, outecliPos);

        outPolygon.clear();
        for (int i = 0; i < outecliPos.size(); ++i) {
            outPolygon.append(outecliPos[i]);
        }
    }
}
