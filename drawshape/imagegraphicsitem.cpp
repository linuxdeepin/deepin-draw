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


#include "imagegraphicsitem.h"


//ImageGraphicsItem::ImageGraphicsItem(QPointF &pointf, QPixmap &pixmap, QGraphicsItem *parent)
//    : BaseGraphicsItem(parent)
//{
//    m_targetPoint = pointf.toPoint();
//    m_pixmap = pixmap;
//    //m_width = rect.width();
//    //m_height = rect.height();
//    // m_initialRect = rect;

//    //m_localRect = m_initialRect;
//    //m_originPoint = QPointF(0, 0);

//    //updatehandles();
//}


ImageGraphicsItem::ImageGraphicsItem(QGraphicsItem *parent)
    : BaseGraphicsItem(parent)
{

}



QRectF ImageGraphicsItem::boundingRect() const
{
    //return QRectF(m_targetPoint, QPointF((m_pixmap.height()+m_targetPoint.x(),m_pixmap.width()+m_targetPoint.y()).normalized();
    return QRectF(0, 0, 500, 500);
}


QPainterPath ImageGraphicsItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());

    return path;
}

void ImageGraphicsItem::setPainter(const QPointF &targetPoint, const QPixmap &pixmap)
{
    m_targetPoint = targetPoint;
    m_pixmap = pixmap;
}


void ImageGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    painter->setPen(pen());
    painter->setBrush(Qt::NoBrush);
    painter->drawPixmap(m_targetPoint, m_pixmap);
    //painter->drawPixmap(1, 1, 50, 50, pixmap);
    //painter->drawPixmap(1, 1, pixmap);
}
