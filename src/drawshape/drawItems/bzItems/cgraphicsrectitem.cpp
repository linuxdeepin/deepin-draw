// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cgraphicsrectitem.h"
#include "csizehandlerect.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cgraphicsitemevent.h"
#include "cattributeitemwidget.h"
#include "cgraphicsitem.h"
#include "../../../service/dyncreatobject.h"

#include <QPainter>
#include <QPixmap>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPainter>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QtMath>

REGISTITEMCLASS(CGraphicsRectItem, RectType)
CGraphicsRectItem::CGraphicsRectItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    qDebug() << "Creating CGraphicsRectItem with default constructor";
    CGraphicsItem::initHandle();
}

CGraphicsRectItem::CGraphicsRectItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent),
      m_topLeftPoint(rect.topLeft()),
      m_bottomRightPoint(rect.bottomRight())
{
    qDebug() << "Creating CGraphicsRectItem with rect:" << rect;
    CGraphicsItem::initHandle();
}

CGraphicsRectItem::CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    qDebug() << "Creating CGraphicsRectItem with position:" << x << y << "size:" << w << h;
    CGraphicsItem::initHandle();
}

CGraphicsRectItem::~CGraphicsRectItem()
{
    qDebug() << "Destroying CGraphicsRectItem";
}

DrawAttribution::SAttrisList CGraphicsRectItem::attributions()
{
    DrawAttribution::SAttrisList result;
    result << DrawAttribution::SAttri(DrawAttribution::EBrushColor, brush().color())
           << DrawAttribution::SAttri(DrawAttribution::EPenColor, pen().color())
           << DrawAttribution::SAttri(DrawAttribution::EBorderWidth,  pen().width())
           << DrawAttribution::SAttri(DrawAttribution::ERectRadius,  m_xRedius);
    return result;
}

void CGraphicsRectItem::setAttributionVar(int attri, const QVariant &var, int phase)
{
    qDebug() << "Setting attribution:" << attri << "value:" << var << "phase:" << phase;
    if (attri == DrawAttribution::ERectRadius) {
        bool isPreview = (phase == EChangedBegin || phase == EChangedUpdate);
        setXYRedius(var.toInt(), var.toInt(), isPreview);
        return;
    }
    CGraphicsItem::setAttributionVar(attri, var, phase);
}

int CGraphicsRectItem::type() const
{
    return RectType;
}

void CGraphicsRectItem::setRect(const QRectF &rect)
{
    qDebug() << "Setting rectangle to:" << rect;
    if (rect.isValid()) {
        prepareGeometryChange();
        m_topLeftPoint = rect.topLeft();
        m_bottomRightPoint = rect.bottomRight();
        updateShape();
    } else {
        qWarning() << "Invalid rectangle provided:" << rect;
    }
}

void CGraphicsRectItem::setXYRedius(int xRedius, int yRedius, bool preview)
{
    qDebug() << "Setting radius - x:" << xRedius << "y:" << yRedius << "preview:" << preview;
    if (!preview) {
        m_xRedius = xRedius;
        m_yRedius = yRedius;
    } else {
        m_rediusForPreview = xRedius;
    }
    m_isPreviewRedius = preview;

    updateShape();

    if (curView() != nullptr) {
        curView()->viewport()->update();
    }
}

int CGraphicsRectItem::getXRedius()
{
    return  m_xRedius;
}

//int CGraphicsRectItem::getPaintRedius()
//{
//    if (m_isPreviewRedius) {
//        return m_rediusForPreview;
//    }
//    return m_xRedius;
//}

void CGraphicsRectItem::loadGraphicsRectUnit(const SGraphicsRectUnitData &rectData)
{
    qDebug() << "Loading graphics rect unit data";
    this->m_topLeftPoint = rectData.topLeft;
    this->m_bottomRightPoint = rectData.bottomRight;
    this->m_xRedius = rectData.xRedius;
    this->m_yRedius = rectData.yRedius;

    m_isPreviewRedius = false;

    this->setTransformOriginPoint(QRectF(m_topLeftPoint, m_bottomRightPoint).center());
}

void CGraphicsRectItem::paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    if (type() == RectType && m_isPreviewRedius) {
        beginCheckIns(painter);
        const QPen curPen = this->paintPen();
        painter->setPen(curPen.width() == 0 ? Qt::NoPen : curPen);
        painter->setBrush(this->paintBrush());
        painter->drawRoundedRect(this->rect(), m_rediusForPreview, m_rediusForPreview, Qt::AbsoluteSize);
        endCheckIns(painter);
        paintMutBoundingLine(painter, option);
    } else {
        CGraphicsItem::paintSelf(painter, option);
    }
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

void CGraphicsRectItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    qDebug() << "Loading graphics unit for rectangle";
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
    qDebug() << "Scaling rectangle with transform:" << event->trans();
    QTransform trans = event->trans();
    QRectF rct = this->rect();
    QPointF pos1 = trans.map(rct.topLeft());
    QPointF pos4 = trans.map(rct.bottomRight());
    QRectF newRect = QRectF(pos1, pos4);

    if (newRect.isValid())
        this->setRect(newRect);
    else
        qWarning() << "Invalid rectangle after scaling:" << newRect;
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
        if (newRect.width() < s_minSize.width() || newRect.height() < s_minSize.height()) {
            qDebug() << "Scaling rejected - new size too small:" << newRect.size() << "minimum:" << s_minSize;
            accept = false;
        }
    }
    return accept;
}
