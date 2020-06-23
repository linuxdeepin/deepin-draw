#ifndef CGRAPHICSITEMGROUP_H
#define CGRAPHICSITEMGROUP_H

#include "csizehandlerect.h"
#include "cgraphicsitem.h"
#include <QGraphicsItemGroup>
/**
 * @brief The CGraphicsItemSelectedMgr class 选中图元管理类
 * 所有的图元操作都通过该类执行。
 */
class CGraphicsItemSelectedMgr : public CGraphicsItem
{
public:
    /**
     * @brief CGraphicsItemSelectedMgr 构造函数
     */
    CGraphicsItemSelectedMgr(QGraphicsItem *parent = nullptr);

    /**
     * @brief addOrRemoveToGroup 点击一个图元
     * @param item
     */
    void addOrRemoveToGroup(CGraphicsItem *item);

    /**
     * @brief clear 清除多选状态
     */
    void clear();

    /**
     * @brief boundingRect 边界矩形
     * @return
     */
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

    /**
     * @brief shape 图元形状
     * @return
     */
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;

    /**
     * @brief getItems 获取多选的节点
     * @return
     */
    QList<CGraphicsItem *> getItems() const;

    /**
     * @brief addToGroup 添加图元到多选
     * @param item
     */
    void addToGroup(CGraphicsItem *item);

    /**
     * @brief removeFromGroup 从多选列表中删除图元
     * @param item
     */
    void removeFromGroup(CGraphicsItem *item);

    /**
     * @brief resizeTo 拉伸图元 所有图元都通过此接口拉伸
     * @param dir
     * @param point
     */
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) Q_DECL_OVERRIDE;

    /**
     * @brief resizeTo
     * @param dir
     * @param point
     * @param bShiftPress
     * @param bAltPress
     */
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point,
                          bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;

    void resizeAllTo(CSizeHandleRect::EDirection dir, const QPointF &mousePos,
                     const QPointF &offset, bool bShiftPress, bool bAltPress);

    /**
     * @brief move  移动图元
     * @param beginPoint 移动起始点
     * @param movePoint 移动终点
     */
    virtual void move(QPointF beginPoint, QPointF movePoint) Q_DECL_OVERRIDE;

    /**
     * @brief type 返回图元类型
     * @return
     */
    virtual int  type() const Q_DECL_OVERRIDE;

    /**
     * @brief rect
     * @return
     */
    virtual QRectF rect() const Q_DECL_OVERRIDE;

    /**
     * @brief updateGeometry
     */
    virtual void updateGeometry() Q_DECL_OVERRIDE;

protected:
    /**
     * @brief paint 绘制函数
     * @param painter
     * @param option
     * @param widget
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */
    void initRect();


private:
    QList<CGraphicsItem * > m_listItems;
};
#endif // CGRAPHICSITEMGROUP_H
