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
#include "cgraphicsitem.h"
#include "cgraphicsmasicoitem.h"
#include <QDebug>
#include <QGraphicsScene>

CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_RotateCursor(QPixmap(":/theme/resources/rotate_mouse.svg"))
{

}

CGraphicsItem::CGraphicsItem(const CGraphicsUnit &unit, QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_RotateCursor(QPixmap(":/theme/resources/rotate_mouse.svg"))
{

    this->setPen(unit.head.pen);
    this->setBrush(unit.head.brush);

    this->setRotation(unit.head.rotate);
    this->setPos(unit.head.pos);
    this->setZValue(unit.head.zValue);
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
    if (this->shape().contains(pt)) {
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

CGraphicsUnit CGraphicsItem::getGraphicsUnit() const
{
    return CGraphicsUnit();
}

void CGraphicsItem::move(QPointF beginPoint, QPointF movePoint)
{
    QPointF Pos = this->pos();
    //qDebug() << "Pos = " << Pos << "beginPoint" << beginPoint << "movePoint" << movePoint;
    this->setPos(Pos + movePoint - beginPoint);
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

    //change != QGraphicsItem::ItemVisibleChange 避免循环嵌套 引起死循环
    if (this->type() != BlurType && this->scene() != nullptr && change != QGraphicsItem::ItemVisibleChange && change != QGraphicsItem::ItemVisibleHasChanged) {
        QList<QGraphicsItem *> items = this->scene()->items(this->scene()->sceneRect());//this->collidingItems();
        //QList<QGraphicsItem *> items = this->collidingItems();
        foreach (QGraphicsItem *item, items) {
            if (item->type() == BlurType) {
                static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
            }
        }
    }

    return value;
}

