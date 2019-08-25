#include "cgraphicsrectitem.h"

CGraphicsRectItem::CGraphicsRectItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setProperty();
}

CGraphicsRectItem::CGraphicsRectItem(const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
{
    setProperty();
}

CGraphicsRectItem::CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    setProperty();
}

CGraphicsRectItem::~CGraphicsRectItem()
{

}

void CGraphicsRectItem::setProperty()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void CGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter, option, widget);
}
