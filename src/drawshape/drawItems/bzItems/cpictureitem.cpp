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
#include "cdrawscene.h"
#include "global.h"
#include "cgraphicsview.h"
#include "cgraphicsitemevent.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

CPictureItem::CPictureItem(const QPixmap &pixmap, CGraphicsItem *parent, const QByteArray &fileSrcData)
    : CGraphicsRectItem(parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
    , _srcByteArry(fileSrcData)
{
    updateShape();
}


CPictureItem::CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent, const QByteArray &fileSrcData)
    : CGraphicsRectItem(rect, parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
    , _srcByteArry(fileSrcData)
{
    this->setPen(Qt::NoPen);
    updateShape();
}

CPictureItem::~CPictureItem()
{

}

int  CPictureItem::type() const
{
    return PictureType;
}

void CPictureItem::paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(option)

    QRectF pictureRect = QRectF(0, 0, m_pixmap.width(), m_pixmap.height());

    painter->drawPixmap(rect(), m_pixmap, pictureRect);
}

void CPictureItem::setAngle(const qreal &angle)
{
    m_angle = angle;
}

bool CPictureItem::isPosPenetrable(const QPointF &posLocal)
{
    Q_UNUSED(posLocal)
    return false;
}

bool CPictureItem::isRectPenetrable(const QRectF &rectLocal)
{
    Q_UNUSED(rectLocal)
    return false;
}

QPainterPath CPictureItem::getSelfOrgShape() const
{
    QPainterPath path;
    path.addRect(this->rect());
    return path;
}

void CPictureItem::operatingEnd(CGraphItemEvent *event)
{
    if (event->toolEventType() == 3) {
        //刷新出新的模糊图
        updateBlurPixmap(true);
    }
    CGraphicsRectItem::operatingEnd(event);
}

void CPictureItem::operatingBegin(CGraphItemEvent *event)
{
    _trans.reset();
    CGraphicsRectItem::operatingBegin(event);
}

void CPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CGraphicsItem::paint(painter, option, widget);
}

void CPictureItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

void CPictureItem::setRotation90(bool leftOrRight)
{

    QPointF center = this->boundingRect().center();
    qreal angle = 0.0;

    // 左旋转减小 右旋转增大
    if (leftOrRight == true) {
        angle = - 90.0;
    } else {
        angle = 90.0;
    }

    // 矩阵变换
    QTransform trans;
    trans.translate(center.x(), center.y());
    trans.rotate(angle);
    trans.translate(-center.x(), -center.y());

    // 设置当前旋转角度
    setDrawRotatin(angle + drawRotation());
    setTransform(trans, true);
}

void CPictureItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pPic != nullptr) {
        CGraphicsRectItem::loadGraphicsRectUnit(data.data.pPic->rect);

        qDebug() << "-------data.reson = " << data.reson;
        //仅在 '复制' 或者 '保存/加载到ddf文件时'加载图片原数据
        if (data.reson == EDuplicate || data.reson == ESaveToDDf) {
            m_pixmap = QPixmap::fromImage(data.data.pPic->image);
            _srcByteArry = data.data.pPic->srcByteArry;
        }
        this->_flipHorizontal = data.data.pPic->flipHorizontal;
        this->_flipVertical = data.data.pPic->flipVertical;
    }
    loadHeadData(data.head);
    update();
}

CGraphicsUnit CPictureItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;
    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPictureUnitData);

    unit.head.pos = this->pos();
    unit.head.rotate = this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.blurCount = _blurInfos.count();
    unit.head.blurInfos = _blurInfos;
    unit.head.trans = this->transform();

    unit.data.pPic = new SGraphicsPictureUnitData();
    unit.data.pPic->rect.topLeft = this->rect().topLeft();
    unit.data.pPic->rect.bottomRight = this->rect().bottomRight();
    unit.data.pPic->flipHorizontal = this->_flipHorizontal;
    unit.data.pPic->flipVertical = this->_flipVertical;

    //仅在 '复制' 或者 '保存/加载到ddf文件时'读取图片原数据
    if (reson == EDuplicate || reson == ESaveToDDf) {
        unit.data.pPic->image = m_pixmap.toImage();

        //源数据不为空,那么进行对当前图片
        if (_srcByteArry.isEmpty()) {
            QBuffer buferTemp;
            if (buferTemp.open(QIODevice::ReadWrite)) {
                m_pixmap.save(&buferTemp, "png", 100);
                unit.data.pPic->srcByteArry = buferTemp.buffer();
                buferTemp.close();
            }
        } else {
            unit.data.pPic->srcByteArry = _srcByteArry;
        }
    }
    return unit;
}

