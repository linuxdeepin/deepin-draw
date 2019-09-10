#include "cgraphicspolygonitem.h"
#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QPainter>
#include <QPolygonF>

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, CGraphicsItem *parent)
    : CGraphicsRectItem (parent)
    , m_nPointsCount(count)
{

}

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsRectItem (rect, parent)
    , m_nPointsCount(count)
{

}

CGraphicsPolygonItem::CGraphicsPolygonItem(int count, qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsRectItem (x, y, w, h, parent)
    , m_nPointsCount(count)
{

}

QPainterPath CGraphicsPolygonItem::shape() const
{
    QPainterPath path;
    path.addPolygon(m_listPoints);
    return path;
}

int CGraphicsPolygonItem::type() const
{
    return PolygonType;
}

CGraphicsItem *CGraphicsPolygonItem::duplicate() const
{
    CGraphicsPolygonItem *item = new CGraphicsPolygonItem(m_nPointsCount, rect());

    item->setListPoints(m_listPoints);
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

void CGraphicsPolygonItem::setRect(const QRectF &rect)
{
    CGraphicsRectItem::setRect(rect);
    prepareGeometryChange();
    //更新坐标
    calcPoints(m_nPointsCount);
}

void CGraphicsPolygonItem::setPointCount(int num)
{
    m_nPointsCount = num;
    //重新计算
    calcPoints(m_nPointsCount);
}

void CGraphicsPolygonItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    CGraphicsRectItem::resizeTo(dir, point, bShiftPress, bAltPress);
    setPointCount(m_nPointsCount);
}

void CGraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    updateGeometry();
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPolygon(m_listPoints);

    if (this->isSelected()) {
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(224, 224, 224));
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->rect());
    }
}

void CGraphicsPolygonItem::calcPoints(int n)
{
    prepareGeometryChange();
    m_listPoints.clear();
    qreal angle = 90 * M_PI / 180;
    QPointF pointCenter = this->rect().center();
    qreal w = this->rect().width();
    qreal h = this->rect().height();
    qreal R = w < h ? w : h;
    qreal r = R / 2;

    if (n > 0) {
        qreal preAngle = 360 / n * M_PI / 180;
        for (int i = 0; i != n; i++) {
            ////程序坐标的Y轴和数学坐标中的Y轴是相反的
            //qreal x = pointCenter.x() + cos(angle + preAngle * i) * r;
            //qreal y = pointCenter.y() - sin(angle + preAngle * i) * r;
            //x = a cosθ y = b sinθ
            qreal x = pointCenter.x() + w / 2 * cos(angle + preAngle * i);
            qreal y = pointCenter.y() - h / 2 * sin(angle + preAngle * i);
            m_listPoints.push_back(QPointF(x, y));
        }
    }

}

void CGraphicsPolygonItem::setListPoints(const QVector<QPointF> &listPoints)
{
    m_listPoints = listPoints;
}

int CGraphicsPolygonItem::nPointsCount() const
{
    return m_nPointsCount;
}
