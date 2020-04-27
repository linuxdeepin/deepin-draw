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
#include <QtMath>
#include <QDebug>
#include <QApplication>

const int SmoothMaxCount = 10;

CGraphicsPenItem *CGraphicsPenItem::s_curPenItem = nullptr;

#include <QtMath>

static QPainterPath generateSmoothCurve(QList<double> points, bool closed, double tension, int numberOfSegments = 16)
{
    QList<double> ps(points); // clone array so we don't change the original points
    QList<double> result; // generated smooth curve coordinates
    double x, y;
    double t1x, t2x, t1y, t2y;
    double c1, c2, c3, c4;
    double st;

// The algorithm require a previous and next point to the actual point array.
// Check if we will draw closed or open curve.
// If closed, copy end points to beginning and first points to end
// If open, duplicate first points to befinning, end points to end
    if (closed) {
        ps.prepend(points[points.length() - 1]);
        ps.prepend(points[points.length() - 2]);
        ps.prepend(points[points.length() - 1]);
        ps.prepend(points[points.length() - 2]);
        ps.append(points[0]);
        ps.append(points[1]);
    } else {
        ps.prepend(points[1]); // copy 1st point and insert at beginning
        ps.prepend(points[0]);
        ps.append(points[points.length() - 2]); // copy last point and append
        ps.append(points[points.length() - 1]);
    }

// 1. loop goes through point array
// 2. loop goes through each segment between the 2 points + 1e point before and after
    for (int i = 2; i < (ps.length() - 4); i += 2) {
// calculate tension vectors
        t1x = (ps[i + 2] - ps[i - 2]) * tension;
        t2x = (ps[i + 4] - ps[i - 0]) * tension;
        t1y = (ps[i + 3] - ps[i - 1]) * tension;
        t2y = (ps[i + 5] - ps[i + 1]) * tension;

        for (int t = 0; t <= numberOfSegments; t++) {
// calculate step
            st = (double)t / (double)numberOfSegments;

// calculate cardinals
            c1 = 2 * qPow(st, 3) - 3 * qPow(st, 2) + 1;
            c2 = -2 * qPow(st, 3) + 3 * qPow(st, 2);
            c3 = qPow(st, 3) - 2 * qPow(st, 2) + st;
            c4 = qPow(st, 3) - qPow(st, 2);

// calculate x and y cords with common control vectors
            x = c1 * ps[i] + c2 * ps[i + 2] + c3 * t1x + c4 * t2x;
            y = c1 * ps[i + 1] + c2 * ps[i + 3] + c3 * t1y + c4 * t2y;

//store points in array
            result << x << y;
        }
    }

// 使用的平滑曲线的坐标创建 QPainterPath
    QPainterPath path;
    path.moveTo(result[0], result[1]);
    for (int i = 2; i < result.length() - 2; i += 2) {
        path.lineTo(result[i + 0], result[i + 1]);
    }

    if (closed) {
        path.closeSubpath();
    }

    return path;
}

static QPainterPath generateSmoothCurve(QList<QPointF> points, bool closed, double tension, int numberOfSegments = 16)
{
    QList<double> ps;

    foreach (QPointF p, points) {
        ps << p.x() << p.y();
    }

    return generateSmoothCurve(ps, closed, tension, numberOfSegments);
}

static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (path == QPainterPath() || pen == Qt::NoPen)
        return path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;

}


CGraphicsPenItem::CGraphicsPenItem(QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isShiftPress(false)
    , m_isDrawing(false)
    , m_drawIndex(0)
{
    initPen();
}

CGraphicsPenItem::CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isShiftPress(false)
    , m_currentType(straight)
    , m_isDrawing(false)
    , m_drawIndex(0)

{
    initPen();
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
    initPen();

    prepareGeometryChange();
    m_currentType = static_cast<EPenType>(data->penType);
//    m_poitsPath = data->poitsVector;
    m_arrow = data->arrow;
    m_path = data->path;
    updateGeometry();
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

    if (arrow == m_currentType) {
        path.addPolygon(m_arrow);
    }
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

    if (arrow == m_currentType) {
//        path.addPolygon(m_arrow);
        return path.controlPointRect().normalized();
    } else {
        return path.controlPointRect().normalized();
    }
}

void CGraphicsPenItem::duplicate(CGraphicsItem *item)
{
    static_cast<CGraphicsPenItem *>(item)->setCurrentType(this->m_currentType);
    static_cast<CGraphicsPenItem *>(item)->setPath(this->m_path);
    static_cast<CGraphicsPenItem *>(item)->setArrow(this->m_arrow);
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
    unit.data.pPen->penType = this->m_currentType;
    unit.data.pPen->path = this->m_path;
    unit.data.pPen->arrow = this->m_arrow;
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
    ///更新画笔路径
    m_path = path;
    ///更新箭头

//    if (m_currentType == arrow) {
    for (QPointF &point : m_arrow) {
        point = mapFromScene(mapToScene(point) + delta);
    }
//    }

    setTransform(transform().translate(delta.x(), delta.y()));
//    setTransformOriginPoint(m_path.controlPointRect().center());
    moveBy(-delta.x(), -delta.y());
    setTransform(transform().translate(-delta.x(), -delta.y()));

    updateGeometry();
}

void CGraphicsPenItem::drawComplete()
{
    //qDebug() << "---------------- drawComplete0 count = " << m_path.elementCount() << boundingRect().size();
    if (m_isShiftPress) {
        m_isShiftPress = false;
//        m_poitsPath.push_back(m_straightLine.p2());
        m_path.lineTo(m_straightLine.p2());
    }

//    if (m_path.elementCount() > 3) {

//        QPainterPath vout;


//        for (int i = 0; i < m_path.elementCount() - 3; i += 3) {
//            QPainterPath::Element p0 = m_path.elementAt(i);
//            QPainterPath::Element p1 = m_path.elementAt(i + 1);
//            QPainterPath::Element p2 = m_path.elementAt(i + 2);
//            QPainterPath::Element p3 = m_path.elementAt(i + 3);


//            if (0 == i) {
//                QPointF dot1 = GetThreeBezierValue(p0, p1, p2, p3, 0.);
//                vout.moveTo(dot1);
//            }
//            QPointF dot2 = GetThreeBezierValue(p0, p1, p2, p3, 1 / 3.0);
//            QPointF dot3 = GetThreeBezierValue(p0, p1, p2, p3, 2 / 3.0);
//            QPointF dot4 = GetThreeBezierValue(p0, p1, p2, p3, 1.0);

//            vout.lineTo(dot2);
//            vout.lineTo(dot3);
//            vout.lineTo(dot4);

//        }

//        m_path = vout;

//    }


    if (m_path.elementCount() > 5) {

//        QPainterPath vout;


//        for (int i = 0; i < m_path.elementCount() - 5; i += 5) {
//            QPainterPath::Element p0 = m_path.elementAt(i);
//            QPainterPath::Element p1 = m_path.elementAt(i + 1);
//            QPainterPath::Element p2 = m_path.elementAt(i + 2);
//            QPainterPath::Element p3 = m_path.elementAt(i + 3);
//            QPainterPath::Element p4 = m_path.elementAt(i + 4);
//            QPainterPath::Element p5 = m_path.elementAt(i + 5);


//            if (0 == i) {
//                QPointF dot1 = GetBezierValue(p0, p1, p2, p3, p4, p5, 0.);
//                vout.moveTo(dot1);
//            }
//            QPointF dot2 = GetBezierValue(p0, p1, p2, p3, p4, p5,  1 / 5.0);
//            QPointF dot3 = GetBezierValue(p0, p1, p2, p3, p4, p5,  2 / 5.0);
//            QPointF dot4 = GetBezierValue(p0, p1, p2, p3, p4, p5,  3 / 5.0);
//            QPointF dot5 = GetBezierValue(p0, p1, p2, p3, p4, p5,  4 / 5.0);
//            QPointF dot6 = GetBezierValue(p0, p1, p2, p3, p4, p5,  1);


//            vout.lineTo(dot2);
//            vout.lineTo(dot3);
//            vout.lineTo(dot4);
//            vout.lineTo(dot5);
//            vout.lineTo(dot6);

//        }
//        prepareGeometryChange();
//        m_path = vout;

        calcVertexes();
    }

    updateCoordinate();

    //qDebug() << "---------------- drawComplete1 count = " << m_path.elementCount() << boundingRect().size();

}

void CGraphicsPenItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)

    resizeTo(dir, point);

}

#include <QTime>
void CGraphicsPenItem::updatePenPath(const QPointF &endPoint, bool isShiftPress)
{
    if (m_recordPrePos == endPoint)
        return;

    /*prepareGeometryChange();

    if (isShiftPress) {
        m_straightLine.setP1(m_path.currentPosition());
        m_straightLine.setP2(endPoint);

        //if (m_currentType == arrow) {
            calcVertexes(m_straightLine.p1(), m_straightLine.p2());
        //}
    } else {
        m_path.lineTo(endPoint);

        ///
        m_smoothVector.push_back(endPoint);
        if (m_smoothVector.count() > SmoothMaxCount) {
            m_smoothVector.removeFirst();
        }
        ///

    //        if (m_currentType == arrow) {
        calcVertexes(m_smoothVector.first(), m_smoothVector.last());
    //        }
    }*/

    prepareGeometryChange();
    m_isShiftPress = isShiftPress;

    QPainterPath   thisTimePath;
    if (isShiftPress) {
        m_straightLine.setP1(m_path.currentPosition());
        m_straightLine.setP2(endPoint);

        //if (m_currentType == arrow) {
        calcVertexes(m_straightLine.p1(), m_straightLine.p2());
        //}
    } else {
//        int oldCount      = m_path.elementCount();
//        QPointF   lastPos = oldCount == 0 ? endPoint : m_path.elementAt(m_path.elementCount() - 1);

//        m_path.lineTo(endPoint);
//        m_smoothVector.push_back(endPoint);
//        if (m_smoothVector.count() > SmoothMaxCount) {
//            m_smoothVector.removeFirst();
//        }
//        calcVertexes(m_smoothVector.first(), m_smoothVector.last());
//        m_points.append(endPoint);

//        if (m_points.size() > 1)
//            m_path   = generateSmoothCurve(m_points, false, 0.8, 16);

//        pathTemp.moveTo(lastPos);
//        for (int i = oldCount; i < m_path.elementCount(); ++i) {
//            pathTemp.lineTo(m_path.elementAt(i));
//        }

        //qDebug() << "come one point = " << endPoint;
        bool isFirstPoint = m_points.isEmpty();

        m_points.append(endPoint);
        m_smoothVector.push_back(endPoint);
        if (m_smoothVector.count() > SmoothMaxCount) {
            m_smoothVector.removeFirst();
        }
        calcVertexes(m_smoothVector.first(), m_smoothVector.last());

        if (!isFirstPoint) {
//            QTime timer;
//            timer.start();
//            qDebug() << "generateSmoothCurve begin --------";

            //证明两个点以上了要进行插值优化
            QPainterPath newPath      = generateSmoothCurve(m_points, false, 0.5, 8);
            thisTimePath.moveTo(m_path.elementAt(m_path.elementCount() - 1));
            //int newCreatPosCount = newPath.elementCount() - m_path.elementCount();
            //qDebug() << "------------- newCreatPosCount ====  " << newCreatPosCount;
            for (int i = m_path.elementCount(); i < newPath.elementCount(); ++i) {
                thisTimePath.lineTo(newPath.elementAt(i));
                m_path.lineTo(newPath.elementAt(i));
            }
            //qDebug() << "generateSmoothCurve ends   ms =   " << timer.elapsed();
        } else {
            //初始化第一个点
            m_path.moveTo(endPoint);
        }
    }

    if (thisTimePath.elementCount() >= 1) {
//        QTime timer;
//        timer.start();
//        qDebug() << "updatePenPath paint begin --------";

        QPainter pp(&m_tmpPix);
        pp.setRenderHint(QPainter::Antialiasing);
        pp.setRenderHint(QPainter::SmoothPixmapTransform);
        QPen p(pen());
        p.setWidthF(1.0);
        pp.setPen(p);
        pp.drawPath(thisTimePath);

        m_drawIndex = thisTimePath.elementCount() - 1;

        //qDebug() << "updatePenPath paint ens   ms = " << timer.elapsed();
    }

    m_recordPrePos = endPoint;

    updateGeometry();


    //qDebug() << "---------------- mouse count = " << m_path.elementCount() << boundingRect().size();
}



//qreal CGraphicsPenItem::GetThreeBezierValue(qreal p0, qreal p1, qreal p2, qreal p3, qreal t)
//{
//    return pow(1 - t, 3.0) * p0 + 3 * t * (1 - t) * (1 - t) * p1 + 3 * t * t * (1 - t) * p2 + t * t * t * p3;
//}

qreal CGraphicsPenItem::GetBezierValue(qreal p0, qreal p1, qreal p2, qreal p3, qreal p4, qreal p5, qreal t)
{

    return pow(1 - t, 5.0) * p0 + 5 * pow(1 - t, 4.0) * t * p1 + 10 * pow(1 - t, 3.0) * pow(t, 2.0) * p2 + 10 * pow(1 - t, 2.0) * pow(t, 3.0) * p3 + 5 * pow(1 - t, 1.0) * pow(t, 4.0) * p4 + pow(t, 5.0) * p5;
}

//QPointF CGraphicsPenItem::GetThreeBezierValue(QPainterPath::Element p0, QPainterPath::Element p1, QPainterPath::Element p2, QPainterPath::Element p3, qreal t)
//{
//    QPointF dot;
//    dot = QPointF(GetThreeBezierValue(p0.x, p1.x, p2.x, p3.x, t), GetThreeBezierValue(p0.y, p1.y, p2.y, p3.y, t));
//    return dot;
//}

QPointF CGraphicsPenItem::GetBezierValue(QPainterPath::Element p0, QPainterPath::Element p1, QPainterPath::Element p2, QPainterPath::Element p3, QPainterPath::Element p4, QPainterPath::Element p5, qreal t)
{
    QPointF dot;
    dot = QPointF(GetBezierValue(p0.x, p1.x, p2.x, p3.x, p4.x, p5.x, t), GetBezierValue(p0.y, p1.y, p2.y, p3.y,  p4.y, p5.y, t));
    return dot;
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
#include <QCursor>
#include <QTime>
void CGraphicsPenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    updateGeometry();
    QPen pen = this->pen();
    pen.setJoinStyle(Qt::BevelJoin);

    if (/*m_isDrawing*/s_curPenItem == this) {
        //如果正在绘图，就在辅助画布上绘制
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
//        QTime timer;
//        timer.start();
//        qDebug() << "timer begin --------";
        painter->drawPixmap(0, 0, m_tmpPix);
        //qDebug() << "timer ens   ms = " << timer.elapsed();
    } else {
        if (s_curPenItem == nullptr) {
            //qDebug() << "------------------------s_curPenItem = " << s_curPenItem;
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setRenderHint(QPainter::SmoothPixmapTransform);
            painter->setPen(pen);
            painter->drawPath(m_path);
        }
    }

    if (s_curPenItem != nullptr)
        return;

    if (m_isShiftPress) {
        painter->drawLine(m_straightLine);
    }

    if (m_currentType == arrow) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(this->pen().color()));
        painter->drawPolygon(m_arrow);
    }

    if (this->isSelected()) {
        QPen pen;
        pen.setWidthF(1 / CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale());
        if ( CManageViewSigleton::GetInstance()->getThemeType() == 1) {
            pen.setColor(QColor(224, 224, 224));
        } else {
            pen.setColor(QColor(69, 69, 69));
        }
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
    }
}


void CGraphicsPenItem::setArrow(const QPolygonF &arrow)
{
    m_arrow = arrow;
}

QPolygonF CGraphicsPenItem::getArrow() const
{
    return m_arrow;
}

QPainterPath CGraphicsPenItem::getPath() const
{
    return m_path;
}

void CGraphicsPenItem::setPath(const QPainterPath &path)
{
    m_path = path;
    qDebug() << "CGraphicsPenItem::setPath count = " << m_path.elementCount();
}

EPenType CGraphicsPenItem::currentType() const
{
    return m_currentType;
}

void CGraphicsPenItem::setCurrentType(const EPenType &currentType)
{
    m_currentType = currentType;
}

void CGraphicsPenItem::updatePenType(const EPenType &currentType)
{
    prepareGeometryChange();
    m_currentType = currentType;
//    if (currentType == arrow) {
//        QPointF startPoint;
//        QPointF endPoint;
//        int totalCount = m_path.elementCount();
//        startPoint.setX(m_path.elementAt(totalCount - 2).x);
//        startPoint.setY(m_path.elementAt(totalCount - 2).y);
//        endPoint.setX(m_path.elementAt(totalCount  - 1).x);
//        endPoint.setY(m_path.elementAt(totalCount  - 1).y);
//        calcVertexes(startPoint, endPoint);
//        updateCoordinate();
//    } else if (currentType == straight) {

    //    }
}

void CGraphicsPenItem::setPixmap()
{
//    QTime timer;
//    timer.start();
//    qDebug() << "creat pixmap begin --------";

//    //qDebug() << "---------------- beginMove count = " << m_path.elementCount();
//    //QRect rect = CDrawScene::GetInstance()->sceneRect().toRect();
//    QRect rect = CDrawScene::GetInstance()->sceneRect().toRect();
//    //qDebug() << "creat map = -----------------------------------";

////    QRect viewInSceneRect =  QRect(CDrawScene::GetInstance()->views().first()->mapToScene(QPoint(0, 0)).toPoint(),
////                                   QSize(CDrawScene::GetInstance()->views().first()->viewport()->size()));

////    viewInSceneRect.adjust(-10, -10, 10, 10);

//    m_tmpPix = QPixmap(rect.width(), rect.height());
//    QPainter painterd(&m_tmpPix);
//    painterd.setRenderHint(QPainter::Antialiasing);
//    //painterd.setRenderHint(QPainter::SmoothPixmapTransform);
//    CDrawScene::GetInstance()->render(&painterd);

    m_tmpPix = CDrawScene::GetInstance()->scenPixMap();
    //qDebug() << "creat pixmap end ms = " << timer.elapsed();
}

void CGraphicsPenItem::setDrawFlag(bool flag)
{
    m_isDrawing = flag;
}

void CGraphicsPenItem::initPen()
{
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
    /*if (prePoint == currentPoint) {
        return;
    }

    m_arrow.clear();

    qreal x1, y1, x2, y2;

    qreal arrow_lenght_ = 10.0 + pen().width() * 3; //箭头的斜边长
    qreal arrow_degrees_ = qDegreesToRadians(25.0); //箭头的角度/2

    qreal angle = atan2 (currentPoint.y() - prePoint.y(), currentPoint.x() - prePoint.x()) + M_PI;

    x1 = currentPoint.x() + arrow_lenght_ * cos(angle - arrow_degrees_);

    y1 = currentPoint.y() + arrow_lenght_ * sin(angle - arrow_degrees_);

    x2 = currentPoint.x() + arrow_lenght_ * cos(angle + arrow_degrees_);

    y2 = currentPoint.y() + arrow_lenght_ * sin(angle + arrow_degrees_);

    m_arrow.push_back(QPointF(x1, y1));
    m_arrow.push_back(QPointF(x2, y2));
    m_arrow.push_back(currentPoint);*/

//    QPointF prePoint = prePoint;
//    QPointF currentPoint = currentPoint;
//    m_point4 = prePoint;
    if (prePoint == currentPoint) {
        return;
    }

    m_arrow.clear();

    QLineF line(prePoint, currentPoint);

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
//    QPointF diffV = p1 - m_line.p2();
//    p1 -= diffV;
//    p2 -= diffV;
//    p3 -= diffV;

//    m_point4 = (p2 + p3) / 2;
    m_arrow.push_back(p1);
    m_arrow.push_back(p2);
    m_arrow.push_back(p3);
}

void CGraphicsPenItem::calcVertexes()
{
    qint32 count = m_path.elementCount();

    calcVertexes(m_path.elementAt(count - 2), m_path.elementAt(count - 1));
}
