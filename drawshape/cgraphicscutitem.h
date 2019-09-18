#ifndef CGRAPHICSCUTITEM_H
#define CGRAPHICSCUTITEM_H

#include "cgraphicsitem.h"
#include "cbuttonrect.h"

class CSizeHandleRect;
class QGraphicsProxyWidget;

class CGraphicsCutItem : public CGraphicsItem
{

public:
    explicit CGraphicsCutItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsCutItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsCutItem()  Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    virtual void setRect(const QRectF &rect);
    virtual QRectF rect() const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress ) Q_DECL_OVERRIDE;


    CButtonRect::EButtonType collideTest(const QPointF &point) const;

    void doChangeType(int);
    void doChangeSize(int, int);


    bool isFreeMode() const;
    void setIsFreeMode(bool isFreeMode);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void updateGeometry() Q_DECL_OVERRIDE;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */
    void initRect();
    void initPenAndBrush();
    void drawFourConner(QPainter *painter, QPainterPath &path, const int penWidth);
    void drawTrisectorRect(QPainter *painter, QPainterPath &path);
    void showControlRects(bool);

private:
    QPointF m_topLeftPoint; //左上角的点
    QPointF m_bottomRightPoint; //右下角的点
    CButtonRect *m_okBtn;
    CButtonRect *m_cancelBtn;
    QRectF m_originalRect;
    bool m_isFreeMode;

};

#endif // CGRAPHICSCUTITEM_H
