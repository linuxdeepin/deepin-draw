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
#include "rectitem.h"
#include "csizehandlerect.h"
#include "pageview.h"
#include "cgraphicsitemevent.h"
#include "attributewidget.h"

#include <QPainter>
#include <QPixmap>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPainter>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QtMath>

REGISTITEMCLASS(RectItem, RectType)
RectBaseItem::RectBaseItem(PageItem *parent)
    : RectBaseItem(0, 0, 0, 0, parent)
{
}

RectBaseItem::RectBaseItem(const QRectF &rect, PageItem *parent)
    : RectBaseItem(rect.left(), rect.top(), rect.width(), rect.height(), parent)
{

}

RectBaseItem::RectBaseItem(qreal x, qreal y, qreal w, qreal h, PageItem *parent)
    : VectorItem(parent), _rct(x, y, w, h)
{
}

RectBaseItem::~RectBaseItem()
{
}

QRectF RectBaseItem::rect() const
{
    return _rct;
}

void RectBaseItem::setRect(const QRectF &rect)
{
    if (rect != _rct) {
        preparePageItemGeometryChange();
        _rct = rect;
        updateShape();
    }
}

QPainterPath RectBaseItem::getOrgShape() const
{
    return calOrgShapeBaseRect(rect());
}

void RectBaseItem::loadUnit(const Unit &data)
{
    RectUnitData i = data.value<RectUnitData>();
    loadVectorData(data.head);
    loadGraphicsRectUnit(i);
}

Unit RectBaseItem::getUnit(int reson) const
{
    Unit unit;

    unit.usage = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(RectUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    RectUnitData i;
    i.topLeft = _rct.topLeft();
    i.bottomRight = _rct.bottomRight();
//    i.xRedius = m_xRedius;
//    i.yRedius = m_yRedius;
    unit.data.data.setValue<RectUnitData>(i);

    return unit;
}

//QRectF RectBaseItem::itemRect() const
//{
//    return VectorItem::itemRect();
//}

//QRectF RectBaseItem::orgRect() const
//{
//    return rect();
//}

void RectBaseItem::doScaling(PageItemScalEvent *event)
{
    QTransform trans = event->trans();
    QRectF rct = this->rect();
    QPointF pos1 = trans.map(rct.topLeft());
    QPointF pos4 = trans.map(rct.bottomRight());
    QRectF newRect = QRectF(pos1, pos4);

    if (newRect.isValid()) {
        this->setRect(newRect);
    }
}

bool RectBaseItem::testScaling(PageItemScalEvent *event)
{
    static QSizeF s_minSize = QSizeF(5, 5);
    bool accept = true;
    QTransform trans = event->trans();
    QRectF rct = this->orgRect();
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

void RectItem::setXYRedius(int xRedius, int yRedius, bool preview)
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

int RectItem::getXRedius() const
{
    return m_xRedius;
}

void RectItem::loadUnit(const Unit &ut)
{
    RectBaseItem::loadUnit(ut);
    RectUnitData d = ut.value<RectUnitData>();
    setXYRedius(d.xRedius, d.yRedius);
    m_isPreviewRedius = false;
}

Unit RectItem::getUnit(int reson) const
{
    Unit ut = RectBaseItem::getUnit(reson);
    RectUnitData d = ut.value<RectUnitData>();
    d.xRedius = m_xRedius;
    d.yRedius = m_yRedius;
    ut.setValue<RectUnitData>(d);
    return ut;
}

void RectBaseItem::loadGraphicsRectUnit(const RectUnitData &rectData)
{
    setRect(QRectF(rectData.topLeft, rectData.bottomRight));
}

int RectItem::type() const
{
    return RectType;
}

SAttrisList RectItem::attributions()
{
    SAttrisList result;
    result <<  SAttri(EBrushColor, brush().color())
           <<  SAttri(EPenColor, pen().color())
           << SAttri(EPenWidth, pen().width())
           //<<  SAttri(EBorderWidth,  pen().width())
           <<  SAttri(ERectRadius,  m_xRedius)
           << SAttri(ERotProperty,  drawRotation());
    return result;
}

void RectItem::setAttributionVar(int attri, const QVariant &var, int phase)
{
    if (attri ==  ERectRadius) {
        bool isPreview = (phase == EChangedBegin || phase == EChangedUpdate);
        setXYRedius(var.toInt(), var.toInt(), isPreview);
        return;
    }
    VectorItem::setAttributionVar(attri, var, phase);
}

QPainterPath RectItem::calOrgShapeBaseRect(const QRectF &rect) const
{
    QPainterPath path;
    path.addRoundedRect(rect, m_xRedius, m_yRedius, Qt::AbsoluteSize);
    return path;
}
void RectItem::paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    if (type() == RectType && m_isPreviewRedius) {
        beginCheckIns(painter);
        const QPen curPen = this->paintPen();
        painter->setPen(curPen.width() == 0 ? Qt::NoPen : curPen);
        painter->setBrush(this->paintBrush());
        painter->drawRoundedRect(this->orgRect(), m_rediusForPreview, m_rediusForPreview, Qt::AbsoluteSize);
        endCheckIns(painter);


        painter->setBrush(QColor(255, 0, 0));
        painter->drawPath(itemShape());


        paintMutBoundingLine(painter, option);
    } else {
        VectorItem::paintSelf(painter, option);
    }
}
