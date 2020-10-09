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

    virtual int  type() const override;
    void updateMasicPixmap();
    void updateMasicPixmap(const QPixmap &pixmap);
    virtual QRectF boundingRect() const override;
    virtual QPainterPath getSelfOrgShape() const override;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) override;
    void resizeToMul(CSizeHandleRect::EDirection dir,
                     const QPointF &offset,
                     const double &xScale, const double &yScale,
                     bool bShiftPress, bool bAltPress) override;
    void updateBlurPath();
    EBlurEffect getBlurEffect() const;
    void setBlurEffect(EBlurEffect effect);
    int getBlurWidth() const;
    void setBlurWidth(int width);
    CGraphicsUnit getGraphicsUnit(bool all) const override;
    void loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo) override;
    CGraphicsItem *duplicateCreatItem() override;
    void duplicate(CGraphicsItem *item) override;

    ELineType getPenStartType() const;
    void setPenStartType(const ELineType &penType);

    ELineType getPenEndType() const;
    void setPenEndType(const ELineType &penType);
private:
    QList<QGraphicsItem *> filterItems(QList<QGraphicsItem *> items);


protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    virtual bool isPosPenetrable(const QPointF &posLocal) override;
private:
    QPixmap m_pixmap;
    QPainterPath m_blurPath;
    EBlurEffect m_nBlurEffect; //0是模糊  1是马赛克

    // 画笔类型
//    ELineType m_penStartType; // 画笔起点样式
//    ELineType m_penEndType; // 画笔终点样式
};

#endif // CGRAPHICSMASICOITEM_H
