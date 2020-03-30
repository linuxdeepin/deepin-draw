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
#include "cundocommands.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicsrectitem.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include "drawshape/cgraphicspenitem.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/cdrawscene.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicsmasicoitem.h"
#include "drawshape/cgraphicsitemselectedmgr.h"
#include "drawshape/cgraphicstextitem.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QBrush>
#include <QDebug>
//升序排列用
static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() <= info2->zValue();
}
//降序排列用
static bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() >= info2->zValue();
}

CMoveShapeCommand::CMoveShapeCommand(CDrawScene *scene, const QPointF &delta, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myItem = nullptr;
    myItems = scene->selectedItems();
    myGraphicsScene = scene;
    myDelta = delta;
    bMoved = true;
}

CMoveShapeCommand::CMoveShapeCommand(CDrawScene *scene, QGraphicsItem *item, const QPointF &delta, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = scene;
    myItem = item;
    myDelta = delta;
    bMoved = true;
}

//! [2]
void CMoveShapeCommand::undo()
{
    qDebug() << "CMoveShapeCommand undo";
    if ( myItem )
        myItem->moveBy(-myDelta.x(), -myDelta.y());
    else if ( myItems.count() > 0 ) {
        foreach (QGraphicsItem *item, myItems) {
            item->moveBy(-myDelta.x(), -myDelta.y());
        }
    }
//    setText(QObject::tr("Undo Move %1,%2")
//            .arg(-myDelta.x()).arg(-myDelta.y()));
    bMoved = false;

    myGraphicsScene->setModify(true);
    if (!myGraphicsScene->views().isEmpty()) {
        myGraphicsScene->views().first()->update();
    }
}
//! [2]

//! [3]
void CMoveShapeCommand::redo()
{
    qDebug() << "CMoveShapeCommand redo";
    if ( !bMoved ) {
        if ( myItem ) {
            myItem->moveBy(myDelta.x(), myDelta.y());
            myItem->scene()->update();
        } else if ( myItems.count() > 0 ) {
            foreach (QGraphicsItem *item, myItems) {
                item->moveBy(myDelta.x(), myDelta.y());
            }
            myGraphicsScene->update();
        }
    }

    myGraphicsScene->setModify(true);
    if (!myGraphicsScene->views().isEmpty()) {
        myGraphicsScene->views().first()->update();
    }
}
//! [3]

CDeleteShapeCommand::CDeleteShapeCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent)
{
    myGraphicsScene = scene;
    m_items = items;
    m_oldIndex = -1;
}

CDeleteShapeCommand::~CDeleteShapeCommand()
{

}

void CDeleteShapeCommand::undo()
{
    qDebug() << "CDeleteShapeCommand undo";
    foreach (QGraphicsItem *item, m_items) {
        myGraphicsScene->addItem(item);
    }
    myGraphicsScene->update();
    if (m_oldIndex != -1) {
        QList<QGraphicsItem *> itemList = myGraphicsScene->items(/*Qt::AscendingOrder*/);
        for (int i = 0; i < m_oldIndex + 1 ; i++) {
//            qDebug() << "!!!!!!!!!!!item=" << itemList.at(i)->type() << "zValue=" << "i=" << i << "::" << itemList.at(i)->zValue();
            if (itemList.at(i)->type() > QGraphicsItem::UserType /*&& itemList.at(i) != m_items.first()*/) {
//                qDebug() << "@@@@@@@@@@@@@@@@item=" << itemList.at(i)->type() << "zValue=" << "i=" << i << "::" << itemList.at(i)->zValue();
                QGraphicsItem *item = itemList.at(i);
                m_items.first()->stackBefore(item);
            }
        }
    }
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem();
}

void CDeleteShapeCommand::redo()
{
    qDebug() << "CDeleteShapeCommand redo";
    qDebug() << "multSelectItems count = ";
    if (m_items.count() == 1) {
        QList<QGraphicsItem *> itemList = myGraphicsScene->items();
//        for (int i = 0; i < itemList.count(); i++) {
//            qDebug() << "**********item=" << itemList.at(i)->type() << "zValue=" << "i=" << i << "::" << itemList.at(i)->zValue();
//        }
        m_oldIndex = itemList.indexOf(m_items.first());
    }

    foreach (QGraphicsItem *item, m_items) {
        myGraphicsScene->removeItem(item);
    }
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem();
}


//! [4]
CRemoveShapeCommand::CRemoveShapeCommand(CDrawScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = scene;
    for (auto item : scene->selectedItems()) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            items.append(item);
        }
    }
}

CRemoveShapeCommand::~CRemoveShapeCommand()
{

}
//! [4]

//! [5]
void CRemoveShapeCommand::undo()
{
    qDebug() << "CRemoveShapeCommand undo";
    foreach (QGraphicsItem *item, items) {
//        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(item->parentItem());
//        if ( g != nullptr )
        myGraphicsScene->addItem(item);
    }
    myGraphicsScene->update();
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem();
    //setText(QObject::tr("Undo Delete %1").arg(items.count()));
}
//! [5]

//! [6]
void CRemoveShapeCommand::redo()
{
    qDebug() << "CRemoveShapeCommand redo";
    foreach (QGraphicsItem *item, items) {
//        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(item->parentItem());
//        if ( g != nullptr )
        myGraphicsScene->removeItem(item);
    }
    myGraphicsScene->setModify(true);
    //setText(QObject::tr("Redo Delete %1").arg(items.count()));
}

//! [6]

//! [7]

CAddShapeCommand::CAddShapeCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    static int itemCount = 0;

    myGraphicsScene = scene;
    m_items = items;
    //myDiagramItem = item;
    //initialPosition = item->pos();
    ++itemCount;
}
//! [7]

CAddShapeCommand::~CAddShapeCommand()
{

}

//! [8]
void CAddShapeCommand::undo()
{
    qDebug() << "CAddShapeCommand undo";
    //myGraphicsScene->removeItem(myDiagramItem);
    myGraphicsScene->update();
    myGraphicsScene->setModify(true);

    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = 0; i < m_items.size(); i++) {
        QGraphicsItem *item = m_items.at(i);
        if (allItems.contains(static_cast<CGraphicsItem *>(item))) {
            myGraphicsScene->removeItem(item);
        }
        if (myGraphicsScene->getItemsMgr()->getItems().contains(static_cast<CGraphicsItem *>(item))) {
            myGraphicsScene->getItemsMgr()->removeFromGroup(static_cast<CGraphicsItem *>(item));
        }
    }

    if (myGraphicsScene->getItemsMgr()->getItems().count() <= 1) {
        myGraphicsScene->getItemsMgr()->setSelected(false);
    }

    //重置保存的最大z值
    allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }
}
//! [8]

//! [9]
void CAddShapeCommand::redo()
{
    qDebug() << "CAddShapeCommand redo";
//    if ( myDiagramItem->scene() == nullptr )
//        myGraphicsScene->addItem(myDiagramItem);
//    myDiagramItem->setPos(initialPosition);

    qDebug() << "myGraphicsScene->getMaxZValue() = " << myGraphicsScene->getMaxZValue();
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        if (allItems.at(i)->zValue() == 0.0) {
            allItems.removeAt(i);
            continue;
        }
        if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
            allItems.removeAt(i);
        }
    }

    for (int i = 0; i < m_items.size(); i++) {
        QGraphicsItem *item = m_items.at(i);
        if (!allItems.contains(static_cast<CGraphicsItem *>(item))) {
            item->setZValue(myGraphicsScene->getMaxZValue() + 1);
            myGraphicsScene->addItem(item);
            myGraphicsScene->setMaxZValue(myGraphicsScene->getMaxZValue() + 1);
        }
    }

    //重置保存的最大z值
    allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }

    myGraphicsScene->update();
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem();
}

/*

QString createCommandString(QGraphicsItem *item, const QPointF &pos)
{
    return QObject::tr("Item at (%1, %2)")
           .arg(pos.x()).arg(pos.y());
}
*/

CRotateShapeCommand::CRotateShapeCommand(CDrawScene *scene, QGraphicsItem *item, const qreal oldAngle, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = scene;
    myItem = item;
    myOldAngle = oldAngle;
    newAngle = item->rotation();

}

void CRotateShapeCommand::undo()
{
    myItem->setRotation(myOldAngle);
    myItem->scene()->update();
    myGraphicsScene->setModify(true);
//    setText(QObject::tr("Undo Rotate %1").arg(newAngle));
}

void CRotateShapeCommand::redo()
{
    myItem->setRotation(newAngle);
    myItem->update();
    myGraphicsScene->setModify(true);
//    setText(QObject::tr("Redo Rotate %1").arg(newAngle));
}

/*
GroupShapeCommand::GroupShapeCommand(QGraphicsItemGroup *group,
                                     QGraphicsScene *graphicsScene,
                                     QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = graphicsScene;
    myGroup = group;
    items = group->childItems();
    b_undo = false;
}

void GroupShapeCommand::undo()
{
    myGroup->setSelected(false);
    QList<QGraphicsItem *> plist = myGroup->childItems();
    foreach (QGraphicsItem *item, plist) {
        item->setSelected(true);
        myGroup->removeFromGroup(item);
    }
    myGraphicsScene->removeItem(myGroup);
    myGraphicsScene->update();
    b_undo = true;
    setText(QObject::tr("Undo Group %1").arg(items.count()));

}

void GroupShapeCommand::redo()
{
    if (b_undo) {
        foreach (QGraphicsItem *item, items) {
            item->setSelected(false);
            QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(item->parentItem());
            if ( !g )
                myGroup->addToGroup(item);
        }
    }
    myGroup->setSelected(true);
    if ( myGroup->scene() == NULL )
        myGraphicsScene->addItem(myGroup);
    myGraphicsScene->update();

    setText(QObject::tr("Redo Group %1").arg(items.count()));

}


UnGroupShapeCommand::UnGroupShapeCommand(QGraphicsItemGroup *group,
                                         QGraphicsScene *graphicsScene,
                                         QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = graphicsScene;
    myGroup = group;
    items = group->childItems();
}

void UnGroupShapeCommand::undo()
{
    foreach (QGraphicsItem *item, items) {
        item->setSelected(false);
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(item->parentItem());
        if ( !g )
            myGroup->addToGroup(item);
    }
    myGroup->setSelected(true);
    if ( myGroup->scene() == NULL )
        myGraphicsScene->addItem(myGroup);
    myGraphicsScene->update();

    setText(QObject::tr("Undo UnGroup %1").arg(items.count()));

}

void UnGroupShapeCommand::redo()
{
    myGroup->setSelected(false);
    foreach (QGraphicsItem *item, myGroup->childItems()) {
        item->setSelected(true);
        myGroup->removeFromGroup(item);
        AbstractShape *ab = qgraphicsitem_cast<AbstractShape *>(item);
        if (ab && !qgraphicsitem_cast<SizeHandleRect *>(ab))
            ab->updateCoordinate();
    }
    myGraphicsScene->removeItem(myGroup);
    myGraphicsScene->update();
    setText(QObject::tr("Redo UnGroup %1").arg(items.count()));

}
*/


CResizeShapeCommand::CResizeShapeCommand(CDrawScene *scene, CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress, QUndoCommand *parent)
    : QUndoCommand(parent)
    , myItem(item)
    , m_handle(handle)
    , m_beginPos(beginPos)
    , m_endPos(endPos)
    , m_bShiftPress(bShiftPress)
    , m_bAltPress(bALtPress)
{
    myGraphicsScene = scene;
}

void CResizeShapeCommand::undo()
{
    qDebug() << "CResizeShapeCommand undo";
    if (myItem != nullptr) {
        myItem->resizeTo(m_handle,  m_beginPos, m_bShiftPress, m_bAltPress);
        myItem->update();
    }
    myGraphicsScene->setModify(true);
}

void CResizeShapeCommand::redo()
{
    qDebug() << "CResizeShapeCommand redo";
    if (myItem != nullptr) {
        myItem->resizeTo(m_handle, m_endPos, m_bShiftPress, m_bAltPress);
        myItem->update();
    }
    myGraphicsScene->setModify(true);
}

/*
ControlShapeCommand::ControlShapeCommand(QGraphicsItem *item,
                                         int handle,
                                         const QPointF &newPos,
                                         const QPointF &lastPos,
                                         QUndoCommand *parent)
{
    myItem = item;
    handle_ = handle;
    lastPos_  = QPointF(lastPos) ;
    newPos_ = QPointF(newPos);
    bControled = true;
}

void ControlShapeCommand::undo()
{

    AbstractShape *item = qgraphicsitem_cast<AbstractShape *>(myItem);
    if ( item ) {
        item->control(handle_, lastPos_);
        item->updateCoordinate();
        item->update();
    }
    bControled = false;
    setText(QObject::tr("Undo Control %1,%2")
            .arg(lastPos_.x()).arg(lastPos_.y()));

}

void ControlShapeCommand::redo()
{
    if ( !bControled ) {
        AbstractShape *item = qgraphicsitem_cast<AbstractShape *>(myItem);
        if ( item ) {
            item->control(handle_, newPos_);
            item->updateCoordinate();
            item->update();
        }
    }
    setText(QObject::tr("Redo Control %1,%2")
            .arg(newPos_.x()).arg(newPos_.y()));

}
bool ControlShapeCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != ControlShapeCommand::Id )
        return false;

    const ControlShapeCommand *cmd = static_cast<const ControlShapeCommand *>(command);
    QGraphicsItem *item = cmd->myItem;

    if (myItem != item )
        return false;
    if ( cmd->handle_ != handle_ )
        return false;
    handle_ = cmd->handle_;
    lastPos_ = cmd->lastPos_;
    newPos_  = cmd->newPos_;
    setText(QObject::tr(" mergeWith Control %1,%2,%3")
            .arg(newPos_.x()).arg(newPos_.y()).arg(handle_));

    return true;
}
*/


CSetPropertyCommand::CSetPropertyCommand(CDrawScene *scene, CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange, QUndoCommand *parent)
    : QUndoCommand (parent)
    , m_pItem(item)
    , m_newPen(pen)
    , m_newBrush(brush)
    , m_bPenChange(bPenChange)
    , m_bBrushChange(bBrushChange)
{
    myGraphicsScene = scene;
    m_oldPen = item->pen();
    m_oldBrush = item->brush();
}

CSetPropertyCommand::~CSetPropertyCommand()
{

}

void CSetPropertyCommand::undo()
{
    if (m_bPenChange) {
        m_pItem->setPen(m_oldPen);
        if (m_pItem->type() == LineType) {
            static_cast<CGraphicsLineItem *>(m_pItem)->calcVertexes();
        }
    }

    if (m_bBrushChange) {
        m_pItem->setBrush(m_oldBrush);
    }
    myGraphicsScene->setModify(true);
    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}

void CSetPropertyCommand::redo()
{
    if (m_bPenChange) {
        m_pItem->setPen(m_newPen);
        if (m_pItem->type() == LineType) {
            static_cast<CGraphicsLineItem *>(m_pItem)->calcVertexes();
        }
    }

    if (m_bBrushChange) {
        m_pItem->setBrush(m_newBrush);
    }

    myGraphicsScene->setModify(true);
    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}


CSetRectXRediusCommand::CSetRectXRediusCommand(CDrawScene *scene, CGraphicsRectItem *item, int redius, bool bRediusChange, QUndoCommand *parent)
    : QUndoCommand (parent)
    , myGraphicsScene(scene)
    , m_pItem(item)
    , m_newRectXRedius(redius)
    , m_bRectXRediusChange(bRediusChange)
{
    m_oldRectXRedius = item->getXRedius();
}

CSetRectXRediusCommand::~CSetRectXRediusCommand()
{

}

void CSetRectXRediusCommand::undo()
{
    if (m_bRectXRediusChange) {
        m_pItem->setXYRedius(m_oldRectXRedius, m_oldRectXRedius);
    }
    myGraphicsScene->setModify(true);
    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}

void CSetRectXRediusCommand::redo()
{
    if (m_bRectXRediusChange) {
        m_pItem->setXYRedius(m_newRectXRedius, m_newRectXRedius);
        m_pItem->update();
    }

    myGraphicsScene->setModify(true);
    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}

CSetPolygonAttributeCommand::CSetPolygonAttributeCommand(CDrawScene *scene, CGraphicsPolygonItem *item, int newNum)
    : m_pItem(item)
    , m_nNewNum(newNum)
{
    myGraphicsScene = scene;
    m_nOldNum = m_pItem->nPointsCount();
}

void CSetPolygonAttributeCommand::undo()
{
    m_pItem->setPointCount(m_nOldNum);
    myGraphicsScene->setModify(true);
    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem(m_pItem);
}

void CSetPolygonAttributeCommand::redo()
{
    m_pItem->setPointCount(m_nNewNum);

    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem(m_pItem);
}

CSetPolygonStarAttributeCommand::CSetPolygonStarAttributeCommand(CDrawScene *scene, CGraphicsPolygonalStarItem *item, int newNum, int newRadius)
    : m_pItem(item)
    , m_nNewNum(newNum)
    , m_nNewRadius(newRadius)
{
    myGraphicsScene = scene;
    m_nOldNum = m_pItem->anchorNum();
    m_nOldRadius = m_pItem->innerRadius();
}

void CSetPolygonStarAttributeCommand::undo()
{
    m_pItem->updatePolygonalStar(m_nOldNum, m_nOldRadius);

    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem(m_pItem);
}

void CSetPolygonStarAttributeCommand::redo()
{
    m_pItem->updatePolygonalStar(m_nNewNum, m_nNewRadius);

    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->setModify(true);
    myGraphicsScene->updateBlurItem(m_pItem);
}

CSetPenAttributeCommand::CSetPenAttributeCommand(CDrawScene *scene, CGraphicsPenItem *item, bool isStart, ELineType type)    : m_pItem(item)
    , m_newStartType(noneLine)
    , m_newEndType(noneLine)
{
    myGraphicsScene = scene;

    if (isStart) {
        m_newStartType = type;
    } else {
        m_newEndType = type;
    }

    m_oldStartType = item->getPenStartType();
    m_oldEndType = item->getPenEndType();
}

void CSetPenAttributeCommand::undo()
{
    m_pItem->setPenStartType(static_cast<ELineType>(m_oldStartType));
    m_pItem->setPenEndType(static_cast<ELineType>(m_oldEndType));

    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}

void CSetPenAttributeCommand::redo()
{
    m_pItem->setPenStartType(static_cast<ELineType>(m_oldStartType));
    m_pItem->setPenEndType(static_cast<ELineType>(m_oldEndType));

    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}

CSetLineAttributeCommand::CSetLineAttributeCommand(CDrawScene *scene, CGraphicsLineItem *item, bool isStart, ELineType type)
    : m_pItem(item)
    , m_newStartType(noneLine)
    , m_newEndType(noneLine)
{
    myGraphicsScene = scene;

    if (isStart) {
        m_newStartType = type;
    } else {
        m_newEndType = type;
    }

    m_oldStartType = item->getLineStartType();
    m_oldEndType = item->getLineEndType();
}

void CSetLineAttributeCommand::undo()
{
    m_pItem->setLineStartType(static_cast<ELineType>(m_oldStartType));
    m_pItem->setLineEndType(static_cast<ELineType>(m_oldEndType));
    myGraphicsScene->updateBlurItem(m_pItem);
}

void CSetLineAttributeCommand::redo()
{
    m_pItem->setLineStartType(static_cast<ELineType>(m_newStartType));
    m_pItem->setLineEndType(static_cast<ELineType>(m_newEndType));
//    m_pItem->update(
//    myGraphicsScene->changeAttribute(true, m_pItem);
    myGraphicsScene->updateBlurItem(m_pItem);
}


COneLayerUpCommand::COneLayerUpCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    myGraphicsScene = scene;
    QList<QGraphicsItem *> curItems;
    for (auto item : scene->items(Qt::AscendingOrder)) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            if (curItems.isEmpty()) {
                if (items.contains(item)) {
                    curItems.append(item);
                }
            } else {
                if (items.contains(item)) {
                    curItems.append(item);
                } else {
                    m_items.append(qMakePair<QGraphicsItem *, QList<QGraphicsItem *> > (item, curItems));
                    curItems.clear();
                }
            }
        }
    }
    m_oldItemZValue.clear();
    m_selectItems = items;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

COneLayerUpCommand::~COneLayerUpCommand()
{

}

void COneLayerUpCommand::undo()
{
    qDebug() << "COneLayerUpCommand undo";
    if (!m_isRedoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> itemList = myGraphicsScene->items();

    m_isRedoExcuteSuccess = false;

    int count = m_oldItemZValue.count();
    for (int i = 0; i < count; i++) {
        QGraphicsItem *item = m_oldItemZValue.keys().at(i);
        item->setZValue(m_oldItemZValue[item]);
        myGraphicsScene->updateBlurItem(item);
        m_isRedoExcuteSuccess = true;
    }
    //重置保存的最大z值
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }

    if (m_isRedoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

void COneLayerUpCommand::redo()
{
    qDebug() << "COneLayerUpCommand redo";
//    qDebug() << "########################upUndo";

    if (!m_isUndoExcuteSuccess) {
        return;
    }

    m_isRedoExcuteSuccess = false;
    //移除z值为0的和多选管理图元
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        if (allItems.at(i)->zValue() == 0.0) {
            allItems.removeAt(i);
            continue;
        }
        if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
            allItems.removeAt(i);
            continue;
        }
    }
    //升序排列
    qSort(m_selectItems.begin(), m_selectItems.end(), zValueSortASC);
    for (int selectIndex = 0; selectIndex < m_selectItems.size(); selectIndex++) {
        QGraphicsItem *selectItem = m_selectItems.at(selectIndex);
        QGraphicsItem *selectItemNext = nullptr;
        if ((selectIndex + 1) < m_selectItems.size()) {
            selectItemNext = m_selectItems.at(selectIndex + 1);
        }
        m_isRedoExcuteSuccess = true;
        qSort(allItems.begin(), allItems.end(), zValueSortASC);
        for (int allItemIndex = 0; allItemIndex < allItems.size(); allItemIndex++) {
            QGraphicsItem *allItem = allItems.at(allItemIndex);
            if (allItem->zValue() > selectItem->zValue()) {
                if (allItem == selectItemNext) {
                    break;
                }
                //记录原来z值
                m_oldItemZValue[allItem] = allItem->zValue();
                m_oldItemZValue[selectItem] = selectItem->zValue();

                qreal tmpZValue = selectItem->zValue();
                selectItem->setZValue(allItem->zValue());
                allItem->setZValue(tmpZValue);
                myGraphicsScene->updateBlurItem(allItem);
                myGraphicsScene->updateBlurItem(selectItem);
                break;
            }
        }
    }
    //重置保存的最大z值
    allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }
    if (m_isRedoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

COneLayerDownCommand::COneLayerDownCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    myGraphicsScene = scene;
    QList<QGraphicsItem *> curItems;
    QGraphicsItem *preItem = nullptr;
    for (auto item : scene->items(Qt::AscendingOrder)) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            if (items.contains(item)) {
                if (preItem != nullptr) {
                    curItems.append(item);
                }
            } else {
                if (!curItems.isEmpty()) {
                    m_items.append(qMakePair<QGraphicsItem *, QList<QGraphicsItem *> > (preItem, curItems));
                    curItems.clear();
                }
                preItem = item;
            }
        }
    }
    if (preItem != nullptr && !curItems.isEmpty()) {
        m_items.append(qMakePair<QGraphicsItem *, QList<QGraphicsItem *> > (preItem, curItems));
    }
    m_oldItemZValue.clear();
    m_selectItems = items;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

COneLayerDownCommand::~COneLayerDownCommand()
{

}

void COneLayerDownCommand::undo()
{
    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!downUndo";
    if (!m_isRedoExcuteSuccess) {
        return;
    }

    m_isRedoExcuteSuccess = false;

    int count = m_oldItemZValue.count();
    for (int i = 0; i < count; i++) {
        QGraphicsItem *item = m_oldItemZValue.keys().at(i);
        item->setZValue(m_oldItemZValue[item]);
        myGraphicsScene->updateBlurItem(item);
        m_isRedoExcuteSuccess = true;
    }
    //重置保存的最大z值
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }

    if (m_isRedoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

void COneLayerDownCommand::redo()
{
    qDebug() << "COneLayerDownCommand::redo";
    if (!m_isUndoExcuteSuccess) {
        return;
    }

    m_isRedoExcuteSuccess = false;
    //移除z值为0的和多选管理图元
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        if (allItems.at(i)->zValue() == 0.0) {
            allItems.removeAt(i);
            continue;
        }
        if (allItems[i]->type() <= QGraphicsItem::UserType || allItems[i]->type() >= EGraphicUserType::MgrType) {
            allItems.removeAt(i);
            int count = allItems.size();
            qDebug() << "count = " << count;
            for (int i = allItems.size() - 1; i >= 0; i--) {
                qDebug() << "i = " << i;
                qDebug() << "allItems zValue = " << allItems.at(i)->zValue();
            }
            continue;
        }
    }
    qSort(m_selectItems.begin(), m_selectItems.end(), zValueSortDES);
    QGraphicsItem *selectItemNext = nullptr;
    for (int selectIndex = 0; selectIndex < m_selectItems.size(); selectIndex++) {
        QGraphicsItem *selectItem = m_selectItems.at(selectIndex);
        selectItemNext = nullptr;
        if ((selectIndex + 1) < m_selectItems.size()) {
            selectItemNext = m_selectItems.at(selectIndex + 1);
        }
        m_isRedoExcuteSuccess = true;
        qSort(allItems.begin(), allItems.end(), zValueSortDES);
        for (int allItemIndex = 0; allItemIndex < allItems.size(); allItemIndex++) {
            QGraphicsItem *allItem = allItems.at(allItemIndex);
            if (allItem->zValue() < selectItem->zValue()) {
                if (allItem == selectItemNext) {
                    break;
                }
                //记录原来z值
                m_oldItemZValue[allItem] = allItem->zValue();
                m_oldItemZValue[selectItem] = selectItem->zValue();

                qreal tmpZValue = selectItem->zValue();
                selectItem->setZValue(allItem->zValue());
                allItem->setZValue(tmpZValue);
                myGraphicsScene->updateBlurItem(allItem);
                myGraphicsScene->updateBlurItem(selectItem);
                break;
            }
        }
    }
    //重置保存的最大z值
    allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }
    if (m_isRedoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

CBringToFrontCommand::CBringToFrontCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    myGraphicsScene = scene;
    for (auto item : scene->items()) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            if (items.contains(item)) {
                m_items.append(item);
            }
        }
    }
    QList<QGraphicsItem *> curItems;
    for (auto item : scene->items(Qt::AscendingOrder)) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            if (curItems.isEmpty()) {
                if (items.contains(item)) {
                    curItems.append(item);
                }
            } else {
                if (items.contains(item)) {
                    curItems.append(item);
                } else {
                    m_changedItems.append(qMakePair<QGraphicsItem *, QList<QGraphicsItem *> > (item, curItems));
                    curItems.clear();
                }
            }
        }
    }
    for (auto item : items) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            m_oldItemZValue[item] = item->zValue();
        }
    }
    m_selectItems = items;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

CBringToFrontCommand::~CBringToFrontCommand()
{

}

void CBringToFrontCommand::undo()
{
    if (!m_isRedoExcuteSuccess) {
        return;
    }

    m_isUndoExcuteSuccess = false;
    int count = m_oldItemZValue.count();
    for (int i = 0; i < count; i++) {
        QGraphicsItem *item = m_oldItemZValue.keys().at(i);
        item->setZValue(m_oldItemZValue[item]);
        myGraphicsScene->updateBlurItem(item);
        m_isRedoExcuteSuccess = true;
    }
    //重置保存的最大z值
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }

    if (m_isUndoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

void CBringToFrontCommand::redo()
{
    if (!m_isUndoExcuteSuccess) {
        return;
    }

    m_isRedoExcuteSuccess = false;

    qSort(m_selectItems.begin(), m_selectItems.end(), zValueSortASC);
    qreal maxZValue = myGraphicsScene->getMaxZValue();
    for (int selectIndex = 0; selectIndex < m_selectItems.size(); selectIndex++) {
        QGraphicsItem *selectItem = m_selectItems.at(selectIndex);
        maxZValue = myGraphicsScene->getMaxZValue();
        selectItem->setZValue(maxZValue + 1);
        myGraphicsScene->setMaxZValue(maxZValue + 1);
        myGraphicsScene->updateBlurItem(selectItem);
        m_isRedoExcuteSuccess = true;
    }
    if (m_isRedoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

CSendToBackCommand::CSendToBackCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    myGraphicsScene = scene;
    for (auto item : scene->items()) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            if (items.contains(item)) {
                m_items.append(item);
            }
        }
    }
    QList<QGraphicsItem *> curItems;
    QGraphicsItem *preItem = nullptr;
    for (auto item : scene->items(Qt::AscendingOrder)) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            if (items.contains(item)) {
                if (preItem != nullptr) {
                    curItems.append(item);
                }
            } else {
                if (!curItems.isEmpty()) {
                    m_changedItems.append(qMakePair<QGraphicsItem *, QList<QGraphicsItem *> > (preItem, curItems));
                    curItems.clear();
                }
                preItem = item;
            }
        }
    }
    if (preItem != nullptr && !curItems.isEmpty()) {
        m_changedItems.append(qMakePair<QGraphicsItem *, QList<QGraphicsItem *> > (preItem, curItems));
    }
    m_oldItemZValue.clear();
    for (auto item : items) {
        if (item->type() > QGraphicsItem::UserType && item->type() < EGraphicUserType::MgrType) {
            m_oldItemZValue[item] = item->zValue();
        }
    }
    m_selectItems = items;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

CSendToBackCommand::~CSendToBackCommand()
{

}

void CSendToBackCommand::undo()
{
    qDebug() << "CSendToBackCommand::undo";
    if (!m_isRedoExcuteSuccess) {
        return;
    }

    m_isUndoExcuteSuccess = false;
    int count = m_oldItemZValue.count();
    for (int i = 0; i < count; i++) {
        QGraphicsItem *item = m_oldItemZValue.keys().at(i);
        item->setZValue(m_oldItemZValue[item]);
        myGraphicsScene->updateBlurItem(item);
        m_isRedoExcuteSuccess = true;
    }
    //重置保存的最大z值
    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            continue;
        }
        if (allItem->zValue() > myGraphicsScene->getMaxZValue()) {
            myGraphicsScene->setMaxZValue(allItem->zValue());
        }
    }
    if (m_isUndoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}

void CSendToBackCommand::redo()
{
    qDebug() << "CSendToBackCommand::redo";
    if (!m_isUndoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> allItems = myGraphicsScene->items();
    for (int i = allItems.size() - 1; i >= 0; i--) {
        QGraphicsItem *allItem = allItems.at(i);
        if (allItems.at(i)->zValue() == 0.0) {
            allItems.removeAt(i);
            continue;
        }
        if (allItem->type() <= QGraphicsItem::UserType || allItem->type() >= EGraphicUserType::MgrType) {
            allItems.removeAt(i);
            continue;
        }
        for (int j = 0; j < m_selectItems.size(); j++) {
            QGraphicsItem *selectItem = m_selectItems.at(j);
            if (allItem == selectItem) {
                allItems.removeAt(i);
                break;
            }
        }
    }

    qSort(m_selectItems.begin(), m_selectItems.end(), zValueSortASC);
    myGraphicsScene->setMaxZValue(m_selectItems.last()->zValue());
    qSort(allItems.begin(), allItems.end(), zValueSortASC);
    for (int allItemIndex = 0; allItemIndex < allItems.size(); allItemIndex++) {
        QGraphicsItem *allItem = allItems.at(allItemIndex);
        m_oldItemZValue[allItem] = allItem->zValue();
        allItem->setZValue(myGraphicsScene->getMaxZValue() + 1);
        myGraphicsScene->setMaxZValue(myGraphicsScene->getMaxZValue() + 1);
        m_isRedoExcuteSuccess = true;
    }

    if (m_isRedoExcuteSuccess) {
        myGraphicsScene->update();
        myGraphicsScene->setModify(true);
    }
}


CSetBlurAttributeCommand::CSetBlurAttributeCommand(CDrawScene *scene, CGraphicsMasicoItem *item, int newType, int newRadio, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_pItem(item)
    , m_nNewType(newType)
    , m_nNewRadius(newRadio)
{
    myGraphicsScene = scene;
    m_nOldType = item->getBlurEffect();
    m_nOldRadius = item->getBlurWidth();
}

void CSetBlurAttributeCommand::undo()
{
    m_pItem->setBlurEffect(static_cast<EBlurEffect>(m_nOldType));
    m_pItem->setBlurWidth(m_nOldRadius);
    myGraphicsScene->setModify(true);
    if (nullptr != m_pItem->scene()) {
        auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
        curScene->changeAttribute(true, m_pItem);
    }
}

void CSetBlurAttributeCommand::redo()
{
    m_pItem->setBlurEffect(static_cast<EBlurEffect>(m_nNewType));
    m_pItem->setBlurWidth(m_nNewRadius);
    myGraphicsScene->setModify(true);
    if (nullptr != m_pItem->scene()) {
        auto curScene = static_cast<CDrawScene *>(m_pItem->scene());
        curScene->changeAttribute(true, m_pItem);
    }
}

CSceneCutCommand::CSceneCutCommand(CDrawScene *scene, QRectF rect, QUndoCommand *parent)
    : QUndoCommand (parent)
    , m_newRect(rect)
{
    myGraphicsScene = scene;
    m_oldRect = scene->sceneRect();
}

CSceneCutCommand::~CSceneCutCommand()
{

}

void CSceneCutCommand::undo()
{
    myGraphicsScene->setSceneRect(m_oldRect);
    myGraphicsScene->update();

    myGraphicsScene->updateBlurItem();
}

void CSceneCutCommand::redo()
{
    myGraphicsScene->setSceneRect(m_newRect);
    myGraphicsScene->update();

    myGraphicsScene->updateBlurItem();
}


CMultResizeShapeCommand::CMultResizeShapeCommand(CDrawScene *scene, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bAltPress, QUndoCommand *parent)
{
    myGraphicsScene = scene;
    m_handle = handle;
    m_endPos = endPos;
    m_beginPos = beginPos;
    m_bShiftPress = bShiftPress;
    m_bAltPress = bAltPress;
    m_offsetPos = m_beginPos - m_endPos;
    m_bResized = false;
}

void CMultResizeShapeCommand::undo()
{
    myGraphicsScene->getItemsMgr()->resizeTo(m_handle, m_beginPos, m_offsetPos, m_bShiftPress, m_bAltPress);
}

void CMultResizeShapeCommand::redo()
{
    if (m_bResized) {
        myGraphicsScene->getItemsMgr()->resizeTo(m_handle, m_endPos, -m_offsetPos, m_bShiftPress, m_bAltPress);
    }
    m_bResized = true;
}

CMultMoveShapeCommand::CMultMoveShapeCommand(CDrawScene *scene, QPointF beginPos, QPointF endPos, QUndoCommand *parent)
{
    myGraphicsScene = scene;
    m_endPos = endPos;
    m_beginPos = beginPos;
    m_bMoved = false;
}

void CMultMoveShapeCommand::undo()
{
    qDebug() << "CMultMoveShapeCommand::undo";
    myGraphicsScene->getItemsMgr()->move(m_endPos, m_beginPos);
}

void CMultMoveShapeCommand::redo()
{
    qDebug() << "CMultMoveShapeCommand::redo";
    if (m_bMoved) {
        myGraphicsScene->getItemsMgr()->move(m_beginPos, m_endPos);
    }
    m_bMoved = true;
}

CSetItemsCommonPropertyValueCommand::CSetItemsCommonPropertyValueCommand(CDrawScene *scene, QList<CGraphicsItem *> items, EDrawProperty property, QVariant value)
{
    myGraphicsScene = scene;
    m_items = items;
    m_property = property;
    m_value = value;
    QVariant oldValue;
    for (auto item : items) {
        switch (m_property) {
        case FillColor:
            oldValue.setValue(item->brush());
            break;
        case LineWidth:
            oldValue.setValue(item->pen().width());
            break;
        case LineColor:
            oldValue.setValue(item->pen().color());
            break;
        case RectRadius:
            oldValue.setValue(static_cast<CGraphicsRectItem *>(item)->getXRedius());
            break;
        case Anchors:
            oldValue.setValue(static_cast<CGraphicsPolygonalStarItem *>(item)->anchorNum());
            break;
        case StarRadius:
            oldValue.setValue(static_cast<CGraphicsPolygonalStarItem *>(item)->innerRadius());
            break;
        case SideNumber:
            oldValue.setValue(static_cast<CGraphicsPolygonItem *>(item)->nPointsCount());
            break;
        case LineStartArrowType:
            oldValue.setValue(static_cast<CGraphicsLineItem *>(item)->getLineStartType());
            break;
        case LineEndArrowType:
            oldValue.setValue(static_cast<CGraphicsLineItem *>(item)->getLineEndType());
            break;
        case PenStartArrowType:
            oldValue.setValue(static_cast<CGraphicsPenItem *>(item)->getPenStartType());
            break;
        case PenEndArrowType:
            oldValue.setValue(static_cast<CGraphicsPenItem *>(item)->getPenEndType());
            break;
        case TextColor:
            // 此处代码需要作其它处理
            oldValue.setValue(static_cast<CGraphicsPenItem *>(item)->getPenStartType());
//            oldValue.setValue(static_cast<CGraphicsPenItem *>(item)->currentType());
            if (item->type() == EGraphicUserType::TextType) {
                oldValue.setValue(static_cast<CGraphicsTextItem *>(item)->getTextColor());
            } else if (item->type() == EGraphicUserType::RectType ||
                       item->type() == EGraphicUserType::EllipseType ||
                       item->type() == EGraphicUserType::TriangleType ||
                       item->type() == EGraphicUserType::PolygonalStarType ||
                       item->type() == EGraphicUserType::PolygonType) {
                QBrush brush = item->brush();
                oldValue.setValue(brush.color());
            }
            break;
        case TextSize:
            oldValue.setValue(static_cast<CGraphicsTextItem *>(item)->getFontSize());
            break;
        default:
            //oldValue.setValue(QMetaType::User);
            break;
        }
        m_oldValues[item] = oldValue;
    }
}

void CSetItemsCommonPropertyValueCommand::undo()
{
    QVariant oldValue;
    for (int i = 0; i < m_items.size(); i++) {
        CGraphicsItem *item = m_items.at(i);
        oldValue = m_oldValues[item];
        switch (m_property) {
        case FillColor:
            item->setBrush(oldValue.value<QBrush>());
            break;
        case LineWidth: {
            QPen widthpen = item->pen();
            widthpen.setWidth(oldValue.toInt());
            item->setPen(widthpen);
            break;
        }
        case LineColor: {
            QPen colorpen = item->pen();
            colorpen.setColor(oldValue.value<QColor>());
            item->setPen(colorpen);
            break;
        }
        case RectRadius:
            static_cast<CGraphicsRectItem *>(item)->setXYRedius(oldValue.toInt(), oldValue.toInt());
            break;
        case Anchors:
            static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(oldValue.toInt(), static_cast<CGraphicsPolygonalStarItem *>(item)->innerRadius());
            break;
        case StarRadius:
            static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(static_cast<CGraphicsPolygonalStarItem *>(item)->innerRadius(), oldValue.toInt());
            break;
        case SideNumber:
            static_cast<CGraphicsPolygonItem *>(item)->setPointCount(oldValue.toInt());
            break;
        case LineStartArrowType:
            static_cast<CGraphicsLineItem *>(item)->setLineStartType(oldValue.value<ELineType>());
            break;
        case LineEndArrowType:
            static_cast<CGraphicsLineItem *>(item)->setLineEndType(oldValue.value<ELineType>());
            break;
        case PenStartArrowType:
            static_cast<CGraphicsPenItem *>(item)->setPenStartType(oldValue.value<ELineType>());
            break;
        case PenEndArrowType:
            static_cast<CGraphicsPenItem *>(item)->setPenEndType(oldValue.value<ELineType>());
            break;
        case TextColor:
            if (item->type() == EGraphicUserType::TextType) {
                static_cast<CGraphicsTextItem *>(item)->setTextColor(oldValue.value<QColor>());
            } else if (item->type() == EGraphicUserType::RectType ||
                       item->type() == EGraphicUserType::EllipseType ||
                       item->type() == EGraphicUserType::TriangleType ||
                       item->type() == EGraphicUserType::PolygonalStarType ||
                       item->type() == EGraphicUserType::PolygonType) {
                QBrush brush = item->brush();
                brush.setColor(oldValue.value<QColor>());
                item->setBrush(brush);
            }
            break;
        case TextSize:
            static_cast<CGraphicsTextItem *>(item)->setFontSize(oldValue.value<qreal>());
            break;
        default:
            break;
        }
        item->update();
    }
    myGraphicsScene->update();
}

void CSetItemsCommonPropertyValueCommand::redo()
{
    for (int i = 0; i < m_items.size(); i++) {
        CGraphicsItem *item = m_items.at(i);
        switch (m_property) {
        case FillColor:
            item->setBrush(m_value.value<QBrush>());
            break;
        case LineWidth: {
            QPen widthpen = item->pen();
            widthpen.setWidth(m_value.toInt());
            item->setPen(widthpen);
            break;
        }
        case LineColor: {
            QPen colorpen = item->pen();
            colorpen.setColor(m_value.value<QColor>());
            item->setPen(colorpen);
            break;
        }
        case RectRadius:
            static_cast<CGraphicsRectItem *>(item)->setXYRedius(m_value.toInt(), m_value.toInt());
            break;
        case Anchors:
            static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(m_value.toInt(), static_cast<CGraphicsPolygonalStarItem *>(item)->innerRadius());
            break;
        case StarRadius:
            static_cast<CGraphicsPolygonalStarItem *>(item)->updatePolygonalStar(static_cast<CGraphicsPolygonalStarItem *>(item)->anchorNum(), m_value.toInt());
            break;
        case SideNumber:
            static_cast<CGraphicsPolygonItem *>(item)->setPointCount(m_value.toInt());
            break;
        case LineStartArrowType:
            static_cast<CGraphicsLineItem *>(item)->setLineStartType(m_value.value<ELineType>());
            break;
        case LineEndArrowType:
            static_cast<CGraphicsLineItem *>(item)->setLineEndType(m_value.value<ELineType>());
            break;
        case PenStartArrowType:
            static_cast<CGraphicsPenItem *>(item)->setPenStartType(m_value.value<ELineType>());
            break;
        case PenEndArrowType:
            static_cast<CGraphicsPenItem *>(item)->setPenEndType(m_value.value<ELineType>());
            break;
        case TextColor:
            if (item->type() == EGraphicUserType::TextType) {
                static_cast<CGraphicsTextItem *>(item)->setTextColor(m_value.value<QColor>());
            } else if (item->type() == EGraphicUserType::RectType ||
                       item->type() == EGraphicUserType::EllipseType ||
                       item->type() == EGraphicUserType::TriangleType ||
                       item->type() == EGraphicUserType::PolygonalStarType ||
                       item->type() == EGraphicUserType::PolygonType) {
                QBrush brush = item->brush();
                brush.setColor(m_value.value<QColor>());
                item->setBrush(brush);
            }
            break;
        case TextSize:
            static_cast<CGraphicsTextItem *>(item)->setFontSize(m_value.value<qreal>());
            break;
        default:
            break;
        }
        item->update();
    }
    myGraphicsScene->update();
}
