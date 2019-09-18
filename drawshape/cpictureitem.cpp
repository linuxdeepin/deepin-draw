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
#include "cpictureitem.h"

#include <QPainter>
#include <QDebug>

CPictureItem::CPictureItem(const QPixmap &pixmap, CGraphicsItem *parent )
    : CGraphicsRectItem(parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
{

}


CPictureItem::CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent )
    : CGraphicsRectItem(rect, parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
{

}

CPictureItem::CPictureItem(const CGraphicsUnit &unit, CGraphicsItem *parent )
    : CGraphicsRectItem(unit, parent)
    , m_angle(0.0)
{
    m_pixmap = QPixmap::fromImage(unit.data.pPic->image);
//    QByteArray byteArray(unit.data.pPic->pic, unit.data.pPic->length);
//    m_pixmap.loadFromData(byteArray);
}


CPictureItem::~CPictureItem()
{

}

int  CPictureItem::type() const
{
    return PictureType;
}

void CPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    Q_UNUSED(option)
    Q_UNUSED(widget)
    //获取原始图片大小
    QRectF pictureRect = QRectF(0, 0, m_pixmap.width(), m_pixmap.height());
    painter->drawPixmap(rect(), m_pixmap, pictureRect);

    if (this->isSelected()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(224, 224, 224));
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->rect());
        painter->setClipping(true);
    }

}

void CPictureItem::setAngle(const qreal &angle)
{
    m_angle = angle;
}

void CPictureItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

//进行翻转，先转化为qimage，翻转后转化为qpixmap
void CPictureItem::setMirror(bool hor, bool ver)
{
    //qDebug() << "entered the  setMirror function" << endl;
    QImage image = m_pixmap.toImage();
    QImage mirrorImage = image.mirrored(hor, ver);
    m_pixmap = QPixmap::fromImage(mirrorImage);
    update();
}


void CPictureItem::setRotation90(bool leftOrRight)
{
    //旋转图形 有BUG
    QPointF center = this->rect().center();
    this->setTransformOriginPoint(center);
    if (leftOrRight == true) {
        m_angle = m_angle - 90.0;
        this->setRotation(m_angle);
    } else {
        m_angle = m_angle + 90.0;
        this->setRotation(m_angle);
    }
}

CGraphicsItem *CPictureItem::duplicate() const
{
    CPictureItem *item = new CPictureItem(rect(), m_pixmap);

    item->setAngle(m_angle);

    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
    return item;
}

CGraphicsUnit CPictureItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPictureUnitData);

    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();


    unit.data.pPic = new SGraphicsPictureUnitData();
    unit.data.pPic->rect.topLeft = this->rect().topLeft();
    unit.data.pPic->rect.bottomRight = this->rect().bottomRight();

//    unit.data.pPic->length =  m_pixmap.toImage().byteCount();
//    unit.data.pPic->pic = m_pixmap.toImage().bits();
    unit.data.pPic->image = m_pixmap.toImage();




//    QByteArray bytearray;
//    QBuffer buffer(&bytearray);
//    buffer.open(QIODevice::WriteOnly);
//    bool isSuccess = m_pixmap.save(&buffer, "PNG", 10);
//    if (isSuccess) {
//        unit.data.pPic->length = bytearray.length();
//        unit.data.pPic->pic = bytearray;
//    }


//    qDebug() << "!!!!!!!!!!!!!!!" << unit.data.pPic->length;

    return unit;
}

