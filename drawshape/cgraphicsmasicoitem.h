#ifndef CGRAPHICSMASICOITEM_H
#define CGRAPHICSMASICOITEM_H

#include "cgraphicsrectitem.h"
#include "cgraphicspenitem.h"

class CSizeHandleRect;

class CGraphicsMasicoItem : public CGraphicsPenItem
{
public:
    explicit CGraphicsMasicoItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsMasicoItem(const QPointF &startPoint, QGraphicsItem *parent = nullptr);
//    explicit CGraphicsMasicoItem(const CGraphicsUnit &unit, CGraphicsItem *parent = nullptr);
    virtual int  type() const Q_DECL_OVERRIDE;
    void setPixmap();
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;
    void updateBlurPath();
    EBlurEffect getBlurEffect() const;
    void setBlurEffect(EBlurEffect effect);
    int getBlurWidth() const;
    void setBlurWidth(int width);
    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

private:
    QList<QGraphicsItem *> filterItems(QList<QGraphicsItem *> items);


protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
    QPainterPath m_blurPath;
    EBlurEffect m_nBlurEffect; //0是模糊  1是马赛克
};

#endif // CGRAPHICSMASICOITEM_H
