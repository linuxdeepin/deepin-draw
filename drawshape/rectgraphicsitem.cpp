#include "rectgraphicsitem.h"
#include <QDebug>


RectGraphicsItem::RectGraphicsItem(const QRect &rect, QGraphicsItem *parent)
    : BaseGraphicsItem(parent)
{
    m_width = rect.width();
    m_height = rect.height();
    m_initialRect = rect;

    m_localRect = m_initialRect;
    m_localRect = rect;
    m_originPoint = QPointF(0, 0);

    updatehandles();

}

QRectF RectGraphicsItem::boundingRect() const
{
    return QRectF(m_localRect.x() - pen().widthF() / 2,
                  m_localRect.y() - pen().widthF() / 2,
                  m_localRect.width() + pen().widthF(),
                  m_localRect.height() + pen().widthF());
}

QPainterPath RectGraphicsItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());

    return path;
}

void RectGraphicsItem::stretch(int handle, double sx, double sy, const QPointF &origin)
{
    QTransform trans  ;
    switch (handle) {
    case Right:
    case Left:
        sy = 1;
        break;
    case Top:
    case Bottom:
        sx = 1;
        break;
    default:
        break;
    }

    opposite_ = origin;

    trans.translate(origin.x(), origin.y());
    trans.scale(sx, sy);
    trans.translate(-origin.x(), -origin.y());

    prepareGeometryChange();
    m_localRect = trans.mapRect(m_initialRect);

    m_width = m_localRect.width();
    m_height = m_localRect.height();
    updatehandles();

//    QRectF delta = m_localRect;

//    switch (handle) {
//    case LeftTop:
//        delta.setTopLeft(origin.toPoint());

//        break;
//    case Top:
//        delta.setTop(origin.y());
//        break;

//    case RightTop:
//        delta.setTopRight(origin.toPoint());
//        break;

//    case Left:
//        delta.setLeft(origin.x());
//        break;
//    case Right:
//        delta.setRight(origin.x());
//        break;

//    case LeftBottom:
//        delta.setBottomLeft(origin.toPoint());
//        break;
//    case Bottom:
//        delta.setBottom(origin.y());
//        break;

//    case RightBottom:
//        delta.setBottomRight(origin.toPoint());
//        break;

//    default:
//        break;
//    }

//    prepareGeometryChange();
//    m_width = delta.width();
//    m_height = delta.height();
//    m_localRect = delta;
//    updatehandles();
}

QRectF RectGraphicsItem::rect() const
{
    return m_localRect;
}

void RectGraphicsItem::updateCoordinate()
{

    QPointF pt1, pt2, delta;

    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(m_localRect.center());
    delta = pt1 - pt2;

    if (!parentItem() ) {
        prepareGeometryChange();
        m_localRect = QRectF(-m_width / 2, -m_height / 2, m_width, m_height);
        m_width = m_localRect.width();
        m_height = m_localRect.height();
        setTransform(transform().translate(delta.x(), delta.y()));
        setTransformOriginPoint(m_localRect.center());
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        opposite_ = QPointF(0, 0);
        updatehandles();
    }
    m_initialRect = m_localRect;
}

void RectGraphicsItem::move(const QPointF &point)
{
    moveBy(point.x(), point.y());
}

QGraphicsItem *RectGraphicsItem::duplicate() const
{
    RectGraphicsItem *item = new RectGraphicsItem( rect().toRect());
    item->m_width = width();
    item->m_height = height();
    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue() + 0.1);
    item->updateCoordinate();
    return item;
}


void RectGraphicsItem::updatehandles()
{
    BaseGraphicsItem::updatehandles();
}

static QRectF RecalcBounds(const QPolygonF  &pts)
{
    QRectF bounds(pts[0], QSize(0, 0));
    for (int i = 1; i < pts.count(); ++i) {
        if (pts[i].x() < bounds.left())
            bounds.setLeft(pts[i].x());
        if (pts[i].x() > bounds.right())
            bounds.setRight(pts[i].x());
        if (pts[i].y() < bounds.top())
            bounds.setTop(pts[i].y());
        if (pts[i].y() > bounds.bottom())
            bounds.setBottom (pts[i].y());
    }
    bounds = bounds.normalized();
    return bounds;
}

void RectGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)

    painter->setPen(pen());
    painter->setBrush(Qt::NoBrush);


    painter->drawRect(rect().toRect());

    painter->setPen(Qt::blue);
    painter->drawLine(QLine(QPoint(opposite_.x() - 6, opposite_.y()), QPoint(opposite_.x() + 6, opposite_.y())));
    painter->drawLine(QLine(QPoint(opposite_.x(), opposite_.y() - 6), QPoint(opposite_.x(), opposite_.y() + 6)));


    if (option->state & QStyle::State_Selected) {
        qt_graphicsItem_highlightSelected(this, painter, option);
    }
    /*

       QPolygonF pts;
       pts<<m_localRect.topLeft()<<m_localRect.topRight()<<m_localRect.bottomRight()<<m_localRect.bottomLeft();
       pts = mapToScene(pts);
       QRectF bound = RecalcBounds(pts);


       qDebug()<<m_localRect<<bound;
        pts.clear();
       pts<<bound.topLeft()<<bound.topRight()<<bound.bottomRight()<<bound.bottomLeft();
       pts = mapFromScene(pts);
       if ( scene() ){
       painter->save();
       painter->setPen(Qt::blue);
       painter->setBrush(Qt::NoBrush);
       painter->drawPolygon(pts);
       painter->restore();
       }
    */
}
