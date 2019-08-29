#ifndef CGRAPHICSITEM_H
#define CGRAPHICSITEM_H

#include "csizehandlerect.h"
#include <QAbstractGraphicsShapeItem>
#include "globaldefine.h"

class CGraphicsItem : public QAbstractGraphicsShapeItem
{
public:
    CGraphicsItem(QGraphicsItem *parent );
    enum {Type = UserType};
    virtual int  type() const Q_DECL_OVERRIDE;
    virtual CSizeHandleRect::EDirection  hitTest( const QPointF &point ) const;
    virtual QPointF origin () const;
    virtual Qt::CursorShape getCursor(CSizeHandleRect::EDirection dir );
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) = 0;
    virtual QRectF rect() const = 0;

protected:
    virtual void updateGeometry() = 0;
    virtual void setState(ESelectionHandleState st);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;

protected:
    typedef QVector<CSizeHandleRect *> Handles;
    Handles m_handles;  //选中时 显示的小方框
};

#endif // CGRAPHICSITEM_H
