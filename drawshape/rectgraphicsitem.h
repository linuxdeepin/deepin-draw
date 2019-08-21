#ifndef RECTGRAPHICSITEM_H
#define RECTGRAPHICSITEM_H
#include "basegraphicsitem.h"


class RectGraphicsItem : public BaseGraphicsItem
{
public:
    RectGraphicsItem(const QRect &rect, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void stretch(int handle, double sx, double sy, const QPointF &origin);
    QRectF  rect() const;
    void updateCoordinate();
    void move( const QPointF &point );
    QGraphicsItem *duplicate () const ;

protected:
    void updatehandles();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF m_initialRect;
    QPointF opposite_;
    QPointF m_originPoint;
};

#endif // RECTGRAPHICSITEM_H
