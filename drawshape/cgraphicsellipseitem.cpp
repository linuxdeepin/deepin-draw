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

CGraphicsItem *CGraphicsEllipseItem::duplicate() const
{
    CGraphicsEllipseItem *item = new CGraphicsEllipseItem(rect());

    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
    return item;
}

void CGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawEllipse(rect());
    if (this->isSelected()) {
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(224, 224, 224));
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->rect());
    }
}
