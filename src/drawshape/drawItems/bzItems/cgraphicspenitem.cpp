/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "cgraphicspenitem.h"
#include "cdrawparamsigleton.h"
#include "cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "cgraphicsitemevent.h"

#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QDebug>

const int SmoothMaxCount = 10;

// 绘制起始终点的最小矩形范围
const QSizeF minRectSize(10, 10);

CGraphicsPenItem::CGraphicsPenItem(QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isShiftPress(false)
    , m_isDrawing(false)
    , m_drawIndex(0)
    , m_penStartType(noneLine)
    , m_penEndType(noneLine)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

CGraphicsPenItem::CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isShiftPress(false)
    , m_isDrawing(false)
    , m_drawIndex(0)
    , m_penStartType(noneLine)
    , m_penEndType(noneLine)

{
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);

    m_path.moveTo(startPoint);
    m_smoothVector.push_back(startPoint);
}

CGraphicsPenItem::~CGraphicsPenItem()
{

}

int CGraphicsPenItem::type() const
{
    return PenType;
}

void CGraphicsPenItem::doChangeSelf(CGraphItemEvent *event)
{
    switch (event->type()) {
    case CGraphItemEvent::EScal: {
        QTransform trans = event->trans();
        m_path = trans.map(m_path);
        updateShape();
        break;
    }
    default:
        CGraphicsItem::doChangeSelf(event);
        break;
    }
}

QRectF CGraphicsPenItem::rect() const
{
    QPainterPath path;
    path = m_path;

    if (m_isShiftPress) {
        path.lineTo(m_straightLine.p2());
    }

    path.addPath(m_startPath);
    path.addPath(m_endPath);

    return path.controlPointRect().normalized();
}

CGraphicsUnit CGraphicsPenItem::getGraphicsUnit(EDataReason reson) const
{
    CGraphicsUnit unit;

    unit.reson = reson;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPenUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = /*this->rotation()*/this->drawRotation();
    unit.head.zValue = this->zValue();
    unit.head.trans = this->transform();

    unit.data.pPen = new SGraphicsPenUnitData();
    unit.data.pPen->start_type = this->m_penStartType;
    unit.data.pPen->end_type = this->m_penEndType;
    unit.data.pPen->path = this->m_path;
//    unit.data.pPen->arrow = this->m_arrow;
//    unit.data.pPen->poitsVector = this->m_poitsPath;

    return unit;
}

void CGraphicsPenItem::drawComplete(bool doBz)
{
    if (m_isShiftPress) {
        m_isShiftPress = false;
        m_path.lineTo(m_straightLine.p2());
    }

    if (m_path.elementCount() > 5 && doBz) {
        QPainterPath vout;
        for (int i = 0; i < m_path.elementCount() - 5; i += 5) {
            QPainterPath::Element p0 = m_path.elementAt(i);
            QPainterPath::Element p1 = m_path.elementAt(i + 1);
            QPainterPath::Element p2 = m_path.elementAt(i + 2);
            QPainterPath::Element p3 = m_path.elementAt(i + 3);
            QPainterPath::Element p4 = m_path.elementAt(i + 4);
            QPainterPath::Element p5 = m_path.elementAt(i + 5);

            if (0 == i) {
                QPointF dot1 = GetBezierValue(p0, p1, p2, p3, p4, p5, 0.);
                vout.moveTo(dot1);
            }
            QPointF dot2 = GetBezierValue(p0, p1, p2, p3, p4, p5,  1 / 5.0);
            QPointF dot3 = GetBezierValue(p0, p1, p2, p3, p4, p5,  2 / 5.0);
            QPointF dot4 = GetBezierValue(p0, p1, p2, p3, p4, p5,  3 / 5.0);
            QPointF dot5 = GetBezierValue(p0, p1, p2, p3, p4, p5,  4 / 5.0);
            QPointF dot6 = GetBezierValue(p0, p1, p2, p3, p4, p5,  1);

            vout.lineTo(dot2);
            vout.lineTo(dot3);
            vout.lineTo(dot4);
            vout.lineTo(dot5);
            vout.lineTo(dot6);
        }

        //保证未被优化的点也加入到最终的绘制路径中
        if (vout.elementCount() < m_path.elementCount()) {
            for (int i = vout.elementCount() - 1; i < m_path.elementCount(); ++i) {
                QPointF psF(m_path.elementAt(i).x, m_path.elementAt(i).y);
                vout.lineTo(psF);
            }
        }
        prepareGeometryChange();
        m_path = vout;
    }

    updateShape();

    m_isStartWithLine = false;
    m_isEndWithLine = false;
}


void CGraphicsPenItem::updatePenPath(const QPointF &endPoint, bool isShiftPress)
{
    prepareGeometryChange();
    m_isShiftPress = isShiftPress;

    if (isShiftPress) {
        m_straightLine.setP1(m_path.currentPosition());
        m_straightLine.setP2(endPoint);
        //calcVertexes(m_straightLine.p1(), m_straightLine.p2());
    } else {
        m_path.lineTo(endPoint);
        m_smoothVector.push_back(endPoint);
        if (m_smoothVector.count() > SmoothMaxCount) {
            m_smoothVector.removeFirst();
        }
        //calcVertexes(m_path.elementAt(0), endPoint);
    }

    //CGraphicsItem::updateShape();
    //updateHandlesGeometry();

    updateShape();
    updateHandlesGeometry();
}

qreal CGraphicsPenItem::GetBezierValue(qreal p0, qreal p1, qreal p2, qreal p3, qreal p4, qreal p5, qreal t)
{

    return pow(1 - t, 5.0) * p0 + 5 * pow(1 - t, 4.0) * t * p1 + 10 * pow(1 - t, 3.0) * pow(t, 2.0) * p2 + 10 * pow(1 - t, 2.0) * pow(t, 3.0) * p3 + 5 * pow(1 - t, 1.0) * pow(t, 4.0) * p4 + pow(t, 5.0) * p5;
}

QPointF CGraphicsPenItem::GetBezierValue(QPainterPath::Element p0, QPainterPath::Element p1, QPainterPath::Element p2, QPainterPath::Element p3, QPainterPath::Element p4, QPainterPath::Element p5, qreal t)
{
    QPointF dot;
    dot = QPointF(GetBezierValue(p0.x, p1.x, p2.x, p3.x, p4.x, p5.x, t), GetBezierValue(p0.y, p1.y, p2.y, p3.y,  p4.y, p5.y, t));
    return dot;
}

void CGraphicsPenItem::updateStartPathStyle()
{
    if (m_straightLine.isNull()) {
        if (m_path.boundingRect().width() < minRectSize.width() && m_path.boundingRect().height() < minRectSize.height())
            return;
    }

    // 判断当前是否是以画直线开始绘制
    if (!m_isStartWithLine && !m_straightLine.isNull() && m_path.isEmpty() && m_isShiftPress) {
        m_isStartWithLine = true;
    } else if (m_isStartWithLine && !m_isShiftPress) {
        m_isStartWithLine = false;
    }

    QLineF line;
    if (!m_isStartWithLine && m_path.elementCount() > 2) { // 开始画曲线
        line = QLineF(m_path.elementAt(0), m_path.elementAt(2));
    } else if (!m_isStartWithLine && m_path.elementCount() > 1) {
        line = QLineF(m_path.elementAt(0), m_path.elementAt(1));
    } else { // 开始画直线
        line = m_straightLine;
    }

    QLineF templine(line.p2(), line.p1());

    QLineF v = templine.unitVector();
    v.setLength(10 + pen().width() * 3); //改变单位向量的大小，实际就是改变箭头长度
    v.translate(QPointF(templine.dx(), templine.dy()));

    QLineF n = v.normalVector(); //法向量
    n.setLength(n.length() * 0.5); //这里设定箭头的宽度
    QLineF n2 = n.normalVector().normalVector(); //两次法向量运算以后，就得到一个反向的法向量

    QPointF p1 = v.p2();
    QPointF p2 = n.p2();
    QPointF p3 = n2.p2();

    //减去一个箭头的宽度
    QPointF diffV = p1 - line.p1();
    p1 -= diffV;
    p2 -= diffV;
    p3 -= diffV;

    switch (m_penStartType) {
    case noneLine: {
        m_startPath = QPainterPath(p1);
        break;
    }
    case normalArrow: {
        p1 += diffV;
        p2 += diffV;
        p3 += diffV;
        m_startPath = QPainterPath(p1);
        m_startPath.lineTo(p2);
        m_startPath.moveTo(p1);
        m_startPath.lineTo(p3);
        m_startPath.moveTo(p1);
        QPointF center = (p2 + p3) / 2;
        m_startPath.lineTo(center);
        m_startPath.moveTo(p1);
        break;
    }
    case soildArrow: {
        p1 += diffV;
        p2 += diffV;
        p3 += diffV;
        m_startPath = QPainterPath(p1);
        m_startPath.lineTo(p3);
        m_startPath.lineTo(p2);
        m_startPath.lineTo(p1);
        break;
    }
    case normalRing: {
        //qreal radioWidth = this->pen().width() * 2;
        qreal penWMin = qMax(pen().width(), 1);
        qreal radioWidth = penWMin * 2;
        QPointF center;
        qreal yOff = qSin(line.angle() / 180 * M_PI) * radioWidth;
        qreal xOff = qCos(line.angle() / 180 * M_PI) * radioWidth;
        center = line.p1() + QPointF(-xOff, yOff);
        QRectF ecliRect(center + QPointF(-radioWidth, -radioWidth), QSizeF(2 * radioWidth, 2 * radioWidth));
        m_startPath = QPainterPath(center + QPointF(radioWidth, 0));
        m_startPath.arcTo(ecliRect, 0, 360);
        break;
    }
    case soildRing: {
        //qreal radioWidth = this->pen().width() * 2;
        qreal penWMin = qMax(pen().width(), 1);
        qreal radioWidth = penWMin * 2;
        QPointF center;
        qreal yOff = qSin(line.angle() / 180 * M_PI) * radioWidth;
        qreal xOff = qCos(line.angle() / 180 * M_PI) * radioWidth;
        center = line.p1() + QPointF(-xOff, yOff);
        QRectF ecliRect(center + QPointF(-radioWidth, -radioWidth), QSizeF(2 * radioWidth, 2 * radioWidth));
        m_startPath = QPainterPath(center + QPointF(radioWidth, 0));
        m_startPath.arcTo(ecliRect, 0, 360);
        break;
    }
    }
}

void CGraphicsPenItem::updateEndPathStyle()
{
    if (m_straightLine.isNull()) {
        if (m_path.boundingRect().width() < minRectSize.width() && m_path.boundingRect().height() < minRectSize.height())
            return;
    }

    // 判断当前是否是以画直线结束绘制
    if (!m_isEndWithLine && m_isShiftPress) {
        m_isEndWithLine = true;
    } else if (m_isEndWithLine && !m_isShiftPress) {
        m_isEndWithLine = false;
    }

    QLineF line;
    if (m_isEndWithLine) {
        line = m_straightLine;
    } else {
        int count = m_path.elementCount();
        line = QLineF(m_path.elementAt(qMax(count - 10, 0)), m_path.elementAt(count - 1));
    }



    QPointF prePoint = line.p1();
    QPointF currentPoint = line.p2();
    if (prePoint == currentPoint) {
        return;
    }

    QLineF v = line.unitVector();
    v.setLength(10 + pen().width() * 3); //改变单位向量的大小，实际就是改变箭头长度
    v.translate(QPointF(line.dx(), line.dy()));

    QLineF n = v.normalVector(); //法向量
    n.setLength(n.length() * 0.5); //这里设定箭头的宽度
    QLineF n2 = n.normalVector().normalVector(); //两次法向量运算以后，就得到一个反向的法向量

    QPointF p1 = v.p2();
    QPointF p2 = n.p2();
    QPointF p3 = n2.p2();

    //减去一个箭头的宽度
    QPointF diffV = p1 - line.p2();
    //    QPointF diffV = p1 - m_line.p1();
    p1 -= diffV;
    p2 -= diffV;
    p3 -= diffV;
    // 绘制终点
    switch (m_penEndType) {
    case noneLine: {
        m_endPath = QPainterPath(p1);
        break;
    }
    case normalArrow: {
        p1 += diffV;
        p2 += diffV;
        p3 += diffV;
        m_endPath = QPainterPath(p1);
        m_endPath.lineTo(p2);
        m_endPath.moveTo(p1);
        m_endPath.lineTo(p3);
        m_endPath.moveTo(p1);
        QPointF center = (p2 + p3) / 2;
        m_endPath.lineTo(center);
        m_endPath.moveTo(p1);
        break;
    }
    case soildArrow: {
        p1 += diffV;
        p2 += diffV;
        p3 += diffV;
        m_endPath = QPainterPath(p1);
        m_endPath.lineTo(p3);
        m_endPath.lineTo(p2);
        m_endPath.lineTo(p1);
        break;
    }
    case normalRing: {
        qreal penWMin = qMax(pen().width(), 1);
        qreal radioWidth = penWMin * 2;
        QPointF center;
        qreal yOff = qSin(line.angle() / 180 * M_PI) * radioWidth;
        qreal xOff = qCos(line.angle() / 180 * M_PI) * radioWidth;
        center = line.p2() + QPointF(xOff, -yOff);
        QRectF ecliRect(center + QPointF(-radioWidth, -radioWidth), QSizeF(2 * radioWidth, 2 * radioWidth));
        m_endPath = QPainterPath(center + QPointF(radioWidth, 0));
        m_endPath.arcTo(ecliRect, 0, 360);
        break;
    }
    case soildRing: {
        qreal penWMin = qMax(pen().width(), 1);
        qreal radioWidth = penWMin * 2;
        //qreal radioWidth = this->pen().width() * 2;
        QPointF center;
        qreal yOff = qSin(line.angle() / 180 * M_PI) * radioWidth;
        qreal xOff = qCos(line.angle() / 180 * M_PI) * radioWidth;
        center = line.p2() + QPointF(xOff, -yOff);
        QRectF ecliRect(center + QPointF(-radioWidth, -radioWidth), QSizeF(2 * radioWidth, 2 * radioWidth));
        m_endPath = QPainterPath(center + QPointF(radioWidth, 0));
        m_endPath.arcTo(ecliRect, 0, 360);
        break;
    }
    }
}

void CGraphicsPenItem::updateHandlesGeometry()
{
    const QRectF &geom = this->boundingRect();

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;
        qreal w = hndl->boundingRect().width();
        qreal h = hndl->boundingRect().height();
        switch (hndl->dir()) {
        case CSizeHandleRect::LeftTop:
            hndl->move(geom.x() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Top:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::RightTop:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Right:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::RightBottom:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Bottom:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::LeftBottom:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Left:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::Rotation:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h - h / 2);
            break;
        default:
            break;
        }
    }
}

void CGraphicsPenItem::updateShape()
{
    prepareGeometryChange();

    calcVertexes();

    m_selfOrgPathShape   = getSelfOrgShape();
    m_penStroerPathShape = getPenStrokerShape();
    m_boundingShape      = m_penStroerPathShape;
    m_boundingRect       = m_boundingShape.controlPointRect();

    m_boundingShapeTrue = getTrulyShape();
    m_boundingRectTrue  = m_boundingShapeTrue.controlPointRect();


    //resetCachePixmap();

    if (drawScene() != nullptr)
        drawScene()->updateMrItemBoundingRect();

    update();
}

void CGraphicsPenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CGraphicsItem::paint(painter, option, widget);
}

void CGraphicsPenItem::paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    //qDebug() << "cur cached flag = " << _useCachePixmap;
    QPen pen = this->paintPen();
    pen.setJoinStyle(Qt::BevelJoin);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setPen(pen);

    painter->setPen(this->pen().width() == 0 ? Qt::NoPen : this->paintPen());
    painter->drawPath(m_path);

    if (m_isShiftPress) {
        painter->drawLine(m_straightLine);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setBrush(Qt::NoBrush);
    if (pen.width()) {
        if (m_penStartType == soildArrow || m_penStartType == soildRing) {
            painter->setBrush(QBrush(QColor(pen.color())));
        }
    }
    painter->drawPath(m_startPath);

    painter->setBrush(Qt::NoBrush);
    if (pen.width()) {
        if (m_penEndType == soildArrow || m_penEndType == soildRing) {
            painter->setBrush(QBrush(QColor(pen.color())));
        }
    }
    painter->drawPath(m_endPath);
}

void CGraphicsPenItem::operatingBegin(CGraphItemEvent *event)
{
    return CGraphicsItem::operatingBegin(event);
}

void CGraphicsPenItem::operatingEnd(CGraphItemEvent *event)
{
    return CGraphicsItem::operatingEnd(event);
}

bool CGraphicsPenItem::isPosPenetrable(const QPointF &posLocal)
{
    Q_UNUSED(posLocal)
//    if (pen().color().alpha() == 0 || pen().width() == 0 || pen().widthF() == .0) {
//        return true;
//    }
    return false;
}

QPainterPath CGraphicsPenItem::getPath() const
{
    return m_path;
}

void CGraphicsPenItem::setPenStartpath(const QPainterPath &path)
{
    m_startPath = path;
}

QPainterPath CGraphicsPenItem::getPenStartpath() const
{
    return m_startPath;
}

void CGraphicsPenItem::setPenEndpath(const QPainterPath &path)
{
    m_endPath = path;
}

QPainterPath CGraphicsPenItem::getPenEndpath() const
{
    return m_endPath;
}

void CGraphicsPenItem::setPath(const QPainterPath &path)
{
    m_path = path;
    updateShape();
}

void CGraphicsPenItem::setDrawFlag(bool flag)
{
    m_isDrawing = flag;
}

void CGraphicsPenItem::calcVertexes(const QPointF &prePoint, const QPointF &currentPoint)
{
    if (prePoint == currentPoint) {
        return;
    }

    prepareGeometryChange();

    updateStartPathStyle();
    updateEndPathStyle();

    // 更新画布区域
    if (scene() != nullptr)
        scene()->views().first()->viewport()->update();

}

void CGraphicsPenItem::calcVertexes()
{
    qint32 count = m_path.elementCount();

    if (count >= 2)
        calcVertexes(m_path.elementAt(count - 2), m_path.elementAt(count - 1));
}

QPainterPath CGraphicsPenItem::getHighLightPath()
{
//    QPainterPath path = m_path;
//    path.addPath(m_startPath);
//    path.addPath(m_endPath);
//    return path;
    return CGraphicsItem::getHighLightPath();
}

ELineType CGraphicsPenItem::getPenStartType() const
{
    return m_penStartType;
}

void CGraphicsPenItem::setPenStartType(const ELineType &penType)
{
    bool changed = m_penStartType != penType;
    m_penStartType = penType;

    if (changed) {
        updateShapeRecursion();
    }
    // 重新获取缓存数据
    if (isCached()) {
        resetCachePixmap();
    }
}

ELineType CGraphicsPenItem::getPenEndType() const
{
    return m_penEndType;
}

void CGraphicsPenItem::setPenEndType(const ELineType &penType)
{
    bool changed = m_penEndType != penType;
    m_penEndType = penType;

    if (changed) {
        updateShapeRecursion();
        updateHandlesGeometry();
    }
    // 重新获取缓存数据
    if (isCached()) {
        resetCachePixmap();
    }
}

QLineF CGraphicsPenItem::straightLine()
{
    return m_straightLine;
}

void CGraphicsPenItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    prepareGeometryChange();
    if (data.data.pPen != nullptr) {
        m_penStartType = data.data.pPen->start_type;
        m_penEndType = data.data.pPen->end_type;
        m_path = data.data.pPen->path;
    }
    loadHeadData(data.head);

    updateShape();
    updateHandlesGeometry();
}

QPainterPath CGraphicsPenItem::getSelfOrgShape() const
{
    QPainterPath path;
    path = m_path;
    if (m_isShiftPress) {
        path.lineTo(m_straightLine.p2());
    }

    path.addPath(m_startPath);
    path.addPath(m_endPath);

    return path;
}

QPainterPath CGraphicsPenItem::getPenStrokerShape() const
{
    return getGraphicsItemShapePathByOrg(selfOrgShape(), pen(), true, this->incLength(), false);
}
