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
    explicit CGraphicsEllipseItem(const CGraphicsUnit &unit, CGraphicsItem *parent = nullptr);
    virtual QPainterPath shape() const Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    CGraphicsItem *duplicate() const Q_DECL_OVERRIDE;

    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;


protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
};

#endif // CGRAPHICSELLIPSEITEM_H
