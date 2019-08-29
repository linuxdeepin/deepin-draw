#include "cgraphicstriangleitem.h"

#include <QPainter>

#include <QDebug>

CGraphicsTriangleItem::CGraphicsTriangleItem(CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
{

}

CGraphicsTriangleItem::CGraphicsTriangleItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
{

}

QPainterPath CGraphicsTriangleItem::shape() const
{
    QPainterPath path;
    QRectF rc = rect();

    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());

    QPolygonF item;
    item << rc.bottomLeft() << top << rc.bottomRight();
    path.addPolygon(item);

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

    QRectF rc = rect();

    QPointF top = QPointF((rc.x() + rc.width() / 2), rc.y());

    QPolygonF item;
    item << rc.bottomLeft() << top << rc.bottomRight();

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(item);
}
