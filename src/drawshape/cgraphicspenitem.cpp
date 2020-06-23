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

#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QDebug>

const int SmoothMaxCount = 10;

// 绘制起始终点的最小矩形范围
const QSizeF minRectSize(10, 10);

//static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
//{
//    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
//    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
//    const qreal penWidthZero = qreal(0.00000001);

//    if (path == QPainterPath() || pen == Qt::NoPen)
//        return path;
//    QPainterPathStroker ps;
//    ps.setCapStyle(pen.capStyle());
//    if (pen.widthF() <= 0.0)
//        ps.setWidth(penWidthZero);
//    else
//        ps.setWidth(pen.widthF());
//    ps.setJoinStyle(pen.joinStyle());
//    ps.setMiterLimit(pen.miterLimit());
//    QPainterPath p = ps.createStroke(path);
//    p.addPath(path);
//    return p;
//}


CGraphicsPenItem::CGraphicsPenItem(QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isShiftPress(false)
    , m_isDrawing(false)
    , m_drawIndex(0)
{
    CGraphicsItem::initHandle();
}

CGraphicsPenItem::CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isShiftPress(false)
    , m_isDrawing(false)
    , m_drawIndex(0)
    , m_penStartType(noneLine)
    , m_penEndType(noneLine)

{
    CGraphicsItem::initHandle();
    m_path.moveTo(startPoint);
//    m_poitsPath.push_back(startPoint);
    m_smoothVector.push_back(startPoint);
}

CGraphicsPenItem::CGraphicsPenItem(const SGraphicsPenUnitData *data, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsItem(head, parent)
    , m_isShiftPress(false)
    , m_isDrawing(false)
    , m_drawIndex(0)
{
    CGraphicsItem::initHandle();

    prepareGeometryChange();
    m_penStartType = data->start_type;
    m_penEndType = data->end_type;
//    m_poitsPath = data->poitsVector;
//    m_arrow = data->arrow;
    m_path = data->path;
    //CGraphicsItem::updateGeometry();
    updateCoordinate();
}

CGraphicsPenItem::~CGraphicsPenItem()
{

}

int CGraphicsPenItem::type() const
{
    return PenType;
}

QPainterPath CGraphicsPenItem::shape() const
{
    QPainterPath path;
    path = m_path;
    if (m_isShiftPress) {
        path.lineTo(m_straightLine.p2());
    }

    path.addPath(m_startPath);
    path.addPath(m_endPath);

    path.closeSubpath();
    return  qt_graphicsItem_shapeFromPath(path, pen());
}

QRectF CGraphicsPenItem::boundingRect() const
{
//    QRectF rect;
//    QPainterPath path;
//    path = m_path;

//    if (m_isShiftPress) {
//        path.lineTo(m_straightLine.p2());
//    }
//    if (arrow == m_currentType) {
//        path.addPolygon(m_arrow);
//        rect = path.controlPointRect();
//    } else {
//        rect  = path.controlPointRect();
//    }

//    return QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
//                  rect.width() + pen().width(), rect.height() + pen().width());

    return shape().controlPointRect();

}

QRectF CGraphicsPenItem::rect() const
{
    QPainterPath path;
    path = m_path;

    if (m_isShiftPress) {
        path.lineTo(m_straightLine.p2());
    }

    return path.controlPointRect().normalized();
}

void CGraphicsPenItem::duplicate(CGraphicsItem *item)
{
    static_cast<CGraphicsPenItem *>(item)->setPen(this->pen());
    static_cast<CGraphicsPenItem *>(item)->setPenStartType(this->m_penStartType);
    static_cast<CGraphicsPenItem *>(item)->setPenEndType(this->m_penEndType);
    static_cast<CGraphicsPenItem *>(item)->setPath(this->m_path);
    static_cast<CGraphicsPenItem *>(item)->setPenStartpath(this->getPenStartpath());
    static_cast<CGraphicsPenItem *>(item)->setPenEndpath(this->getPenEndpath());

    CGraphicsItem::duplicate(item);
}

CGraphicsUnit CGraphicsPenItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsPenUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();

    unit.data.pPen = new SGraphicsPenUnitData();
    unit.data.pPen->start_type = this->m_penStartType;
    unit.data.pPen->end_type = this->m_penEndType;
    unit.data.pPen->path = this->m_path;
//    unit.data.pPen->arrow = this->m_arrow;
//    unit.data.pPen->poitsVector = this->m_poitsPath;

    return unit;
}

void CGraphicsPenItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    QPointF local = mapFromScene(point);
    QRectF rect = this->rect();
    qreal sx = 1.;
    qreal sy = 1.;
    qreal space = 0.1;
    QTransform transform;


    switch (dir) {
    case CSizeHandleRect::Right:
        if (local.x() - rect.left() > space) {
            sx =  (local.x() - rect.left()) / (rect.right() - rect.left());
//            qDebug() << "local.x=" << local.x() << "rect.left=" << rect.left() << "!!!=" << local.x() - rect.left()
//                     << "rect.right=" << rect.right() << "rect.left=" << rect.left() << "@@@=" << rect.right() - rect.left()
//                     << "rect=" << m_arrow.boundingRect();
            transform.translate(rect.left(), rect.top());
            transform.scale(sx, sy);
            transform.translate(-rect.left(), -rect.top());
        }
        break;
    case CSizeHandleRect::RightTop:
        if (local.x() - rect.left() > space && rect.bottom() - local.y() > space) {
            sx =  (local.x() - rect.left()) / (rect.right() - rect.left());
            sy = (local.y() - rect.bottom()) / (rect.top() - rect.bottom());

            transform.translate(rect.left(), rect.bottom());
            transform.scale(sx, sy);
            transform.translate(-rect.left(), -rect.bottom());
        }

        break;
    case CSizeHandleRect::RightBottom:
        if (local.x() - rect.left() > space && local.y() - rect.top() > space) {
            sx =  (local.x() - rect.left()) / (rect.right() - rect.left());
            sy = (local.y() - rect.top()) / (rect.bottom() - rect.top());

            transform.translate(rect.left(), rect.top());
            transform.scale(sx, sy);
            transform.translate(-rect.left(), -rect.top());
        }
        break;
    case CSizeHandleRect::LeftBottom:
        if (rect.right() - local.x() > space && local.y() - rect.top() > space) {
            sx =  (local.x() - rect.right()) / (rect.left() - rect.right());
            sy = (local.y() - rect.top()) / (rect.bottom() - rect.top());

            transform.translate(rect.right(), rect.top());
            transform.scale(sx, sy);
            transform.translate(-rect.right(), -rect.top());
        }
        break;
    case CSizeHandleRect::Bottom:
        if (local.y() - rect.top() > space) {
            sy = (local.y() - rect.top()) / (rect.bottom() - rect.top());

            transform.translate(rect.left(), rect.top());
            transform.scale(sx, sy);
            transform.translate(-rect.left(), -rect.top());
        }
        break;
    case CSizeHandleRect::LeftTop:
        if (rect.right() - local.x() > space && rect.bottom() - local.y() > space) {
            sx =  (local.x() - rect.right()) / (rect.left() - rect.right());
            sy = (local.y() - rect.bottom()) / (rect.top() - rect.bottom());

            transform.translate(rect.right(), rect.bottom());
            transform.scale(sx, sy);
            transform.translate(-rect.right(), -rect.bottom());
        }
        break;
    case CSizeHandleRect::Left:
        if (rect.right() - local.x() > space) {
            sx =  (local.x() - rect.right()) / (rect.left() - rect.right());
            transform.translate(rect.right(), rect.top());
            transform.scale(sx, sy);
            transform.translate(-rect.right(), -rect.top());
        }
        break;
    case CSizeHandleRect::Top:
        if (rect.bottom() - local.y() > space) {
            sy = (local.y() - rect.bottom()) / (rect.top() - rect.bottom());

            transform.translate(rect.left(), rect.bottom());
            transform.scale(sx, sy);
            transform.translate(-rect.left(), -rect.bottom());
        }
        break;
    default:
        break;
    }

    prepareGeometryChange();

    m_path = transform.map(m_path);

    calcVertexes();

//    if (m_currentType == arrow) {
    //m_arrow = transform.map(m_arrow);
//    }

    updateCoordinate();
}

void CGraphicsPenItem::updateCoordinate()
{
    QPointF pt1, pt2, delta;

    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(boundingRect().center());
    delta = pt1 - pt2;

    QPainterPath path;


    for (int i = 0; i < m_path.elementCount(); i++) {
        QPainterPath::Element element = m_path.elementAt(i);
        QPointF point = mapFromScene(mapToScene(QPointF(element.x, element.y)) + delta);
        if (i == 0) {
            path.moveTo(point);
        } else {
            path.lineTo(point);
        }
    }


    prepareGeometryChange();

    // 更新画笔路径
    m_path = path;

    // 更新起点和终点的路径
    drawStart();
    drawEnd();


    setTransform(transform().translate(delta.x(), delta.y()));
//    setTransformOriginPoint(m_path.controlPointRect().center());
    moveBy(-delta.x(), -delta.y());
    setTransform(transform().translate(-delta.x(), -delta.y()));

    updateGeometry();
}

void CGraphicsPenItem::drawComplete()
{
    if (m_isShiftPress) {
        m_isShiftPress = false;
        m_path.lineTo(m_straightLine.p2());
    }

    if (m_path.elementCount() > 5) {
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

        calcVertexes();
    }

    updateCoordinate();

    m_isStartWithLine = false;
    m_isEndWithLine = false;
}

void CGraphicsPenItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)

    resizeTo(dir, point);

}

void CGraphicsPenItem::resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                                   const double &xScale, const double &yScale,
                                   bool bShiftPress, bool bAltPress)
{
    QRectF rect = this->rect();
    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;
    QTransform transform;
    QPainterPath path;
    QPolygonF arrow;
    QPointF arrowOffset;
    if (!shiftKeyPress && !altKeyPress) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }
    //按住SHIFT等比拉伸
    else if ((shiftKeyPress && !altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }
    //中心拉伸
    else if ((!shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }
    //等比中心拉伸
    else if ((shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x + (element.x - rect.left()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y + (element.y - rect.top()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_path.elementCount(); i++) {
                QPainterPath::Element element = m_path.elementAt(i);
                QPointF point = mapFromScene(mapToScene(QPointF(element.x - (element.x - rect.right()) * xScale, element.y - (element.y - rect.bottom()) * yScale)));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_path.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }

    prepareGeometryChange();
    m_path = path;
    this->moveBy(offset.x(), offset.y());

    updateCoordinate();

    calcVertexes();

    updateGeometry();

}

void CGraphicsPenItem::resizeToMul_7(CSizeHandleRect::EDirection dir, QRectF pressRect, QRectF itemPressRect,
                                     const qreal &xScale, const qreal &yScale,
                                     bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(itemPressRect);
    prepareGeometryChange();
    pressRect = mapRectFromScene(pressRect);
    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;
    QTransform transform;
    QPainterPath path;
    QPolygonF arrow;
    QPointF arrowOffset;
    if (!shiftKeyPress && !altKeyPress) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.right() - (pressRect.right() - element.x) * xScale, element.y);
                point.setY(pressRect.bottom() - (pressRect.bottom() - element.y) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.bottom() - (pressRect.bottom() - element.y) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.bottom() - (pressRect.bottom() - element.y) * yScale);
                point.setX(pressRect.left() + (element.x - pressRect.left())*xScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.left() + (element.x - pressRect.left())*xScale, element.y);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.left() + (element.x - pressRect.left())*xScale, element.y);
                point.setY(pressRect.top() + (element.y - pressRect.top()) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.top() + (element.y - pressRect.top()) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.right() - (pressRect.right() - element.x) * xScale, element.y);
                point.setY(pressRect.top() + (element.y - pressRect.top()) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.right() - (pressRect.right() - element.x) * xScale, element.y);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }
    //按住SHIFT等比拉伸
    else if ((shiftKeyPress && !altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.bottom() - (pressRect.bottom() - element.y) * yScale);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.bottom() - (pressRect.bottom() - element.y) * yScale);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.bottom() - (pressRect.bottom() - element.y) * yScale);
                point.setX(pressRect.left() + (element.x - pressRect.left())*xScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.left() + (element.x - pressRect.left())*xScale, element.y);
                point.setY(element.y + (pressRect.top() + pressRect.height() / 2 - element.y) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.left() + (element.x - pressRect.left())*xScale, element.y);
                point.setY(pressRect.top() + (element.y - pressRect.top()) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, pressRect.top() + (element.y - pressRect.top()) * yScale);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.right() - (pressRect.right() - element.x) * xScale, element.y);
                point.setY(pressRect.top() + (element.y - pressRect.top()) * yScale);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(pressRect.right() - (pressRect.right() - element.x) * xScale, element.y);
                point.setY(element.y + (pressRect.top() + pressRect.height() / 2 - element.y) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }
    //中心拉伸
    else if ((!shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                point.setX(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale), element.y);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale), element.y);
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale), element.y);
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }
    //等比中心拉伸
    else if ((shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::LeftTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Top:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightTop:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Right:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::RightBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Bottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::LeftBottom:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x - (element.x - pressRect.left() - pressRect.width() / 2) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        case CSizeHandleRect::Left:
            for (int i = 0; i < m_pathBeforResize.elementCount(); i++) {
                QPainterPath::Element element = m_pathBeforResize.elementAt(i);
                QPointF point(element.x, element.y);
                point.setX(element.x + (pressRect.left() + pressRect.width() / 2 - element.x) * (1 - xScale));
                point.setY(element.y + (pressRect.bottom() - element.y - pressRect.height() / 2) * (1 - yScale));
                if (i == 0) {
                    path.moveTo(point);
                } else {
                    path.lineTo(point);
                }
                if (i == (m_pathBeforResize.elementCount() - 1)) {
                    arrowOffset.setX(point.x() - element.x);
                    arrowOffset.setY(point.y() - element.y);
                }
            }
            break;
        default:
            break;
        }
    }

    m_path = path;
    calcVertexes();
    updateGeometry();
}

void CGraphicsPenItem::updatePenPath(const QPointF &endPoint, bool isShiftPress)
{
    prepareGeometryChange();
    m_isShiftPress = isShiftPress;

    if (isShiftPress) {
        m_straightLine.setP1(m_path.currentPosition());
        m_straightLine.setP2(endPoint);
        calcVertexes(m_straightLine.p1(), m_straightLine.p2());
    } else {
        m_path.lineTo(endPoint);
        m_smoothVector.push_back(endPoint);
        if (m_smoothVector.count() > SmoothMaxCount) {
            m_smoothVector.removeFirst();
        }
        calcVertexes(m_path.elementAt(0), endPoint);
//        calcVertexes(m_smoothVector.first(), m_smoothVector.last());
    }

    updateGeometry();
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

void CGraphicsPenItem::drawStart()
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
        qreal radioWidth = this->pen().width() * 2;
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
        qreal radioWidth = this->pen().width() * 2;
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

void CGraphicsPenItem::drawEnd()
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
        qreal radioWidth = this->pen().width() * 2;
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
        qreal radioWidth = this->pen().width() * 2;
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

void CGraphicsPenItem::updateGeometry()
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

void CGraphicsPenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateGeometry();
    QPen pen = this->pen();
    pen.setJoinStyle(Qt::BevelJoin);


    beginCheckIns(painter);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setPen(pen);

    painter->setPen(this->pen().width() == 0 ? Qt::NoPen : this->pen());
    painter->drawPath(m_path);

    if (m_isShiftPress) {
        painter->drawLine(m_straightLine);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setBrush(Qt::NoBrush);
    if (this->pen().width()) {
        if (m_penStartType == soildArrow || m_penStartType == soildRing) {
            painter->setBrush(QBrush(QColor(this->pen().color())));
        }
    }
    painter->drawPath(m_startPath);

    painter->setBrush(Qt::NoBrush);
    if (this->pen().width()) {
        if (m_penEndType == soildArrow || m_penEndType == soildRing) {
            painter->setBrush(QBrush(QColor(this->pen().color())));
        }
    }
    painter->drawPath(m_endPath);

    endCheckIns(painter);

    if (this->getMutiSelect()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale());
//        if ( CManageViewSigleton::GetInstance()->getThemeType() == 1) {
//            pen.setColor(QColor(224, 224, 224));
//        } else {
//            pen.setColor(QColor(69, 69, 69));
//        }
        pen.setColor(QColor(224, 224, 224));
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
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
}

void CGraphicsPenItem::updatePenType(const ELineType &startType, const ELineType &endType)
{
    prepareGeometryChange();

    m_penStartType = startType;
    m_penEndType = endType;
    QPointF startPoint;
    QPointF endPoint;
    int totalCount = m_path.elementCount();
    startPoint.setX(m_path.elementAt(totalCount - 2).x);
    startPoint.setY(m_path.elementAt(totalCount - 2).y);
    endPoint.setX(m_path.elementAt(totalCount  - 1).x);
    endPoint.setY(m_path.elementAt(totalCount  - 1).y);
    calcVertexes(startPoint, endPoint);
    updateCoordinate();
}

void CGraphicsPenItem::setPixmap()
{
    if (nullptr != scene()) {
        auto curScene = static_cast<CDrawScene *>(scene());
        QRect rect = curScene->sceneRect().toRect();
        m_tmpPix = QPixmap(rect.width(), rect.height());
        QPainter painterd(&m_tmpPix);
        painterd.setRenderHint(QPainter::Antialiasing);
        painterd.setRenderHint(QPainter::SmoothPixmapTransform);
        curScene->render(&painterd);
    }
}

void CGraphicsPenItem::setDrawFlag(bool flag)
{
    m_isDrawing = flag;
}

void CGraphicsPenItem::savePathBeforResize(QPainterPath path)
{
    m_pathBeforResize = path;
}

void CGraphicsPenItem::initHandle()
{
    clearHandle();
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Rotation; ++i) {
        CSizeHandleRect *shr = nullptr;
        if (i == CSizeHandleRect::Rotation) {
            shr   = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), QString(":/theme/light/images/mouse_style/icon_rotate.svg"));

        } else {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        }
        m_handles.push_back(shr);

    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsPenItem::calcVertexes(const QPointF &prePoint, const QPointF &currentPoint)
{
    if (prePoint == currentPoint) {
        return;
    }

    drawStart();
    drawEnd();

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
    QPainterPath path = m_path;
    path.addPath(m_startPath);
    path.addPath(m_endPath);
    return path;
}

ELineType CGraphicsPenItem::getPenStartType() const
{
    return m_penStartType;
}

void CGraphicsPenItem::setPenStartType(const ELineType &penType)
{
    m_penStartType = penType;
    calcVertexes();
    updateGeometry();
}

ELineType CGraphicsPenItem::getPenEndType() const
{
    return m_penEndType;
}

void CGraphicsPenItem::setPenEndType(const ELineType &penType)
{
    m_penEndType = penType;
    calcVertexes();
    updateGeometry();
}
