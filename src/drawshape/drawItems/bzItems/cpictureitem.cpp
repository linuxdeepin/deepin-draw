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
    //旋转图形 有BUG
    QPointF center = this->rect().center();
    this->setTransformOriginPoint(center);
    if (leftOrRight == true) {
        m_angle = this->rotation() - 90.0;
    } else {
        m_angle = this->rotation() + 90.0;
    }
    this->setRotation(m_angle);

    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->updateBlurItem(this);
    }
}

void CPictureItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pPic != nullptr) {
        CGraphicsRectItem::loadGraphicsRectUnit(data.data.pPic->rect);
        if (data.reson != EUndoRedo) {
            m_pixmap = QPixmap::fromImage(data.data.pPic->image);
            _srcByteArry = data.data.pPic->srcByteArry;
        }
        this->flipHorizontal = data.data.pPic->flipHorizontal;
        this->flipVertical = data.data.pPic->flipVertical;
    }
    loadHeadData(data.head);
//    updateShape();
//    if (isCached()) {
//        updateBlurPixmap(true);
//        resetCachePixmap();
//    }
    update();
}

CGraphicsUnit CPictureItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;
    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPictureUnitData);

    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();
    unit.head.blurCount = blurInfos.count();
    unit.head.blurInfos = blurInfos;
    unit.head.trans = this->transform();

    unit.data.pPic = new SGraphicsPictureUnitData();
    unit.data.pPic->rect.topLeft = this->rect().topLeft();
    unit.data.pPic->rect.bottomRight = this->rect().bottomRight();
    unit.data.pPic->flipHorizontal = this->flipHorizontal;
    unit.data.pPic->flipVertical = this->flipVertical;

    if (reson != EUndoRedo)
        unit.data.pPic->image = m_pixmap.toImage();

    if (reson != EUndoRedo) {
        if (_srcByteArry.isEmpty()) {
            QBuffer buferTemp;
            QDataStream strem(&buferTemp);
            strem << m_pixmap;
            buferTemp.close();
            unit.data.pPic->srcByteArry = buferTemp.buffer();
        } else {
            unit.data.pPic->srcByteArry = _srcByteArry;
        }
    }
    return unit;
}

