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
#include "csizehandlerect.h"
#include "cdrawparamsigleton.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QDebug>
#include <QString>
#include <QSvgRenderer>


CSizeHandleRect::CSizeHandleRect(QGraphicsItem *parent, EDirection d)
    : QGraphicsSvgItem(QString(":/theme/resources/node.svg"), parent)
    , m_dir(d)
    , m_state(SelectionHandleOff)
{
    setParentItem(parent);
    setCacheMode(NoCache);
    hide();
}

CSizeHandleRect::CSizeHandleRect(QGraphicsItem *parent, CSizeHandleRect::EDirection d, const QString &filename)
    : QGraphicsSvgItem(filename, parent)
    , m_dir(d)
    , m_state(SelectionHandleOff)
{
    setParentItem(parent);
    setCacheMode(NoCache);
    hide();
}


void CSizeHandleRect::updateCursor()
{
    switch (m_dir) {
    case Right:
        setCursor(Qt::SizeHorCursor);
        return;
    case RightTop:
        setCursor(Qt::SizeBDiagCursor);
        return;
    case RightBottom:
        setCursor(Qt::SizeFDiagCursor);
        return;
    case LeftBottom:
        setCursor(Qt::SizeBDiagCursor);
        return;
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        return;
    case LeftTop:
        setCursor(Qt::SizeFDiagCursor);
        return;
    case Left:
        setCursor(Qt::SizeHorCursor);
        return;
    case Top:
        setCursor(Qt::SizeVerCursor);
        return;
    default:
        break;
    }
    setCursor(Qt::ArrowCursor);
}


void CSizeHandleRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipping(false);
    QRectF rect = this->boundingRect();

    this->renderer()->render(painter, rect);
    painter->setClipping(true);

//    painter->setClipping(true);
//    QGraphicsSvgItem::paint(painter, option, widget);

}


void CSizeHandleRect::setState(ESelectionHandleState st)
{
    if (st != m_state) {
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
}

bool CSizeHandleRect::hitTest(const QPointF &point)
{
    QPointF pt = mapFromScene(point);
    return this->boundingRect().contains(pt);
}

void CSizeHandleRect::move(qreal x, qreal y)
{
    setPos(x, y);
}

QRectF CSizeHandleRect::boundingRect() const
{
    qreal scale = CDrawParamSigleton::GetInstance()->getScale();
    QRectF rect = QGraphicsSvgItem::boundingRect();
    rect.setWidth(rect.width() / scale);
    rect.setHeight(rect.height() / scale);
    return rect;
}





