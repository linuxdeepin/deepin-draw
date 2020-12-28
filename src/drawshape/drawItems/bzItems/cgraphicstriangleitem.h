/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#ifndef CGRAPHICSTRIANGLEITEM_H
#define CGRAPHICSTRIANGLEITEM_H

#include "cgraphicsrectitem.h"
#include <QPainterPath>

class CGraphicsTriangleItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsTriangleItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);

    /**
     * @brief type 图元的类型
     * @return
     */
    int type() const override;

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data) override;

    /**
     * @brief getGraphicsUnit 得到图元数据
     * @return
     */
    CGraphicsUnit getGraphicsUnit(EDataReason reson) const override;

protected:

    /**
      * @brief selfOrgShape 图元的原始形状（rect类图元不包括边线）
      */
    QPainterPath getSelfOrgShape() const override;

    /**
     * @brief penStrokerShape 图元线条的形状（边线轮廓所组成的形状）
     */
    QPainterPath getTrulyShape() const override;


    /**
     * @brief shape 返回图元的外形状
     */
    QPainterPath getPenStrokerShape() const override;

    /**
     * @brief shape 返回真实显示的图元的外形状()
     */
    QPainterPath getShape() const override;

    /**
     * @brief updateShape 刷新图元的形状
     * @return
     */
    void updateShape() override;

    /**
     * @brief paint 绘制图元
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    void calcPoints();
    void calcPoints_helper(QVector<QPointF> &outVector, qreal offset = 0.0);


    QPolygonF polyForBrush;
    QPolygonF polyForPen;
    QPolygonF m_hightlightPath;
    QPainterPath m_pathForRenderPenLine;
};

#endif // CGRAPHICSTRIANGLEITEM_H
