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
    explicit CGraphicsMasicoItem(const SGraphicsBlurUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    virtual int  type() const Q_DECL_OVERRIDE;
    void setPixmap();
    void setPixmap(const QPixmap &pixmap);
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) Q_DECL_OVERRIDE;
    void resizeToMul(CSizeHandleRect::EDirection dir,
                     const QPointF &offset,
                     const double &xScale, const double &yScale,
                     bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;
    void updateBlurPath();
    EBlurEffect getBlurEffect() const;
    void setBlurEffect(EBlurEffect effect);
    int getBlurWidth() const;
    void setBlurWidth(int width);
    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;
    virtual CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

    ELineType getPenStartType() const;
    void setPenStartType(const ELineType &penType);

    ELineType getPenEndType() const;
    void setPenEndType(const ELineType &penType);
private:
    QList<QGraphicsItem *> filterItems(QList<QGraphicsItem *> items);


protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
    QPainterPath m_blurPath;
    EBlurEffect m_nBlurEffect; //0是模糊  1是马赛克

    // 画笔类型
    ELineType m_penStartType; // 画笔起点样式
    ELineType m_penEndType; // 画笔终点样式
};

#endif // CGRAPHICSMASICOITEM_H
