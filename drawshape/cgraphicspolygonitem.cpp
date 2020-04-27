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
#include "drawshape/cgraphicspenitem.h"

#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QPainter>
#include <QPolygonF>

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
    , m_nPointsCount(count)
{

}

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
    , m_nPointsCount(count)
{

}

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
    , m_nPointsCount(count)
{

}

CGraphicsPolygonItem::CGraphicsPolygonItem(const SGraphicsPolygonUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem (data->rect, head, parent)
    , m_nPointsCount(data->pointNum)
{
    calcPoints(m_nPointsCount);
}

QPainterPath CGraphicsPolygonItem::shape() const
{
    QPainterPath path;
    path.addPolygon(m_listPoints);
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path, pen());
}

QRectF CGraphicsPolygonItem::boundingRect() const
{
    QRectF rect = shape().controlPointRect();
    return rect.united(this->rect());
}

int CGraphicsPolygonItem::type() const
{
    return PolygonType;
}

void CGraphicsPolygonItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);
    static_cast<CGraphicsPolygonItem *>(item)->setPointCount(m_nPointsCount);
}

CGraphicsUnit CGraphicsPolygonItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPolygonUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pPolygon = new SGraphicsPolygonUnitData();
    unit.data.pPolygon->rect.topLeft = this->rect().topLeft();
    unit.data.pPolygon->rect.bottomRight = this->rect().bottomRight();
    unit.data.pPolygon->pointNum = this->m_nPointsCount;

    return  unit;
}

void CGraphicsPolygonItem::setRect(const QRectF &rect)
{
    CGraphicsRectItem::setRect(rect);
    prepareGeometryChange();
    //更新坐标
    calcPoints(m_nPointsCount);
}

void CGraphicsPolygonItem::setPointCount(int num)
{
    m_nPointsCount = num;
    //重新计算
    calcPoints(m_nPointsCount);
}

void CGraphicsPolygonItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    CGraphicsRectItem::resizeTo(dir, point, bShiftPress, bAltPress);
    setPointCount(m_nPointsCount);
}

void CGraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (CGraphicsPenItem::s_curPenItem != nullptr)
        return;

    updateGeometry();
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(m_listPoints);

    if (this->isSelected()) {
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

void CGraphicsPolygonItem::calcPoints(int n)
{
    prepareGeometryChange();
    m_listPoints.clear();
    qreal angle = 90. * M_PI / 180.;
    QPointF pointCenter = this->rect().center();
    qreal w = this->rect().width();
    qreal h = this->rect().height();
    qreal R = w < h ? w : h;
    qreal r = R / 2;

    if (n > 0) {
        qreal preAngle = 360. / n * M_PI / 180.;
        for (int i = 0; i != n; i++) {
            ////程序坐标的Y轴和数学坐标中的Y轴是相反的
            //qreal x = pointCenter.x() + cos(angle + preAngle * i) * r;
            //qreal y = pointCenter.y() - sin(angle + preAngle * i) * r;
            //x = a cosθ y = b sinθ
            qreal x = pointCenter.x() + w / 2 * cos(angle + preAngle * i);
            qreal y = pointCenter.y() - h / 2 * sin(angle + preAngle * i);
            m_listPoints.push_back(QPointF(x, y));
        }
    }

}

void CGraphicsPolygonItem::setListPoints(const QVector<QPointF> &listPoints)
{
    m_listPoints = listPoints;
}

int CGraphicsPolygonItem::nPointsCount() const
{
    return m_nPointsCount;
}
