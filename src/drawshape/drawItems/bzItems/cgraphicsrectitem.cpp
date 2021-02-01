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
#include "cgraphicsrectitem.h"
#include "csizehandlerect.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cgraphicsitemevent.h"

#include <QPainter>
#include <QPixmap>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPainter>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QtMath>

CGraphicsRectItem::CGraphicsRectItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    CGraphicsItem::initHandle();
}

CGraphicsRectItem::CGraphicsRectItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent),
      m_topLeftPoint(rect.topLeft()),
      m_bottomRightPoint(rect.bottomRight())
{
    CGraphicsItem::initHandle();
}

CGraphicsRectItem::CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    CGraphicsItem::initHandle();
}

CGraphicsRectItem::~CGraphicsRectItem()
{

}

int CGraphicsRectItem::type() const
{
    return RectType;
}

void CGraphicsRectItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    updateShape();
}

void CGraphicsRectItem::setXYRedius(int xRedius, int yRedius, bool preview)
{
    if (!preview) {
        m_xRedius = xRedius;
        m_yRedius = yRedius;
    } else {
        m_rediusForPreview = xRedius;
    }
    m_isPreviewRedius = preview;

    updateShape();
}

int CGraphicsRectItem::getXRedius()
{
    return  m_xRedius;
}

int CGraphicsRectItem::getPaintRedius()
{
    if (m_isPreviewRedius) {
        return m_rediusForPreview;
    }
    return m_xRedius;
}

void CGraphicsRectItem::loadGraphicsRectUnit(const SGraphicsRectUnitData &rectData)
{
    this->m_topLeftPoint = rectData.topLeft;
    this->m_bottomRightPoint = rectData.bottomRight;
    this->m_xRedius = rectData.xRedius;
    this->m_yRedius = rectData.yRedius;

    m_isPreviewRedius = false;

    this->setTransformOriginPoint(QRectF(m_topLeftPoint, m_bottomRightPoint).center());
}

QPainterPath CGraphicsRectItem::getSelfOrgShape() const
{
    QPainterPath path;
    path.addRoundedRect(this->rect(), m_xRedius, m_yRedius, Qt::AbsoluteSize);
    return path;
}

qreal CGraphicsRectItem::incLength() const
{
    return CGraphicsItem::incLength();
}

void CGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    beginCheckIns(painter);

    const QPen curPen = this->paintPen();

    int redius = getPaintRedius();
    //先绘制填充
    {
        qreal penWidthOffset = curPen.widthF();
        QRectF rectIn = rect();
        rectIn.adjust(penWidthOffset, penWidthOffset, -penWidthOffset, -penWidthOffset);

        painter->setPen(Qt::NoPen);
        painter->setBrush(paintBrush());
        painter->drawRoundedRect(rectIn, qMax(redius - penWidthOffset, 0.0), qMax(redius - penWidthOffset, 0.0), Qt::AbsoluteSize);
    }


    //再绘制描边
    {
        qreal penWidthOffset = curPen.widthF() / 2.0;
        QRectF rectIn = rect();
        rectIn.adjust(penWidthOffset, penWidthOffset, -penWidthOffset, -penWidthOffset);
        painter->setPen(curPen.width() == 0 ? Qt::NoPen : curPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rectIn, redius, redius, Qt::AbsoluteSize);
    }


    endCheckIns(painter);

    //224,224,224
    paintMutBoundingLine(painter, option);
}

void CGraphicsRectItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    if (data.data.pRect != nullptr) {
        loadGraphicsRectUnit(*data.data.pRect);
    }
    loadHeadData(data.head);

    updateShape();
}

CGraphicsUnit CGraphicsRectItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;

    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsRectUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = /*this->rotation()*/this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    unit.data.pRect = new SGraphicsRectUnitData();
    unit.data.pRect->topLeft = this->m_topLeftPoint;
    unit.data.pRect->bottomRight = this->m_bottomRightPoint;
    unit.data.pRect->xRedius = this->m_xRedius;
    unit.data.pRect->yRedius = this->m_yRedius;

    return unit;
}

QRectF CGraphicsRectItem::rect() const
{
    return QRectF(m_topLeftPoint, m_bottomRightPoint);
}

void CGraphicsRectItem::doScaling(CGraphItemScalEvent *event)
{
    QTransform trans = event->trans();
    QRectF rct = this->rect();
    QPointF pos1 = trans.map(rct.topLeft());
    QPointF pos4 = trans.map(rct.bottomRight());
    QRectF newRect = QRectF(pos1, pos4);

    if (newRect.isValid())
        this->setRect(newRect);
}

bool CGraphicsRectItem::testScaling(CGraphItemScalEvent *event)
{
    static QSizeF s_minSize = QSizeF(5, 5);
    bool accept = true;
    QTransform trans = event->trans();
    QRectF rct = this->rect();
    QPointF pos1 = trans.map(rct.topLeft());
    QPointF pos4 = trans.map(rct.bottomRight());
    QRectF newRect = QRectF(pos1, pos4);
    event->setMayResultPolygon(this->mapToScene(newRect));
    accept = newRect.isValid();
    if (accept) {
        if (newRect.width() < s_minSize.width() || newRect.height() < s_minSize.height())
            accept = false;
    }
//    if (accept) {
//        if (event->driverEvent() != nullptr) {
//            auto driverEvent = static_cast<CGraphItemScalEvent *>(event->driverEvent());
//            auto driverFatherScenePolygon = event->driverEvent()->item()->mapFromScene(driverEvent->mayResultPolygon());
//            auto selfWantedScenePolygon = event->driverEvent()->item()->mapFromScene(event->mayResultPolygon());
//            accept = driverFatherScenePolygon.boundingRect().adjusted(-1, -1, 1, 1).contains(selfWantedScenePolygon.boundingRect());
//        }
//    }
    return accept;
}
