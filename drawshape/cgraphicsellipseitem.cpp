#include "cgraphicsellipseitem.h"
#include <QPainter>

CGraphicsEllipseItem::CGraphicsEllipseItem(CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
{

}

CGraphicsEllipseItem::CGraphicsEllipseItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
{

}

QPainterPath CGraphicsEllipseItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

int CGraphicsEllipseItem::type() const
{
    return EllipseType;
}

void CGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawEllipse(rect());
}
