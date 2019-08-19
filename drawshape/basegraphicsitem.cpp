#include "basegraphicsitem.h"
#include <QPainter>


BaseGraphicsItem::BaseGraphicsItem(QGraphicsItem *parent)
    : AbstractShapeType<QGraphicsItem>(parent)
{
    m_handles.reserve(Left);
    for (int i = LeftTop; i <= Left; ++i) {
        SizeHandleRect *shr = new SizeHandleRect(this, i);
        m_handles.push_back(shr);
    }

    QPixmap pixMap;
    pixMap.load(":/theme/light/images/size/rotate.png");
    m_rotateHandle = new RotateHandle(this, OutTop, pixMap);


    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}


QPixmap BaseGraphicsItem::image()
{
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    setPen(QPen(Qt::black));
    setBrush(Qt::white);
    QStyleOptionGraphicsItem *styleOption = new QStyleOptionGraphicsItem;
//    painter.translate(m_localRect.center().x(),m_localRect.center().y());
    paint(&painter, styleOption);
    delete styleOption;
    return pixmap;
}


void BaseGraphicsItem::updatehandles()
{

    const QRectF &geom = m_localRect;
//    const QRectF &geom1 = this->boundingRect();

    m_rotateHandle->move(geom.x() + geom.width() / 2, geom.y() - 30);

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        SizeHandleRect *hndl = *it;;
        switch (hndl->dir()) {
        case LeftTop:
            hndl->move(geom.x(), geom.y() );
            break;
        case Top:
            hndl->move(geom.x() + geom.width() / 2, geom.y());

            break;
        case RightTop:
            hndl->move(geom.x() + geom.width(), geom.y() );
            break;
        case Right:
            hndl->move(geom.x() + geom.width(), geom.y() + geom.height() / 2 );
            break;
        case RightBottom:
            hndl->move(geom.x() + geom.width(), geom.y() + geom.height() );
            break;
        case Bottom:
            hndl->move(geom.x() + geom.width() / 2, geom.y() + geom.height() );
            break;
        case LeftBottom:
            hndl->move(geom.x(), geom.y() + geom.height());
            break;
        case Left:
            hndl->move(geom.x(), geom.y() + geom.height() / 2);
            break;
        default:
            break;
        }
    }
}

void BaseGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event);
}


QVariant BaseGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if ( change == QGraphicsItem::ItemSelectedHasChanged ) {
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(parentItem());
        if (!g)
            setState(value.toBool() ? SelectionHandleActive : SelectionHandleOff);
        else {
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }
    }
    /*
    else if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right()-boundingRect().width()/2, qMax(newPos.x(), rect.left()+boundingRect().width()/2)));
            newPos.setY(qMin(rect.bottom()-boundingRect().height()/2, qMax(newPos.y(), rect.top()+boundingRect().height()/2)));
            return newPos;
        }
    }
    */
    return QGraphicsItem::itemChange(change, value);
}
