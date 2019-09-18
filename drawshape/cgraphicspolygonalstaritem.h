#ifndef CGRAPHICSPOLYGONALSTARITEM_H
#define CGRAPHICSPOLYGONALSTARITEM_H


#include "cgraphicsrectitem.h"

class CGraphicsPolygonalStarItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(int anchorNum, int innerRadius, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonalStarItem(const CGraphicsUnit &unit, CGraphicsItem *parent = nullptr);
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual  int type() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;
    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;
    void setRect(const QRectF &rect) Q_DECL_OVERRIDE;
    void updatePolygonalStar(int anchorNum, int innerRadius);
    int anchorNum() const;
    int innerRadius() const;

    void setPolygon(const QPolygonF &polygon);


    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    int m_anchorNum; //描点数
    int m_innerRadius; //内接圆半径
    QPolygonF m_polygon;

private:
    //QPointF rotationPoint(const QPointF &beforPoint, const QPointF &centerPoint, double angle) const;
    //QPolygonF getPolygon(const QPointF &centerPoint, const qreal &radius) const;
    void calcPolygon();
};

#endif // CGRAPHICSPOLYGONALSTARITEM_H
