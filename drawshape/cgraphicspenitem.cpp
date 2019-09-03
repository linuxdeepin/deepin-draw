#include "cgraphicspenitem.h"
#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QDebug>

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
{

}

CGraphicsPenItem::CGraphicsPenItem(const QPointF &startPoint, QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_prePoint(startPoint)
    , m_isShiftPress(false)
    , m_currentType(straight)
{
    initPen();
    m_drawPath.moveTo(startPoint);
    m_truePath = m_drawPath;

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
    return  qt_graphicsItem_shapeFromPath(m_truePath, pen());
}

QRectF CGraphicsPenItem::boundingRect() const
{
    QRectF rect = m_truePath.controlPointRect();

    return QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
                  rect.width() + pen().width(), rect.height() + pen().width());
}

QRectF CGraphicsPenItem::rect() const
{
    QRectF rect(m_truePath.controlPointRect());
    return rect.normalized();
}

void CGraphicsPenItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
//    QPointF local = mapFromScene(point);
//    QRectF rect = this->rect();

//    switch (dir) {
//    case CSizeHandleRect::Right:
//        if (local.x() - rect.left() > 0.1 ) {
//            rect.setRight(local.x());
//        }
//        break;
//    case CSizeHandleRect::RightTop:
//        if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
//            rect.setTopRight(local);
//        }
//        break;
//    case CSizeHandleRect::RightBottom:
//        if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
//            rect.setBottomRight(local);
//        }
//        break;
//    case CSizeHandleRect::LeftBottom:
//        if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
//            rect.setBottomLeft(local);
//        }
//        break;
//    case CSizeHandleRect::Bottom:
//        if (local.y() - rect.top() > 0.1 ) {
//            rect.setBottom(local.y());
//        }
//        break;
//    case CSizeHandleRect::LeftTop:
//        if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
//            rect.setTopLeft(local);
//        }
//        break;
//    case CSizeHandleRect::Left:
//        if (rect.right() - local.x() > 0.1 ) {
//            rect.setLeft(local.x());
//        }
//        break;
//    case CSizeHandleRect::Top:
//        if (local.y() - rect.bottom() < 0.1 ) {
//            rect.setTop(local.y());
//        }
//        break;
//    default:
//        break;
//    }

//    rect = rect.normalized();
//    prepareGeometryChange();

//    this->setRect(m_);
//    updateGeometry();
}

void CGraphicsPenItem::updatePenPath(const QPointF &endPoint, bool isShiftPress)
{
    m_isShiftPress = isShiftPress;
    prepareGeometryChange();
    if (isShiftPress) {
        m_straightLine.setP1(m_drawPath.currentPosition());
        m_straightLine.setP2(endPoint);
        m_truePath.lineTo(endPoint);
    } else {
        m_drawPath.lineTo(endPoint);
        m_truePath = m_drawPath;
        if (m_currentType == arrow) {
            calcVertexes(m_prePoint, endPoint);
            m_prePoint = endPoint;
            m_truePath.addPolygon(m_arrow);
        }
    }
    updateGeometry();
}

void CGraphicsPenItem::updateGeometry()
{
    const QRectF &geom = this->rect();

    const int w = SELECTION_HANDLE_SIZE;
    const int h = SELECTION_HANDLE_SIZE;

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;
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
            hndl->move(geom.x() + geom.width() / 2 - hndl->rect().width() / 2, geom.y() - 25 - hndl->rect().height() / 2);
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
    painter->setPen(pen());
    painter->drawPath(m_drawPath);

    if (m_isShiftPress) {
        painter->drawLine(m_straightLine);
    }

    if (m_currentType == arrow) {
        painter->setBrush(QBrush(pen().color()));
        painter->drawPolygon(m_arrow);
    }
}

void CGraphicsPenItem::initPen()
{
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Rotation; ++i) {
        CSizeHandleRect *shr = nullptr;
        if (i == CSizeHandleRect::Rotation) {
            QPixmap rotaImage(":/theme/resources/icon_rotate.svg");
            shr   = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), this, rotaImage);
        } else {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), this);
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
    m_arrow.clear();

    qreal x1, y1, x2, y2;

    qreal arrow_lenght_ = 10.0 + pen().width() * 10; //箭头的斜边长
    qreal arrow_degrees_ = qDegreesToRadians(30.0); //箭头的角度/2

    qreal angle = atan2 (currentPoint.y() - prePoint.y(), currentPoint.x() - prePoint.x()) + M_PI;

    x1 = currentPoint.x() + arrow_lenght_ * cos(angle - arrow_degrees_);

    y1 = currentPoint.y() + arrow_lenght_ * sin(angle - arrow_degrees_);

    x2 = currentPoint.x() + arrow_lenght_ * cos(angle + arrow_degrees_);

    y2 = currentPoint.y() + arrow_lenght_ * sin(angle + arrow_degrees_);

    m_arrow.push_back(QPointF(x1, y1));
    m_arrow.push_back(QPointF(x2, y2));
    m_arrow.push_back(currentPoint);

    //|y1-y2|<|x1-x2|，画水平线
    //|y1-y2|>|x1-x2|，画垂直线
    //|y1-y2|=|x1-x2|，画45度直线
}
