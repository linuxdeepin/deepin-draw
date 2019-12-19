/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cdrawscene.h"
#include "widgets/ctextedit.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QVariant>

QPainterPath CGraphicsItem::qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (path == QPainterPath() || pen == Qt::NoPen)
        return path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;

}


CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
{

}

CGraphicsItem::CGraphicsItem(const SGraphicsUnitHead &head, QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
{

    this->setPen(head.pen);
    this->setBrush(head.brush);

    this->setRotation(head.rotate);
    this->setPos(head.pos);
    this->setZValue(head.zValue);
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

void CGraphicsItem::duplicate(CGraphicsItem *item)
{
    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
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

void CGraphicsItem::setSizeHandleRectFlag(CSizeHandleRect::EDirection dir, bool flag)
{
    foreach (CSizeHandleRect *sizeHandleRect, m_handles) {
        if (sizeHandleRect->dir() == dir) {
            sizeHandleRect->setVisible(flag);
            break;
        }
    }
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

    if (change == QGraphicsItem::ItemSelectedHasChanged &&  this->type() == TextType && value.toBool() == false) {
        static_cast<CGraphicsTextItem *>(this)->getTextEdit()->hide();
    }

    //change != QGraphicsItem::ItemVisibleChange 避免循环嵌套 引起死循环
    /*if (this->type() != BlurType && this->scene() != nullptr && change != QGraphicsItem::ItemVisibleChange &&
            change != QGraphicsItem::ItemVisibleHasChanged && change != QGraphicsItem::ItemSelectedChange &&
            change != QGraphicsItem::ItemSelectedHasChanged )*/

    //未来做多选操作，需要把刷新功能做到undoredo来统一管理
    //全选的由其它地方处理刷新 否则会出现卡顿
    if (/*(change == QGraphicsItem::ItemSelectedHasChanged && !CDrawParamSigleton::GetInstance()->getSelectAllFlag())  ||*/
        change == QGraphicsItem::ItemPositionHasChanged ||
        change == QGraphicsItem::ItemMatrixChange ||
        change == QGraphicsItem::ItemZValueHasChanged ||
        change == QGraphicsItem::ItemOpacityHasChanged ||
        change == QGraphicsItem::ItemRotationHasChanged ||
        change == QGraphicsItem::ItemTransformOriginPointHasChanged /*||
                                     (change == QGraphicsItem::ItemSceneHasChanged && this->scene() == nullptr)*/) {
//        QList<QGraphicsItem *> items = CDrawScene::GetInstance()->items();//this->collidingItems();
//        //QList<QGraphicsItem *> items = this->collidingItems();
//        foreach (QGraphicsItem *item, items) {
//            if (item->type() == BlurType) {
//                static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
//            }
//        }

        CDrawScene::GetInstance()->updateBlurItem(this);
    }

    return value;
}


