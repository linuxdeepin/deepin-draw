#ifndef CGRAPHICSPOLYGONITEM_H
#define CGRAPHICSPOLYGONITEM_H

#include "csizehandlerect.h"
#include "cgraphicsrectitem.h"
#include <QtGlobal>
#include <QVector>
#include <QRectF>

class CGraphicsPolygonItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPolygonItem(int count, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonItem(int count, const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsPolygonItem(int count, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    void setRect(const QRectF &rect);
    void setPointCount(int num);
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;


protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    void calcPoints(int n);

private:
    int m_nPointsCount; //点数
    QVector<QPointF> m_listPoints;
    //QPointF m_listPoints[10];
};

#endif // CGRAPHICSPOLYGONITEM_H
