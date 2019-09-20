#include "cgraphicspenitem.h"
#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QDebug>


const int AverageCount = 10;

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

CGraphicsPenItem::CGraphicsPenItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
    , m_prePoint(QPointF(rect.x(), rect.y()))
    , m_isShiftPress(false)
    , m_isDrawCompelet(false)
    , m_currentType(straight)
{
    m_drawPath.moveTo(rect.x(), rect.y());
    m_truePath = m_drawPath;
    m_prePointList.push_back(QPointF(rect.x(), rect.y()));
}

CGraphicsPenItem::~CGraphicsPenItem()
{

}

int CGraphicsPenItem::type() const
{
    return PenType;
}


QRectF CGraphicsPenItem::boundingRect() const
{
    QRectF rect = this->rect();

    return rect;

//    return  QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
//                   rect.width() + pen().width(), rect.height() + pen().width());;
}

void CGraphicsPenItem::updatePenPath(const QPointF &endPoint, bool isShiftPress)
{
    prepareGeometryChange();

    m_isShiftPress = isShiftPress;

    if (isShiftPress) {
        m_straightLine.setP1(m_drawPath.currentPosition());
        m_straightLine.setP2(endPoint);
        m_truePath.lineTo(endPoint);

        if (m_currentType == arrow) {
            calcVertexes(m_straightLine.p1(), m_straightLine.p2());

            m_prePoint = endPoint;
            m_truePath.addPolygon(m_arrow);
        }

    } else {
        m_drawPath.lineTo(endPoint);
        m_truePath = m_drawPath;

        if (m_currentType == arrow ) {

            m_prePointList.push_back(endPoint);
            if (m_prePointList.count() >= AverageCount) {
                m_prePointList.removeFirst();
            }

            m_prePoint = m_prePointList.first();

            calcVertexes(m_prePoint, endPoint);
//            m_prePoint = endPoint;
            m_truePath.addPolygon(m_arrow);
        }

    }

    QRectF rect = m_truePath.controlPointRect();

    setRect(QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
                   rect.width() + pen().width(), rect.height() + pen().width()));
//    setRect(rect);
}

void CGraphicsPenItem::changeToPixMap()
{
    QRectF rect = this->rect();

    prepareGeometryChange();

    QPixmap tmp(rect.width(), rect.height());

    tmp.fill(Qt::transparent);

    QPainter painter(&tmp);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setPen(pen());

    painter.translate(-rect.x(), -rect.y());

    painter.drawPath(m_drawPath);

    if (m_isShiftPress) {
        painter.drawLine(m_straightLine);
    }

    if (m_currentType == arrow) {
        painter.setBrush(QBrush(pen().color()));
        painter.drawPolygon(m_arrow);
    }

    m_pixMap = tmp;

    m_isDrawCompelet = true;

}

CGraphicsItem *CGraphicsPenItem::duplicate() const
{
    if (!m_isDrawCompelet) {
        return nullptr;
    }

    CGraphicsPenItem *item = new CGraphicsPenItem(rect());

//    item->setDrawPath(m_drawPath);
//    item->setTruePath(m_truePath);
//    item->setArrow(m_arrow);
//    item->setPrePoint(m_prePoint);
//    item->setStraightLine(m_straightLine);
    item->setIsDrawCompelet(m_isDrawCompelet);
    item->setCurrentType(m_currentType);
    item->setPixMap(m_pixMap);

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



void CGraphicsPenItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (!m_isDrawCompelet) {
        painter->setPen(pen());
        painter->drawPath(m_drawPath);
        if (m_isShiftPress) {
            painter->drawLine(m_straightLine);
        }
        if (m_currentType == arrow) {
            painter->setBrush(QBrush(pen().color()));
            painter->drawPolygon(m_arrow);
        }
    } else {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(rect(), m_pixMap, m_pixMap.rect());
    }
}

void CGraphicsPenItem::setDrawPath(const QPainterPath &drawPath)
{
    m_drawPath = drawPath;
}

void CGraphicsPenItem::setTruePath(const QPainterPath &truePath)
{
    m_truePath = truePath;
}

void CGraphicsPenItem::setArrow(const QPolygonF &arrow)
{
    m_arrow = arrow;
}

void CGraphicsPenItem::setPrePoint(const QPointF &prePoint)
{
    m_prePoint = prePoint;
}

void CGraphicsPenItem::setStraightLine(const QLineF &straightLine)
{
    m_straightLine = straightLine;
}

void CGraphicsPenItem::setIsDrawCompelet(bool isDrawCompelet)
{
    m_isDrawCompelet = isDrawCompelet;
}

void CGraphicsPenItem::setPixMap(const QPixmap &pixMap)
{
    m_pixMap = pixMap;
}

EPenType CGraphicsPenItem::currentType() const
{
    return m_currentType;
}

void CGraphicsPenItem::setCurrentType(const EPenType &currentType)
{
    m_currentType = currentType;
}


void CGraphicsPenItem::calcVertexes(const QPointF &prePoint, const QPointF &currentPoint)
{
    if (prePoint == currentPoint) {
        return;
    }

    m_arrow.clear();

    qreal x1, y1, x2, y2;

    qreal arrow_lenght_ = 10.0 + pen().width() * 5; //箭头的斜边长
    qreal arrow_degrees_ = qDegreesToRadians(30.0); //箭头的角度/2

    qreal angle = atan2 (currentPoint.y() - prePoint.y(), currentPoint.x() - prePoint.x()) + M_PI;

    x1 = currentPoint.x() + arrow_lenght_ * cos(angle - arrow_degrees_);

    y1 = currentPoint.y() + arrow_lenght_ * sin(angle - arrow_degrees_);

    x2 = currentPoint.x() + arrow_lenght_ * cos(angle + arrow_degrees_);

    y2 = currentPoint.y() + arrow_lenght_ * sin(angle + arrow_degrees_);

    m_arrow.push_back(QPointF(x1, y1));
    m_arrow.push_back(QPointF(x2, y2));
    m_arrow.push_back(currentPoint);
}
