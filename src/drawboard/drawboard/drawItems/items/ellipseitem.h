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
#ifndef CGRAPHICSELLIPSEITEM_H
#define CGRAPHICSELLIPSEITEM_H
#include "rectitem.h"
#include <QPainterPath>

class DRAWLIB_EXPORT EllipseItem : public RectBaseItem
{
public:
    explicit EllipseItem(PageItem *parent = nullptr);
    explicit EllipseItem(qreal x, qreal y, qreal w, qreal h, PageItem *parent = nullptr);

    int type() const override;

    SAttrisList attributions() override;

    /**
     * @brief loadGraphicsUnit 加载图元数据
     * @return
     */
    void loadUnit(const Unit &data) override;

    /**
     * @brief getGraphicsUnit 返回图元数据
     * @return
     */
    Unit getUnit(int reson) const override;

protected:

    /**
     * @brief inSideShape 定义了内部形状
     * @return
     */
    QPainterPath calOrgShapeBaseRect(const QRectF &rect) const override;
};

#endif // CGRAPHICSELLIPSEITEM_H
