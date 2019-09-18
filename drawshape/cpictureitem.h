/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     JiangChangli
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
#ifndef CPICTUREITEM_H
#define CPICTUREITEM_H

#include <QGraphicsPixmapItem>
#include "cgraphicsrectitem.h"



class CPictureItem : public  CGraphicsRectItem
{
public:
    explicit CPictureItem(const QPixmap &pixmap = QPixmap(), CGraphicsItem *parent = nullptr);
    explicit CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent = nullptr);
    explicit CPictureItem(const SGraphicsPictureUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent = nullptr);
    ~CPictureItem() Q_DECL_OVERRIDE;
    virtual int  type() const Q_DECL_OVERRIDE;
    void setMirror(bool hor, bool ver);
    void setRotation90(bool leftOrRight);

    /**
     * @brief duplicate 拷贝自己
     * @return
     */
    virtual void duplicate(CGraphicsItem *item) Q_DECL_OVERRIDE;

    virtual CGraphicsUnit getGraphicsUnit() const Q_DECL_OVERRIDE;

    void setPixmap(const QPixmap &pixmap);

    void setAngle(const qreal &angle);




protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;
    qreal m_angle;
};

#endif // CPICTUREITEM_H
