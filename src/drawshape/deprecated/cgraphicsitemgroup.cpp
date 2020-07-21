#include "cgraphicsitemgroup.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QPainter>
CGraphicsItemGroup::CGraphicsItemGroup()
    : QGraphicsItemGroup ()
{

}

void CGraphicsItemGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->scenePos();
    foreach (QGraphicsItem *item, this->childItems()) {
        if (item->shape().contains(pos)) {
            this->removeFromGroup(item);
            break;
        }
    }

}

void CGraphicsItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (this->childItems().size() != 0) {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(this->boundingRect());
    }

    QGraphicsItemGroup::paint(painter, option, widget);
}
