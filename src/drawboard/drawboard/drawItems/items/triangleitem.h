/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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

#include "rectitem.h"
#include <QPainterPath>

class DRAWLIB_EXPORT TriangleItem : public RectBaseItem
{
public:
    explicit TriangleItem(PageItem *parent = nullptr);
    explicit TriangleItem(const QRectF &rect, PageItem *parent = nullptr);
    explicit TriangleItem(qreal x, qreal y, qreal w, qreal h, PageItem *parent = nullptr);

    SAttrisList attributions() override;

    /**
     * @brief type 图元的类型
     * @return
     */
    int type() const override;

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadUnit(const Unit &data) override;

    /**
     * @brief getGraphicsUnit 得到图元数据
     * @return
     */
    Unit getUnit(int reson) const override;

protected:
    /**
      * @brief selfOrgShape 图元的原始形状（rect类图元不包括边线）
      */
    QPainterPath calOrgShapeBaseRect(const QRectF &rect) const override;

private:
    void calcPoints_helper(QVector<QPointF> &outVector, qreal offset = 0.0) const;


    QPolygonF polyForPen;
};

#endif // CGRAPHICSTRIANGLEITEM_H
