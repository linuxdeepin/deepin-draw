#include "cgraphicslineitem.h"
#include <QPen>
#include <QPainter>
#include <QPointF>

static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
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

CGraphicsLineItem::CGraphicsLineItem(QGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    initLine();
}

CGraphicsLineItem::CGraphicsLineItem(const QLineF &line, QGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    m_line = line;
    initLine();
}

CGraphicsLineItem::CGraphicsLineItem(const QPointF &p1, const QPointF &p2, QGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    setLine(p1.x(), p1.y(), p2.x(), p2.y());
    initLine();
}

CGraphicsLineItem::CGraphicsLineItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_line(x1, y1, x2, y2)
{

}

CGraphicsLineItem::~CGraphicsLineItem()
{

}

int CGraphicsLineItem::type() const
{
    return LineType;
}

QPainterPath CGraphicsLineItem::shape() const
{
    QPainterPath path;
    if (m_line == QLineF())
        return path;

    path.moveTo(m_line.p1());
    path.lineTo(m_line.p2());

    return qt_graphicsItem_shapeFromPath(path, pen());
}

QRectF CGraphicsLineItem::boundingRect() const
{
    if (this->pen().widthF() == 0.0) {
        const qreal x1 = m_line.p1().x();
        const qreal x2 = m_line.p2().x();
        const qreal y1 = m_line.p1().y();
        const qreal y2 = m_line.p2().y();
        qreal lx = qMin(x1, x2);
        qreal rx = qMax(x1, x2);
        qreal ty = qMin(y1, y2);
        qreal by = qMax(y1, y2);
        return QRectF(lx, ty, rx - lx, by - ty);
    }
    return shape().controlPointRect();
}

QRectF CGraphicsLineItem::rect() const
{
    QRectF rect(m_line.p1(), m_line.p2());
    return rect.normalized();
}

void CGraphicsLineItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    QPointF local = mapFromScene(point);
    if (dir == CSizeHandleRect::LeftTop) {
        QPointF p1 = local;
        QPointF p2 = m_line.p2();
        setLine(p1, p2);
    } else {
        QPointF p1 = m_line.p1();
        QPointF p2 = local;
        setLine(p1, p2);
    }
}

QLineF CGraphicsLineItem::line() const
{
    return m_line;
}

void CGraphicsLineItem::setLine(const QLineF &line)
{
    prepareGeometryChange();
    m_line = line;
    updateGeometry();
}

void CGraphicsLineItem::setLine(const QPointF &p1, const QPointF &p2)
{
    setLine(p1.x(), p1.y(), p2.x(), p2.y());
}

void CGraphicsLineItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
    setLine(QLineF(x1, y1, x2, y2));
}

void CGraphicsLineItem::updateGeometry()
{
    const int w = SELECTION_HANDLE_SIZE;
    const int h = SELECTION_HANDLE_SIZE;

    for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it) {
        CSizeHandleRect *hndl = *it;
        QPointF centerPos = (m_line.p1() + m_line.p2()) / 2;
        switch (hndl->dir()) {
        case CSizeHandleRect::LeftTop:
            hndl->move(m_line.p1().x() - w / 2, m_line.p1().y() - h / 2);
            break;
        case CSizeHandleRect::RightBottom:
            hndl->move(m_line.p2().x() - w / 2, m_line.p2().y() - h / 2);
            break;
        case CSizeHandleRect::Rotation:

            hndl->move(centerPos.x() - w / 2, centerPos.y() - h / 2 - 15);
            break;
        default:
            break;
        }
    }
}

void CGraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(pen());
    painter->drawLine(m_line);
}

void CGraphicsLineItem::initLine()
{
    m_handles.reserve(CSizeHandleRect::None);

    m_handles.push_back(new CSizeHandleRect(this, CSizeHandleRect::LeftTop, this));
    m_handles.push_back(new CSizeHandleRect(this, CSizeHandleRect::RightBottom, this));
    m_handles.push_back(new CSizeHandleRect(this, CSizeHandleRect::Rotation, this));

    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}
