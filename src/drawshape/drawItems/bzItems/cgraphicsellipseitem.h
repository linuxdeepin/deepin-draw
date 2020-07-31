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
#ifndef CGRAPHICSELLIPSEITEM_H
#define CGRAPHICSELLIPSEITEM_H
#include "cgraphicsrectitem.h"

class CGraphicsEllipseItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsEllipseItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsEllipseItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsEllipseItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    explicit CGraphicsEllipseItem(const SGraphicsCircleUnitData *data,
                                  const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);

    /**
     * @brief shape 描述图元的形状（通过rect()并结合各类型图元确认）
     * @return
     */
    QPainterPath shape() const Q_DECL_OVERRIDE;

    int type() const Q_DECL_OVERRIDE;

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo) Q_DECL_OVERRIDE;

    /**
     * @brief getGraphicsUnit 返回图元数据
     * @return
     */
    CGraphicsUnit getGraphicsUnit(bool all) const Q_DECL_OVERRIDE;

    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    QPainterPath getHighLightPath() Q_DECL_OVERRIDE;

protected:
    /**
     * @brief duplicateCreatItem 创建同类型图元（未同步数据）
     * @return
     */
    CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;

    /**
     * @brief duplicate 同步数据到item
     * @return
     */
    void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

    /**
     * @brief inSideShape 定义了内部形状
     * @return
     */
    QPainterPath inSideShape() const Q_DECL_OVERRIDE;

    /**
     * @brief paint 绘制
     * @return
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;
};

#endif // CGRAPHICSELLIPSEITEM_H
