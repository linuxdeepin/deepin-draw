#include "sizehandle.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <qdebug.h>
#include <QtWidgets>
#include <QPixmap>

SizeHandleRect::SizeHandleRect(QGraphicsItem *parent, int d)
    : QGraphicsRectItem(-SELECTION_HANDLE_SIZE / 2,
                        -SELECTION_HANDLE_SIZE / 2,
                        SELECTION_HANDLE_SIZE,
                        SELECTION_HANDLE_SIZE, parent)
    , m_dir(d)
    , m_state(SelectionHandleOff)
    , borderColor("white")
{
    this->setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    hide();
}


void SizeHandleRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();

    painter->setPen(Qt::SolidLine);
    painter->setBrush(QBrush(borderColor));

    painter->setRenderHint(QPainter::Antialiasing, false);

    painter->drawRect(rect());

    painter->restore();
}

void SizeHandleRect::setState(SelectionHandleState st)
{
    if (st == m_state)
        return;
    switch (st) {
    case SelectionHandleOff:
        hide();
        break;
    case SelectionHandleInactive:
    case SelectionHandleActive:
        show();
        break;
    }
    borderColor = Qt::white;
    m_state = st;
}

void SizeHandleRect::move(qreal x, qreal y)
{
    setPos(x, y);
}

void SizeHandleRect::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    borderColor = Qt::blue;
    update();
    QGraphicsRectItem::hoverEnterEvent(e);
}

void SizeHandleRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    borderColor = Qt::white;
    update();
    QGraphicsRectItem::hoverLeaveEvent(e);
}



/////////////////////////////////////////////////////////////////////////////////////

RotateHandle::RotateHandle(QGraphicsItem *parent, int d, QPixmap pixmap)
    : QGraphicsRectItem(- pixmap.rect().width() / 2,
                        - pixmap.rect().width() / 2,
                        pixmap.rect().width(),
                        pixmap.rect().width(), parent)
    , m_dir(d)
    , m_pixMap(pixmap)
    , m_pixMapWidth(pixmap.rect().width())
{
    this->setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    hide();
}

void RotateHandle::setState(SelectionHandleState st)
{
    if (st == m_state)
        return;
    switch (st) {
    case SelectionHandleOff:
        hide();
        break;
    case SelectionHandleInactive:
    case SelectionHandleActive:
        show();
        break;
    }
    m_state = st;
}

void RotateHandle::move(qreal x, qreal y)
{
    setPos(x, y);
}

void RotateHandle::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    update();
    QGraphicsRectItem::hoverEnterEvent(e);
}

void RotateHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    update();
    QGraphicsRectItem::hoverLeaveEvent(e);
}

void RotateHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();

    painter->drawPixmap(QPointF(-m_pixMapWidth / 2, -m_pixMapWidth / 2), m_pixMap);

    painter->restore();
}
