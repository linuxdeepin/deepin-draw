#include "cgraphicsitem.h"
#include <QDebug>

CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
{

}

int CGraphicsItem::type() const
{
    return Type;
}

CSizeHandleRect::EDirection CGraphicsItem::hitTest(const QPointF &point) const
{
    const Handles::const_iterator hend =  m_handles.end();
    for (Handles::const_iterator it = m_handles.begin(); it != hend; ++it) {
        if ((*it)->hitTest(point) ) {
            return (*it)->dir();
        }
    }
    return CSizeHandleRect::None;
}

Qt::CursorShape CGraphicsItem::getCursor(CSizeHandleRect::EDirection dir)
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

CGraphicsItem *CGraphicsItem::duplicate() const
{
    return nullptr;
}

void CGraphicsItem::setState(ESelectionHandleState st)
{
    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {

        (*it)->setState(st);
    }
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
        setState(value.toBool() ? SelectionHandleActive : SelectionHandleOff);
    }

    return value;
}

