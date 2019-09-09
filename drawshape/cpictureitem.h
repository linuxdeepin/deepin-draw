#ifndef CPICTUREITEM_H
#define CPICTUREITEM_H

#include <QGraphicsPixmapItem>
#include "cgraphicsrectitem.h"



class CPictureItem : public  CGraphicsRectItem
{
public:
    explicit CPictureItem(const QPixmap &pixmap, CGraphicsItem *parent = nullptr);
    explicit CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent = nullptr);
    ~CPictureItem() Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    void setMirror(bool hor, bool ver);
    void setRotation90(bool leftOrRight);

    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;


    void setPixmap(const QPixmap &pixmap);

    void setAngle(const qreal &angle);


protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
    qreal m_angle;
};

#endif // CPICTUREITEM_H
