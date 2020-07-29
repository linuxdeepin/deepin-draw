/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
    CGraphicsLineItem(const SGraphicsLineUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    virtual ~CGraphicsLineItem() Q_DECL_OVERRIDE;

    /**
     * @brief rect 基于一个矩形范围的图元，重写实现该图元的矩形范围
     */
    QRectF rect() const Q_DECL_OVERRIDE;

    /**
     * @brief boundingRect 包围矩形
     */
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    /**
     * @brief shape 图元形状
     */
    QPainterPath shape() const Q_DECL_OVERRIDE;

    /**
     * @brief type 图元类型
     */
    int type() const Q_DECL_OVERRIDE;

    /**
     * @brief rotatAngle 重写以实现线条图元旋转的逻辑
     */
    void rotatAngle(qreal angle) Q_DECL_OVERRIDE;

    /**
     * @brief rotatAngle 重写以实现线条图元的resize逻辑
     */
    void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) Q_DECL_OVERRIDE;

    /**
     * @brief rotatAngle 重写以实现线条图元的resize逻辑
     */
    void resizeTo(CSizeHandleRect::EDirection dir,
                  const QPointF &point,
                  bool bShiftPress, bool bAltPress) Q_DECL_OVERRIDE;
    /**
     * @brief resizeTo 缩放矩形时，用于设置矩形大小与位置
     * @param dir 8个方向
     * @param offset x，y方向移动距离
     * @param xScale X轴放大缩小比例
     * @param yScale y轴放大缩小比例
     */
    void resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                     const double &xScale, const double &yScale,
                     bool bShiftPress, bool bAltPress) override;

    /**
     * @brief line  返回线条
     */
    QLineF line() const;

    /**
     * @brief setLine  设置直线条
     */
    void setLine(const QLineF &line, bool init = false);
    /**
     * @brief setLine  设置直线条
     */
    void setLine(const QPointF &p1, const QPointF &p2, bool init = false);

    /**
     * @brief setLine  设置直线条
     */
    inline void setLine(qreal x1, qreal y1, qreal x2, qreal y2, bool init = false);

    /**
     * @brief getGraphicsUnit  返回图元的信息
     */
    CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;
    void loadGraphicsUnit(const CGraphicsUnit &data) override;

    /**
     * @brief getQuadrant  返回直线第二个点在第一个点的第几象限内
     */
    int getQuadrant() const;

    /**
     * @brief setLineStartType  设置线开始点的样式
     */
    void setLineStartType(ELineType type);

    /**
     * @brief getLineStartType  返回当前线开始点的样式
     */
    ELineType getLineStartType() const;

    /**
     * @brief setLineEndType  设置线结束点的样式
     */
    void setLineEndType(ELineType type);

    /**
     * @brief getLineEndType  返回当前线结束点的样式
     */
    ELineType getLineEndType() const;

    /**
     * @brief calcVertexes  计算箭头
     */
    void calcVertexes();

    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    QPainterPath getHighLightPath() Q_DECL_OVERRIDE;

    /**
     * @brief setLinePenWidth 设置线宽
     * @return
     */
    void setLinePenWidth(int width);

protected:
    /**
     * @brief duplicateCreatItem 创建一个同类型的图元（未同步数据）
     * @return
     */
    CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;

    /**
     * @brief duplicate 同步数据到item
     * @return
     */
    void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

    /**
     * @brief updateHandlesGeometry 刷新子节点位置
     * @return
     */
    void updateHandlesGeometry() Q_DECL_OVERRIDE;

    /**
     * @brief updateShape 刷新图元形状
     * @return
     */
    void updateShape() Q_DECL_OVERRIDE {calcVertexes();}

    /**
     * @brief paint 绘制图元
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;

    /**
     * @brief itemChange 图元变更
     * @param change 变更属性
     * @param value 变更的值
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;

private:
    void initLine();

    void initHandle() override;

    void drawStart();

    void drawEnd();

private:
    QLineF m_line; // 中间直线的位置信息
    QLineF m_dRectline; // 矩形直线的位置信息
    ELineType m_startType; // 起始点样式
    ELineType m_endType; // 终点样式

    QPainterPath m_startPath; // 绘制起点路径
    QPainterPath m_endPath; // 绘制终点路径
};

#endif // CGRAPHICSLINEITEM_H
