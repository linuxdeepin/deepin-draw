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

class CGraphicsTriangleItem : public CGraphicsRectItem
{
public:
    explicit CGraphicsTriangleItem(CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(const QRectF &rect, CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent = nullptr);
    explicit CGraphicsTriangleItem(const SGraphicsTriangleUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    virtual void loadGraphicsUnit(const CGraphicsUnit &data) Q_DECL_OVERRIDE;
    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;
protected:
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    virtual CGraphicsItem *duplicateCreatItem() Q_DECL_OVERRIDE;
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;
    /**
     * @brief getHighLightPath 获取高亮path
     * @return
     */
    virtual QPainterPath getHighLightPath()Q_DECL_OVERRIDE;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

#endif // CGRAPHICSTRIANGLEITEM_H
