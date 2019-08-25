#include "cgraphicsitem.h"
#include <QDebug>

CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
{

}

void CGraphicsItem::updateGeometry()
{
    const QRectF &geom = this->boundingRect();

    const int w = SELECTION_HANDLE_SIZE;
    const int h = SELECTION_HANDLE_SIZE;

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;;
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
        case CSizeHandleRect::Center:
            hndl->move(geom.center().x()  - w / 2, geom.center().y() - h / 2);
            break;
        default:
            break;
        }
    }

}

void CGraphicsItem::setState(SelectionHandleState st)
{
    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it)
        (*it)->setState(st);
}

CSizeHandleRect::Direction CGraphicsItem::hitTest(const QPointF &point) const
{
    const Handles::const_iterator hend =  m_handles.end();
    for (Handles::const_iterator it = m_handles.begin(); it != hend; ++it) {
        if ((*it)->hitTest(point) ) {
            return (*it)->dir();
        }
    }
    return CSizeHandleRect::None;
}

Qt::CursorShape CGraphicsItem::getCursor(CSizeHandleRect::Direction dir)
{
    switch (dir) {
    case CSizeHandleRect::Right:
        return Qt::SizeHorCursor;
    case CSizeHandleRect::RightTop:
        return Qt::SizeBDiagCursor;
    case CSizeHandleRect::RightBottom:
        return Qt::SizeFDiagCursor;
    case CSizeHandleRect::LeftBottom:
        return Qt::SizeBDiagCursor;
    case CSizeHandleRect::Bottom:
        return Qt::SizeVerCursor;
    case CSizeHandleRect::LeftTop:
        return Qt::SizeFDiagCursor;
    case CSizeHandleRect::Left:
        return Qt::SizeHorCursor;
    case CSizeHandleRect::Top:
        return Qt::SizeVerCursor;
    default:
        break;
    }
    return Qt::ArrowCursor;
}

QRectF CGraphicsItem::rect() const
{
    return QRectF(0, 0, 0, 0);
}

void CGraphicsItem::resizeTo(CSizeHandleRect::Direction dir, const QPointF &point)
{

}

QPointF CGraphicsItem::origin() const
{
    return QPointF(0, 0);
}

void CGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

}

QVariant CGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if ( change == QGraphicsItem::ItemSelectedHasChanged ) {
        qDebug() << " Item Selected : " << value.toString();
        setState(value.toBool() ? SelectionHandleActive : SelectionHandleOff);
    } else if ( change == QGraphicsItem::ItemRotationHasChanged ) {
        qDebug() << "Item Rotation Changed:" << value.toString();
    } else if ( change == QGraphicsItem::ItemTransformOriginPointHasChanged ) {
        qDebug() << "ItemTransformOriginPointHasChanged:" << value.toPointF();
    }
    return value;
}

