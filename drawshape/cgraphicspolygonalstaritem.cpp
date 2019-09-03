#include "cgraphicspolygonalstaritem.h"

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

int CGraphicsPolygonalStarItem::type() const
{
    return PolygonalStarType;
}

QPainterPath CGraphicsPolygonalStarItem::shape() const
{
    QPainterPath path;

    path.addPolygon(m_polygon);

    return path;
}

void CGraphicsPolygonalStarItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    CGraphicsRectItem::resizeTo(dir, point);
    calcPolygon();
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


    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(m_polygon);
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
    prepareGeometryChange();
    m_polygon.clear();

    qreal angle = qDegreesToRadians(90.);
    QPointF pointCenter = this->rect().center();
    qreal outer_w = this->rect().width() / 2.;
    qreal outer_h = this->rect().height() / 2.;

    qreal inner_w = outer_w * m_innerRadius / 100.;
    qreal inner_h = outer_h * m_innerRadius / 100.;



    if (m_anchorNum > 0) {
        qreal preAngle = qDegreesToRadians(360. / m_anchorNum);
        qreal innerAngle = angle + preAngle / 2;
        for (int i = 0; i != m_anchorNum; i++) {

            qreal outer_Angle = angle + preAngle * i;
            qreal inner_Angle = innerAngle + preAngle * i;

            qreal outer_x = pointCenter.x() + outer_w * qCos(outer_Angle);
            qreal outer_y = pointCenter.y() - outer_h  * qSin(outer_Angle);

            m_polygon.push_back(QPointF(outer_x, outer_y));

            qreal inner_x = pointCenter.x() + inner_w * qCos(inner_Angle);
            qreal inner_y = pointCenter.y() - inner_h  * qSin(inner_Angle);

            m_polygon.push_back(QPointF(inner_x, inner_y));
        }
    }
}

//QPolygonF CGraphicsPolygonalStarItem::getPolygon(const QPointF &centerPoint, const qreal &radius) const
//{
//    QPolygonF polygon;

//    double angle = 360. / m_anchorNum;

//    QPointF out_Top(centerPoint.x(), centerPoint.y() - radius);
//    QPointF inner_First(centerPoint.x(), centerPoint.y() - radius * m_innerRadius / 100);

//    inner_First = rotationPoint(inner_First, centerPoint, angle / 2);

//    qreal tmpAngle = 0.;

//    for (int i = 0; i < m_anchorNum; i++) {

//        QPointF point_out = rotationPoint(out_Top, centerPoint, tmpAngle);
//        QPointF point_inner = rotationPoint(inner_First, centerPoint, tmpAngle);

//        polygon.push_back(point_out);
//        polygon.push_back(point_inner);

//        tmpAngle += angle;
//    }

//    return  polygon;
//}

//QPointF CGraphicsPolygonalStarItem::rotationPoint(const QPointF &beforPoint, const QPointF &centerPoint, double angle) const
//{
//    QPointF afterPoint;

//    double x = (beforPoint.x() - centerPoint.x()) * qCos(qDegreesToRadians(angle)) - (beforPoint.y() - centerPoint.y()) * qSin(qDegreesToRadians(angle)) + centerPoint.x() ;

//    double y = (beforPoint.x() - centerPoint.x()) * qSin(qDegreesToRadians(angle)) + (beforPoint.y() - centerPoint.y()) * qCos(qDegreesToRadians(angle)) +  centerPoint.y();

//    afterPoint.setX(x);
//    afterPoint.setY(y);

//    return afterPoint;
//}
