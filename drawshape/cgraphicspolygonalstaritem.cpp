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
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QPainter>
#include <QtMath>


CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
    , m_anchorNum(anchorNum)
    , m_innerRadius(innerRadius)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
    , m_anchorNum(anchorNum)
    , m_innerRadius(innerRadius)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
    , m_anchorNum(anchorNum)
    , m_innerRadius(innerRadius)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(const SGraphicsPolygonStarUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem (data->rect, head, parent)
    , m_anchorNum(data->anchorNum)
    , m_innerRadius(data->radius)
{
    updatePolygonalStar(m_anchorNum, m_innerRadius);
}

int CGraphicsPolygonalStarItem::type() const
{
    return PolygonalStarType;
}

QPainterPath CGraphicsPolygonalStarItem::shape() const
{
    QPainterPath path;

    path.addPolygon(m_polygonPen/*.boundingRect().intersected(rect())*/);
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path, pen());
}

QRectF CGraphicsPolygonalStarItem::boundingRect() const
{
    QRectF rect = shape().controlPointRect();
    return rect.united(this->rect());
}

void CGraphicsPolygonalStarItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    CGraphicsRectItem::resizeTo(dir, point, bShiftPress, bAltPress);
    calcPolygon();
}

void CGraphicsPolygonalStarItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);
    static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(m_anchorNum, m_innerRadius);
}

CGraphicsUnit CGraphicsPolygonalStarItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPolygonStarUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pPolygonStar = new SGraphicsPolygonStarUnitData();
    unit.data.pPolygonStar->rect.topLeft = this->rect().topLeft();
    unit.data.pPolygonStar->rect.bottomRight = this->rect().bottomRight();
    unit.data.pPolygonStar->anchorNum = this->m_anchorNum;
    unit.data.pPolygonStar->radius = this->m_innerRadius;

    return  unit;
}

QPainterPath CGraphicsPolygonalStarItem::getHighLightPath()
{
    QPainterPath path;
    path.addPolygon(/*m_polygonForBrush*/m_polygonPen);
    path.closeSubpath();
    return path;
}

void CGraphicsPolygonalStarItem::setRect(const QRectF &rect)
{
    CGraphicsRectItem::setRect(rect);

    calcPolygon();
}

void CGraphicsPolygonalStarItem::updatePolygonalStar(int anchorNum, int innerRadius)
{
    m_anchorNum = anchorNum;
    m_innerRadius = innerRadius;

    calcPolygon();
}

void CGraphicsPolygonalStarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    updateGeometry();
//先还原成原来的直接绘制所以这里先注释
//    painter->save();
//    painter->setClipRect(this->rect());
//    //绘制填充
//    painter->setPen(Qt::NoPen);
//    painter->setBrush(brush());
//    painter->drawPolygon(m_polygonForBrush);
//    //再绘制描边
//    if(m_renderWay == RenderPathLine)
//    {
//        painter->setPen(Qt::NoPen);
//        painter->setBrush(QColor(pen().color()));
//        painter->drawPath(m_pathForRenderPenLine);
//    }
//    else {
//        painter->setPen(pen().width() == 0 ? Qt::NoPen : pen());
//        painter->setBrush(Qt::NoBrush);
//        painter->drawPolygon(m_polygonPen);
//    }
//    painter->restore();

    painter->save();
    painter->setPen(pen().width() == 0 ? Qt::NoPen : pen());
    painter->setBrush(brush());
    painter->drawPolygon(m_polygonPen);
    painter->restore();

    if (this->getMutiSelect()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale());
        if ( CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            pen.setColor(QColor(224, 224, 224));
        } else {
            pen.setColor(QColor(69, 69, 69));
        }
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->setPen(pen);
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}

void CGraphicsPolygonalStarItem::setPolygon(const QPolygonF &polygon)
{
    m_polygonForBrush = polygon;
}

int CGraphicsPolygonalStarItem::innerRadius() const
{
    return m_innerRadius;
}

int CGraphicsPolygonalStarItem::anchorNum() const
{
    return m_anchorNum;
}

void CGraphicsPolygonalStarItem::calcPolygon()
{
    bool userSetNoPen = (qFuzzyIsNull(pen().widthF()));
    prepareGeometryChange();

    //如果用户设置为没有描边或者有描边但锚点个数不大于3那么都以PaintPolyLine的方式绘制边线
    //（锚点为3的时候已经非常特殊(就是一个三角型) 要使用类似CGraphicsPolygonItem的方式绘制三角形）
    m_renderWay = userSetNoPen?PaintPolyLine:RenderPathLine/*(m_anchorNum>3?RenderPathLine:PaintPolyLine)*/;

    //初始化线的路径
    m_pathForRenderPenLine = QPainterPath();

    if(m_renderWay == RenderPathLine)
    {
        calcPolygon_helper(m_polygonPen, m_anchorNum);

        //以渲染的方式绘制边线那么填充区域就要偏移整个线条的宽度
        calcPolygon_helper(m_polygonForBrush, m_anchorNum, -(pen().widthF()));

        for(int i= 0;i<m_polygonPen.size();++i)
        {
            if(i == 0)
            {
                m_pathForRenderPenLine.moveTo(m_polygonPen.at(i));
            }
            else {
                m_pathForRenderPenLine.lineTo(m_polygonPen.at(i));
            }
        }
        for(int i= 0;i<m_polygonForBrush.size();++i)
        {
            if(i == 0)
            {
                m_pathForRenderPenLine.moveTo(m_polygonForBrush.at(i));
            }
            else {
                m_pathForRenderPenLine.lineTo(m_polygonForBrush.at(i));
            }
        }
    }
    else
    {
        //如果分别绘制两个多边形(一个填充区域的多边形一个线条的多边形(这个多边形不设置填充色)) 因为Qt默认渲染线条和填充有重叠部分重叠部分为线宽的一半所以
        //这里采用这种方式时就只用偏移线宽一半
        CGraphicsPolygonItem::calcPoints_helper(m_polygonForBrush,m_anchorNum,this->rect(),-(pen().widthF()) / 2);

        CGraphicsPolygonItem::calcPoints_helper(m_polygonPen,m_anchorNum,this->rect());
    }

}

void CGraphicsPolygonalStarItem::calcPolygon_helper(QPolygonF &outPolygon, int n, qreal offset)
{
    if (n == 0)return;

    outPolygon.clear();

//    //根据垂直于边线的方向偏移offset得到外圆半径的偏移值finalOffset
    qreal angle = qDegreesToRadians(90.);
    QPointF pointCenter = this->rect().center();

    qreal outerEllipseXDiameter = this->rect().width();
    qreal outerEllipseYDiameter = this->rect().height();

    qreal outer_w = outerEllipseXDiameter / 2.0;
    qreal outer_h = outerEllipseYDiameter / 2.0;

    //根据外圆和内圆的关系计算内圆的半径
    qreal inner_w = outer_w  * m_innerRadius / 100.0;
    qreal inner_h = outer_h * m_innerRadius / 100.0;

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

    if(!qFuzzyIsNull(offset))
    {
        QList<QLineF> outlines;
        auto fGetLines = [=](const QPolygonF& outPolygon,QList<QLineF>& resultLines){
            resultLines.clear();
            for(int i = 0;i<outPolygon.size();++i)
            {
                if(i != 0)
                {
                    resultLines.append(QLineF(outPolygon.at(i-1),outPolygon.at(i)));
                }
            }
            resultLines.push_front(QLineF(outPolygon.last(),outPolygon.first()));
        };

        fGetLines(outPolygon,outlines);

        auto fGetOffsetPos = [=](const QList<QLineF>& lines,QVector<QPointF>& result){
            result.clear();
            for(int i = 0;i<lines.size();++i)
            {
                QLineF curLine  = lines[i];
                QLineF nextLine = (i==lines.size()-1?lines[0]: lines[i+1]);

                bool isInter = /*(i%2!=0)*/curLine.angleTo(nextLine) > 180;   //是否这个线条是和内圈点相关
                qreal inc    = (isInter?-1:1);//内圈点是相反的角度

                qreal finalDegree   =  (180 - curLine.angleTo(nextLine))*inc;   //两条线相交的交角*/
                qreal offLen = offset / qSin(qDegreesToRadians(finalDegree/2.));

                QLineF tempLine(nextLine);
                qreal newAngle = tempLine.angle() + finalDegree/2.0 + (isInter?(360 - curLine.angleTo(nextLine)):0);
                tempLine.setAngle(newAngle);
                qreal finallenth =  qAbs(offLen)/*>tempLine.length()?tempLine.length():qAbs(offLen)*/;
                tempLine.setLength(finallenth);
                result.append(tempLine.p2());
            }
        };

        QVector<QPointF> outecliPos;
        fGetOffsetPos(outlines,outecliPos);

        outPolygon.clear();
        for(int i =0;i<outecliPos.size();++i)
        {
            outPolygon.append(outecliPos[i]);
        }
    }
}
