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
#include <QPainter>
#include <QDebug>

CPictureItem::CPictureItem(const QPixmap &pixmap, CGraphicsItem *parent, const QByteArray &fileSrcData)
    : CGraphicsRectItem(parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
    , _srcByteArry(fileSrcData)
{

}


CPictureItem::CPictureItem(const QRectF &rect, const QPixmap &pixmap, CGraphicsItem *parent, const QByteArray &fileSrcData)
    : CGraphicsRectItem(rect, parent)
    , m_pixmap(pixmap)
    , m_angle(0.0)
    , _srcByteArry(fileSrcData)
{

}

CPictureItem::CPictureItem(const SGraphicsPictureUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsRectItem(data->rect, head, parent)
    , m_angle(0.0)
    , _srcByteArry(data->srcByteArry)
{
    m_pixmap = QPixmap::fromImage(data->image);
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

    beginCheckIns(painter);

    //获取原始图片大小
    QRectF pictureRect = QRectF(0, 0, m_pixmap.width(), m_pixmap.height());
    painter->drawPixmap(rect(), m_pixmap, pictureRect);

    endCheckIns(painter);

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
    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->updateBlurItem(this);
    }
}


void CPictureItem::setRotation90(bool leftOrRight)
{
    //旋转图形 有BUG
    QPointF center = this->rect().center();
    this->setTransformOriginPoint(center);
    if (leftOrRight == true) {
        m_angle = this->rotation() - 90.0;
        this->setRotation(m_angle);
    } else {
        m_angle = this->rotation() + 90.0;
        this->setRotation(m_angle);
    }

    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        curScene->updateBlurItem(this);
    }
}

bool CPictureItem::getAdjustScence()
{
    return m_adjustScence;
}

void CPictureItem::duplicate(CGraphicsItem *item)
{
    CGraphicsRectItem::duplicate(item);

    CPictureItem *pPic = dynamic_cast<CPictureItem *>(item);

    pPic->setPixmap(m_pixmap);

    pPic->_srcByteArry = _srcByteArry;
}

void CPictureItem::loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo)
{
    Q_UNUSED(allInfo)
    if (data.data.pPic != nullptr) {
        CGraphicsRectItem::loadGraphicsRectUnit(data.data.pPic->rect);
        if (allInfo) {
            m_pixmap = QPixmap::fromImage(data.data.pPic->image);
            _srcByteArry = data.data.pPic->srcByteArry;
        }
    }
    loadHeadData(data.head);
}
CGraphicsUnit CPictureItem::getGraphicsUnit(bool all) const
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

    if (all)
        unit.data.pPic->image = m_pixmap.toImage();

    if (all) {
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

QPainterPath CPictureItem::getHighLightPath()
{
    QPainterPath path;
    path.addRect(this->rect());
    return path;
}

