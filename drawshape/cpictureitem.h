#ifndef CPICTUREITEM_H
#define CPICTUREITEM_H

#include <QGraphicsPixmapItem>
#include "cgraphicsrectitem.h"



class CPictureItem : public  CGraphicsRectItem
{
public:
    explicit CPictureItem(QPixmap pixmap, CGraphicsItem *parent = nullptr);
    explicit CPictureItem(const QRectF &rect, QPixmap pixmap, CGraphicsItem *parent = nullptr);
    //  explicit CPictureItem(const QPixmap &pixmap, QGraphicsPixmapItem *parent = nullptr);
    virtual ~CPictureItem()  Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    void setMirror(bool hor, bool ver);
    void setRotation90(bool leftOrRight);
    //virtual QPainterPath shape() const Q_DECL_OVERRIDE;

//    explicit CGraphicsEllipseItem(CGraphicsItem *parent = nullptr);
//    explicit CGraphicsEllipseItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
//    explicit CGraphicsEllipseItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
//    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
//    virtual int  type() const Q_DECL_OVERRIDE;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
    qreal m_angle;
};

#endif // CPICTUREITEM_H
