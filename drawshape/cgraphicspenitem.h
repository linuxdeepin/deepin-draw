#ifndef CGRAPHICSPENITEM_H
#define CGRAPHICSPENITEM_H

#include "cgraphicsrectitem.h"
#include "csizehandlerect.h"

#include <QGraphicsItem>

class CGraphicsPenItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsPenItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsPenItem() Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    void updatePenPath(const QPointF &endPoint, bool isShiftPress);
    EPenType currentType() const;
    void setCurrentType(const EPenType &currentType);
    /**
     * @brief changeToPixMap 当绘制路径完成后将路径绘制到图片里，以后此Item将只绘制图片
     */
    void changeToPixMap();
    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;

    void setPixMap(const QPixmap &pixMap);

    void setIsDrawCompelet(bool isDrawCompelet);

    void setStraightLine(const QLineF &straightLine);

    void setPrePoint(const QPointF &prePoint);

    void setArrow(const QPolygonF &arrow);

    void setTruePath(const QPainterPath &truePath);

    void setDrawPath(const QPainterPath &drawPath);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPainterPath m_drawPath;//画图的路径
    QPainterPath m_truePath;//如果为箭头时路径还要包含箭头的路径 否则和m_drawPath相等
    QPolygonF m_arrow; //箭头三角形
    QPointF m_prePoint; //前一个点
    QLineF m_straightLine;
    bool m_isShiftPress;//是否按住shift
    bool m_isDrawCompelet;//是否绘制完成

    EPenType m_currentType;

    QPixmap m_pixMap;

private:
    /**
     * @brief calcVertexes 计算箭头三角形的三个点
     * @param prePoint 前一个点
     * @param currentPoint 当前点
     */
    void calcVertexes(const QPointF &prePoint, const QPointF &currentPoint);

};

#endif // CGRAPHICSPENITEM_H
