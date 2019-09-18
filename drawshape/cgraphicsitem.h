/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CGRAPHICSITEM_H
#define CGRAPHICSITEM_H

#include "csizehandlerect.h"
#include "globaldefine.h"
#include "sitemdata.h"

#include <QAbstractGraphicsShapeItem>
#include <QCursor>

class CGraphicsItem : public QAbstractGraphicsShapeItem
{
public:
    CGraphicsItem(QGraphicsItem *parent );
    CGraphicsItem(const SGraphicsUnitHead &head, QGraphicsItem *parent);
    enum {Type = UserType};
    virtual int  type() const Q_DECL_OVERRIDE;
    virtual CSizeHandleRect::EDirection  hitTest( const QPointF &point ) const;
    virtual QPointF origin () const;
    virtual QCursor getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress = false);
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point ) = 0;
    virtual void resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress ) = 0;
    virtual QRectF rect() const = 0;
    virtual CGraphicsItem *duplicate() const;
    virtual CGraphicsUnit getGraphicsUnit() const;
    virtual void move(QPointF beginPoint, QPointF movePoint);


protected:
    virtual void updateGeometry() = 0;
    virtual void setState(ESelectionHandleState st);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;

protected:
    typedef QVector<CSizeHandleRect *> Handles;
    Handles m_handles;  //选中时 显示的小方框
    QCursor m_RotateCursor;
};

#endif // CGRAPHICSITEM_H
