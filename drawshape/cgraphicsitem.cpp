#include "cgraphicsitem.h"
#include <QDebug>

CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_RotateCursor(QPixmap(":/theme/resources/rotate_mouse.svg"))
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
    //检测是否在矩形内
    QPointF pt = mapFromScene(point);
    if (this->boundingRect().contains(pt)) {
        return CSizeHandleRect::InRect;
    }

    return CSizeHandleRect::None;
}

QCursor CGraphicsItem::getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress)
{
    Qt::CursorShape result;
    QCursor resultCursor;
    switch (dir) {
    case CSizeHandleRect::Right:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeHorCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::RightTop:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeBDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::RightBottom:
//        result =  Qt::SizeFDiagCursor;
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeFDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::LeftBottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeBDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::Bottom:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeVerCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::LeftTop:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeFDiagCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::Left:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeHorCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::Top:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::SizeVerCursor;
        }
        resultCursor = QCursor(result);
        break;

    case CSizeHandleRect::Rotation:
        resultCursor = m_RotateCursor;
        break;
    case CSizeHandleRect::InRect:
        if (bMouseLeftPress) {
            result =  Qt::ClosedHandCursor;
        } else {
            result =  Qt::OpenHandCursor;
        }
        resultCursor = QCursor(result);
        break;
    case CSizeHandleRect::None:
        result =  Qt::ArrowCursor;
        resultCursor = QCursor(result);
        break;
    //result =  Qt::ClosedHandCursor;
    default:
        result =  Qt::ArrowCursor;
        resultCursor = QCursor(result);
        break;
    }

    return resultCursor;
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

