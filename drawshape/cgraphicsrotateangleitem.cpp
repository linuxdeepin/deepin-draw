#include "cgraphicsrotateangleitem.h"

#include <QPainter>

CGraphicsRotateAngleItem::CGraphicsRotateAngleItem(qreal rotateAngle, QGraphicsItem *parent)
    : QGraphicsRectItem (parent)
    , m_rotateAngle(rotateAngle)
{
    m_width = 45;
    m_height = 20;
    setRect(-m_width / 2, -m_height / 2, m_width, m_height);
}

CGraphicsRotateAngleItem::CGraphicsRotateAngleItem(const QRectF &rect, qreal rotateAngle, QGraphicsItem *parent)
    : QGraphicsRectItem (rect, parent)
    , m_rotateAngle(rotateAngle)

{
    setRect(rect);
}

void CGraphicsRotateAngleItem::updateRotateAngle(qreal rotateAngle)
{
    m_rotateAngle =  rotateAngle;
}

void CGraphicsRotateAngleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#ececf8"));

    painter->drawRoundRect(rect());

    QString angle = QString("%1°").arg(QString::number(m_rotateAngle, 'f', 1));
    painter->setPen(Qt::black);
    painter->drawText(rect(), angle);
}
