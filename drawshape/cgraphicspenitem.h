#ifndef CGRAPHICSPENITEM_H
#define CGRAPHICSPENITEM_H

#include "cgraphicsitem.h"
#include "csizehandlerect.h"

#include <QGraphicsItem>

class CGraphicsPenItem : public CGraphicsItem
{
public:
    explicit CGraphicsPenItem(QGraphicsItem *parent = nullptr);
    explicit CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent = nullptr);

    virtual ~CGraphicsPenItem() Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;

    QPainterPath shape() const Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QRectF rect() const Q_DECL_OVERRIDE;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;

    void updatePenPath(const QPointF &endPoint, bool isShiftPress);

protected:
    virtual void updateGeometry() Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPainterPath m_drawPath;//画图的路径
    QPainterPath m_truePath;//如果为箭头时路径还要包含箭头的路径 否则和m_drawPath相等
    QPolygonF m_arrow; //箭头三角形
    QPointF m_prePoint; //前一个点
    QLineF m_straightLine;
    bool m_isShiftPress;
    QPainterPath m_tmpPath;

    EPenType m_currentType;

private:
    void initPen();
    /**
     * @brief calcVertexes 计算箭头三角形的三个点
     * @param prePoint 前一个点
     * @param currentPoint 当前点
     */
    void calcVertexes(const QPointF &prePoint, const QPointF &currentPoint);

};

#endif // CGRAPHICSPENITEM_H
