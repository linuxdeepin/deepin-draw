#ifndef CGRAPHICSITEMHIGHLIGHT_H
#define CGRAPHICSITEMHIGHLIGHT_H

#include "csizehandlerect.h"
#include "cgraphicsitem.h"
#include <QGraphicsItemGroup>
class CGraphicsItemHighLight : public QGraphicsPathItem
{
public:
    /**
     * @brief CGraphicsItemHighLight 构造函数
     */
    explicit CGraphicsItemHighLight(QGraphicsItem *parent = nullptr);

    /**
     * @brief shape 图元形状
     * @return
     */
    //virtual QPainterPath shape() const Q_DECL_OVERRIDE;

    /**
     * @brief type 返回图元类型
     * @return
     */
    virtual int  type() const Q_DECL_OVERRIDE;
    /**
     * @brief boundingRect 边界矩形
     * @return
     */
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

    void setPen(const QPen &pen);
protected:
    /**
     * @brief paint 绘制函数
     * @param painter
     * @param option
     * @param widget
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPainterPath m_path;
    QPen m_pen;
};
#endif // CGRAPHICSITEMHIGHLIGHT_H
