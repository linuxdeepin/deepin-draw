#ifndef CGRAPHICSLINEITEM_H
#define CGRAPHICSLINEITEM_H
#include "cgraphicsitem.h"
#include "csizehandlerect.h"
#include <QGraphicsItem>
#include <QLineF>
class CGraphicsLineItem : public CGraphicsItem
{
public:
    explicit CGraphicsLineItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsLineItem(const QLineF &line, QGraphicsItem *parent = nullptr);
    explicit CGraphicsLineItem(const QPointF &p1, const QPointF &p2, QGraphicsItem *parent = nullptr);
    explicit CGraphicsLineItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = nullptr);
    virtual ~CGraphicsLineItem() Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;

    QPainterPath shape() const Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QRectF rect() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress ) Q_DECL_OVERRIDE;

    QLineF line() const;
    void setLine(const QLineF &line);
    void setLine(const QPointF &p1, const QPointF &p2);
    inline void setLine(qreal x1, qreal y1, qreal x2, qreal y2);

    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;

protected:
    virtual void updateGeometry() Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;


private:
    void initLine();


private:
    QLineF m_line;
};

#endif // CGRAPHICSLINEITEM_H
