#ifndef CGRAPHICSPOLYGONALSTARITEM_H
#define CGRAPHICSPOLYGONALSTARITEM_H


#include "cgraphicsrectitem.h"

class CGraphicsPolygonalStarItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPolygonalStarItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    int m_anchorNum;
    int m_innerRadius;
    QPointF rotationPoint(const QPointF &beforPoint, const QPointF &centerPoint, double angle) const;
    QPolygonF getPolygon(const QPointF &centerPoint, const qreal &radius) const;
};

#endif // CGRAPHICSPOLYGONALSTARITEM_H
