#include "cgraphicspolygonalstaritem.h"

#include <QPainter>
#include <QtMath>


CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
    , m_anchorNum(5)
    , m_innerRadius(50)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
    , m_anchorNum(5)
    , m_innerRadius(50)
{

}

CGraphicsPolygonalStarItem::CGraphicsPolygonalStarItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
    , m_anchorNum(5)
    , m_innerRadius(50)
{

}

QPainterPath CGraphicsPolygonalStarItem::shape() const
{
    QPainterPath path;

    QRectF rc = rect();
    QPointF centerPoint = rc.center();
    qreal radius = rc.width() < rc.height() ? rc.width() : rc.height();
    QPolygonF polygon = getPolygon(centerPoint, radius);

    path.addPolygon(polygon);

    return path;
}

void CGraphicsPolygonalStarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRectF rc = rect();

    QPointF centerPoint = rc.center();

    qreal radius = rc.width() < rc.height() ? rc.width() / 2 : rc.height() / 2;

    QPolygonF polygon = getPolygon(centerPoint, radius);


    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(polygon);
}

QPolygonF CGraphicsPolygonalStarItem::getPolygon(const QPointF &centerPoint, const qreal &radius) const
{
    QPolygonF polygon;

    double angle = 360. / m_anchorNum;

    QPointF out_Top(centerPoint.x(), centerPoint.y() - radius);
    QPointF inner_First(centerPoint.x(), centerPoint.y() - radius * m_innerRadius / 100);

    inner_First = rotationPoint(inner_First, centerPoint, angle / 2);

    qreal tmpAngle = 0.;

    for (int i = 0; i < m_anchorNum; i++) {

        QPointF point_out = rotationPoint(out_Top, centerPoint, tmpAngle);
        QPointF point_inner = rotationPoint(inner_First, centerPoint, tmpAngle);

        polygon.push_back(point_out);
        polygon.push_back(point_inner);

        tmpAngle += angle;
    }

    return  polygon;
}

QPointF CGraphicsPolygonalStarItem::rotationPoint(const QPointF &beforPoint, const QPointF &centerPoint, double angle) const
{
    QPointF afterPoint;

    double x = (beforPoint.x() - centerPoint.x()) * qCos(qDegreesToRadians(angle)) - (beforPoint.y() - centerPoint.y()) * qSin(qDegreesToRadians(angle)) + centerPoint.x() ;

    double y = (beforPoint.x() - centerPoint.x()) * qSin(qDegreesToRadians(angle)) + (beforPoint.y() - centerPoint.y()) * qCos(qDegreesToRadians(angle)) +  centerPoint.y();

    afterPoint.setX(x);
    afterPoint.setY(y);

    return afterPoint;
}
