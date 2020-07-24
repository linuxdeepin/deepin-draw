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
#include "frame/cgraphicsview.h"

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
    , m_bMutiSelectFlag(false)
{

}

CGraphicsItem::CGraphicsItem(const SGraphicsUnitHead &head, QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_bMutiSelectFlag(false)
{

    this->setPen(head.pen);
    this->setBrush(head.brush);

    this->setRotation(head.rotate);
    this->setPos(head.pos);
    this->setZValue(head.zValue);
}

CGraphicsView *CGraphicsItem::curView() const
{
    CGraphicsView *parentView = nullptr;
    if (scene() != nullptr) {
        if (!scene()->views().isEmpty()) {
            parentView = dynamic_cast<CGraphicsView *>(scene()->views().first());
        }
    }
    return parentView;
}

void CGraphicsItem::setMutiSelect(bool flag)
{
    m_bMutiSelectFlag = flag;
}

bool CGraphicsItem::getMutiSelect() const
{
    auto curSelectFlag = m_bMutiSelectFlag;
    if (isSelected()) {
        curSelectFlag = isSelected();
    }
    return curSelectFlag;
}

QPainterPath CGraphicsItem::getHighLightPath()
{
    return QPainterPath();
}

QRectF CGraphicsItem::scenRect()
{
    if (scene() != nullptr) {
        return sceneBoundingRect().translated(-scene()->sceneRect().topLeft());
    }
    return sceneBoundingRect();
}

int CGraphicsItem::type() const
{
    return Type;
}

CSizeHandleRect::EDirection CGraphicsItem::hitTest(const QPointF &point) const
{
    const Handles::const_iterator hend =  m_handles.end();
    for (Handles::const_iterator it = m_handles.begin(); it != hend; ++it) {
        if ((*it)->hitTest(point)) {
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

void CGraphicsItem::resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                                const double &xScale, const double &yScale,
                                bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(dir)
    Q_UNUSED(offset)
    Q_UNUSED(xScale)
    Q_UNUSED(yScale)
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
}

void CGraphicsItem::resizeToMul_7(CSizeHandleRect::EDirection dir,
                                  QRectF pressRect, QRectF itemPressRect,
                                  const qreal &xScale, const qreal &yScale,
                                  bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(dir)
    Q_UNUSED(itemPressRect)
    Q_UNUSED(pressRect)
    Q_UNUSED(xScale)
    Q_UNUSED(yScale)
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
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

void CGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event);
}

QVariant CGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
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

        if (nullptr != scene()) {
            auto curScene = static_cast<CDrawScene *>(scene());
            curScene->updateBlurItem(this);
        }
    }

    if (QGraphicsItem::ItemSceneChange == change) {

        if (this->type() >= RectType && this->type() < MgrType) {
            QGraphicsScene *pScene = qvariant_cast<QGraphicsScene *>(value);
            if (pScene == nullptr) {
                clearHandle();
            } else {
                initHandle();
            }
        }

    }

    return value;
}

void CGraphicsItem::beginCheckIns(QPainter *painter)
{
    if (scene() == nullptr || !rect().isValid())
        return;

    painter->save();
    QRectF sceneRct = scene()->sceneRect();
    QRectF itemRct  = mapToScene(rect()).boundingRect();
    bool hasIntersects = sceneRct.intersects(itemRct);
    if (!hasIntersects) {
        painter->setOpacity(0.2);//透明度设置
    }
    painter->setClipping(hasIntersects);
}

void CGraphicsItem::endCheckIns(QPainter *painter)
{
    if (scene() == nullptr || !rect().isValid())
        return;
    painter->restore();
}
void CGraphicsItem::clearHandle()
{
    for (CSizeHandleRect *pItem : m_handles) {
        pItem->setParentItem(nullptr);
        if (pItem->scene() != nullptr) {
            pItem->scene()->removeItem(pItem);
        }
    }
    m_handles.clear();
}
void CGraphicsItem::initHandle()
{
    clearHandle();
    // handles
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Rotation; ++i) {
        CSizeHandleRect *shr = nullptr;
        if (i == CSizeHandleRect::Rotation) {
            shr   = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), QString(":/theme/light/images/mouse_style/icon_rotate.svg"));

        } else {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        }
        m_handles.push_back(shr);

    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}
