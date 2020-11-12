#ifndef CGRAPHICSITEMGROUP_H
#define CGRAPHICSITEMGROUP_H

#include "csizehandlerect.h"
#include "cgraphicsitem.h"
#include <QGraphicsItemGroup>

class CGraphicsPenItem;

/**
 * @brief The CGraphicsItemGroup class 组合图元管理类
 * 所有的图元操作都通过该类执行。
 */
class CGraphicsItemGroup : public CGraphicsItem
{
public:
    /**
     * @brief EGroupType 描述组合类型的枚举值
     * ENormalGroup 普通的组合; ESelectGroup 用于管理选择情况的组合; ERootForManage用于临时作为组合树的根组合(如场景内以树结构表示其组合情况);
     */
    enum EGroupType {ENormalGroup, ESelectGroup, ERootForManage, EGroupTypeCount};

    /**
     * @brief CGraphicsItemSelectedMgr 构造函数
     */
    explicit CGraphicsItemGroup(QGraphicsItem *parent = nullptr, EGroupType tp = ENormalGroup);

    /**
     * @brief type 描述组合的类型
     */
    EGroupType groupType();

    /**
     * @brief type 设置组合的类型
     */
    void  setGroupType(EGroupType tp);

    /**
     * @brief isTopBzGroup 是否是顶层的一个组合
     */
    bool  isTopBzGroup();

    /**
     * @brief type 设置组合是否可取消的
     */
    void setCancelable(bool enable);

    /**
     * @brief isCancelable 组合是否可取消的
     */
    bool isCancelable();

    /**
     * @brief add 添加图元到多选
     * @param item
     */
    void add(CGraphicsItem *item, bool updateAttri = true, bool updateRect = true);

    /**
     * @brief removeFromGroup 从多选列表中删除图元
     * @param item
     */
    void remove(CGraphicsItem *item, bool updateAttri = true, bool updateRect = true);

    /**
     * @brief clear 清除多选状态
     */
    void clear();

    /**
     * @brief updateAttributes 刷新属性显示
     */
    void updateAttributes();

    /**
     * @brief setHandleVisible 刷新属性显示
     */
    void setHandleVisible(bool visble, CSizeHandleRect::EDirection dirHandle = CSizeHandleRect::InRect);

    /**
     * @brief boundingRect 边界矩形
     * @return
     */
    QRectF boundingRect() const override;

    /**
     * @brief updateBoundingRect 刷新大小矩形
     * @return
     */
    void updateBoundingRect();

    /**
     * @brief shape 图元形状
     * @return
     */
    QPainterPath getSelfOrgShape() const override;

    /**
     * @brief incLength 虚拟的额外线宽宽度（解决选中困难的问题 提升用户体验）
     * @return 返回额外的线宽（一般与当前的放大值相关）
     */
    qreal incLength() const override;

    /**
     * @brief count 选中的业务图元个数
     * @return
     */
    int count();

    /**
     * @brief items 获取组合中的图元
     * @param recursiveFind 是否递归循环子组合找到所有的图元,为true表示递归返回所有图元,false返回子图元
     * @return 返回找到的图元
     */
    QList<CGraphicsItem *> items(bool recursiveFind = false) const;

    /**
     * @brief getItems 获取组合中的基本业务图元
     * @param recursiveFind 是否递归循环子组合找到所有的基本业务图元,为true表示递归返回所有的基本业务图元,false返回子基本业务图元
     * @return 返回找到的图元
     */
    QList<CGraphicsItem *> getBzItems(bool recursiveFind = false) const;

    /**
     * @brief getGroups 获取组合中的组合
     * @param recursiveFind 是否递归循环找到所有的组合,为true表示递归返回所有的组合,false只返回子组合
     * @return 返回找到的组合
     */
    QList<CGraphicsItemGroup *> getGroups(bool recursiveFind = false) const;

    /**
     * @brief resizeTo 沿一个方向拉伸图元
     * @param dir 拉伸方向
     * @param point 移动距离
     */
    void newResizeTo(CSizeHandleRect::EDirection dir,
                     const QPointF &mousePos,
                     const QPointF &offset,
                     bool bShiftPress, bool bAltPress) override;

    void rotatAngle(qreal angle) override;

    /**
     * @brief resizeTo 拉伸图元 所有图元都通过此接口拉伸
     * @param dir
     * @param point
     */
    void resizeTo(CSizeHandleRect::EDirection dir,
                  const QPointF &point) override;

    /**
     * @brief move  移动图元
     * @param beginPoint 移动起始点
     * @param movePoint 移动终点
     */
    void move(QPointF beginPoint, QPointF movePoint) override;

    /**
     * @brief type 返回图元类型
     * @return
     */
    int type() const override;

    /**
     * @brief move  操作开始
     */
    void operatingBegin(int opTp) override;

    /**
     * @brief move  操作结束
     */
    void operatingEnd(int opTp) override;

    /**
     * @brief rect
     * @return
     */
    QRectF rect() const override;

    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

    /**
     * @brief setNoContent  设置是否有内容
     */
    void setNoContent(bool b, bool children = true);

    /**
     * @brief isNoContent  判断是否有内容
     */
    bool isNoContent();

    /**
     * @brief containItem  是否包含图元
     */
    bool containItem(CGraphicsItem *pBzItem);

    /**
     * @brief nodes  获取操作缩放节点
     */
    Handles nodes();

protected:
    /**
     * @brief paint 绘制函数
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /**
     * @brief itemChange 图元变更
     * @param change 变更属性
     * @param value 变更的值
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    /**
     * @brief initRect 初始化矩形的属性和边框小方块
     */

    void initHandle() override;

    /**
     * @brief updateHandlesGeometry 刷新节点位置
     */
    void updateHandlesGeometry() override;

private:
    EGroupType _type = ENormalGroup;

    QList<CGraphicsItem * > m_listItems;

    QRectF _rct;

    bool  _isCancelable = true;

    int _indexForTest = 0;
};
#endif // CGRAPHICSITEMGROUP_H
