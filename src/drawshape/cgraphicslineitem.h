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
    virtual int  type() const Q_DECL_OVERRIDE;

    QPainterPath shape() const Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual QRectF rect() const Q_DECL_OVERRIDE;

    virtual void rotatAngle(qreal angle) Q_DECL_OVERRIDE;

    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point) Q_DECL_OVERRIDE;

    virtual void resizeTo(CSizeHandleRect::EDirection dir,
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

    void resizeToMul_7(CSizeHandleRect::EDirection dir, QRectF pressRect, QRectF itemPressRect,
                       const qreal &xScale, const qreal &yScale,
                       bool bShiftPress, bool bAltPress) override Q_DECL_DEPRECATED;

    QLineF line() const;
    void setLine(const QLineF &line, bool init = false);
    void setLine(const QPointF &p1, const QPointF &p2, bool init = false);
    inline void setLine(qreal x1, qreal y1, qreal x2, qreal y2, bool init = false);

    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    virtual CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

    //判断直线第二个点在第一个点的第几象限内

    int getQuadrant() const;

    void setLineStartType(ELineType type);
    ELineType getLineStartType() const;

    void setLineEndType(ELineType type);
    ELineType getLineEndType() const;

    /**
     * @brief calcVertexes  计算箭头
     * @param prePoint
     * @param currentPoint
     */
    void calcVertexes();
    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    virtual QPainterPath getHighLightPath() Q_DECL_OVERRIDE;

    /*
    * @bref: setLinePenWidth 设置线的宽度
    * @param: width 宽度
    */
    void setLinePenWidth(int width);

protected:
    void updateGeometry() Q_DECL_OVERRIDE;
    void updateShape() Q_DECL_OVERRIDE {calcVertexes();}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    void initLine();

    void initHandle() override;


private:
    QLineF m_line; // 中间直线的位置信息
    QLineF m_dRectline; // 矩形直线的位置信息
    ELineType m_startType; // 起始点样式
    ELineType m_endType; // 终点样式

    QPainterPath m_startPath; // 绘制起点路径
    QPainterPath m_endPath; // 绘制终点路径

    void drawStart();
    void drawEnd();
};

#endif // CGRAPHICSLINEITEM_H
