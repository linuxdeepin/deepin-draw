#ifndef CGRAPHICSITEMGROUP_H
#define CGRAPHICSITEMGROUP_H

#include "csizehandlerect.h"
#include "cgraphicsitem.h"
#include <QGraphicsItemGroup>

class CGraphicsPenItem;
class CGraphicsRotateAngleItem;
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
     * @brief updateBoundingRect 刷新大小矩形
     * @return
     */
    void updateBoundingRect();

    /**
     * @brief shape 图元形状
     * @return
     */
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;

    /**
     * @brief count 选中的业务图元个数
     * @return
     */
    int count();

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
     * @brief resizeTo 沿一个方向拉伸图元
     * @param dir 拉伸方向
     * @param point 移动距离
     */
    virtual void newResizeTo(CSizeHandleRect::EDirection dir,
                             const QPointF &mousePos,
                             const QPointF &offset,
                             bool bShiftPress, bool bAltPress) override;

    virtual void rotatAngle(qreal angle) override;

    /**
     * @brief resizeTo 拉伸图元 所有图元都通过此接口拉伸
     * @param dir
     * @param point
     */
    virtual void resizeTo(CSizeHandleRect::EDirection dir,
                          const QPointF &point) Q_DECL_OVERRIDE;

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
    int type() const Q_DECL_OVERRIDE;

    /**
     * @brief move  操作开始
     */
    void operatingBegin(int opTp) Q_DECL_OVERRIDE;

    /**
     * @brief move  操作结束
     */
    void operatingEnd(int opTp) Q_DECL_OVERRIDE;

    /**
     * @brief rect
     * @return
     */
    QRectF rect() const Q_DECL_OVERRIDE;

    CGraphicsUnit getGraphicsUnit(bool all) const override;

protected:
    /**
     * @brief paint 绘制函数
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */

    void initHandle() override;

    /**
     * @brief updateHandlesGeometry 刷新节点位置
     */
    void updateHandlesGeometry() Q_DECL_OVERRIDE;

    void updateAttributes();

private:
    QList<CGraphicsItem * > m_listItems;

    CGraphicsRotateAngleItem *rotateItem = nullptr;

    QRectF _rct;
};
#endif // CGRAPHICSITEMGROUP_H
