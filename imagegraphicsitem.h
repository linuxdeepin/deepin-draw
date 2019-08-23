/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Jiangcl
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

#ifndef IMAGEGRAPHICSITEM_H
#define IMAGEGRAPHICSITEM_H

#include "basegraphicsitem.h"

class ImageGraphicsItem : public BaseGraphicsItem
{
    Q_OBJECT

public:

    //ImageGraphicsItem(QPointF &pointf, QPixmap &pixmap, QGraphicsItem *parent = nullptr);
    ImageGraphicsItem(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setPainter( const QPointF &targetPoint, const QPixmap &pixmap);

//    QPainterPath shape() const;
//    void control(int dir, const QPointF &delta);
//    void stretch(int handle, double sx, double sy, const QPointF &origin);
//    QRectF  rect() const;
//    void updateCoordinate();
//    void move( const QPointF &point );
//    QGraphicsItem *duplicate () const ;



protected:
    //void updatehandles();
    //画图片

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    // void QPainter::drawPixmap(const QRectF &targetRect, const QPixmap &pixmap, const QRectF &sourceRect)

private:
    QPointF m_targetPoint;
    QPixmap m_pixmap;
};

#endif // IMAGEGRAPHICSITEM_H
