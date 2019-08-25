#ifndef CGRAPHICSITEM_H
#define CGRAPHICSITEM_H

#include "csizehandle.h"
#include <QAbstractGraphicsShapeItem>

class CGraphicsItem : public QAbstractGraphicsShapeItem
{
public:
    CGraphicsItem(QGraphicsItem *parent );
    enum {Type = UserType + 1};
    int  type() const
    {
        return Type;
    }
    CSizeHandleRect::Direction  hitTest( const QPointF &point ) const;
    virtual void resizeTo(CSizeHandleRect::Direction dir, const QPointF &point );
    virtual QPointF origin () const;
    virtual Qt::CursorShape getCursor(CSizeHandleRect::Direction dir );
    virtual QRectF  rect() const;

protected:
    virtual void updateGeometry();
    void setState(SelectionHandleState st);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    typedef QVector<CSizeHandleRect *> Handles;
    Handles m_handles;

};

#endif // CGRAPHICSITEM_H
