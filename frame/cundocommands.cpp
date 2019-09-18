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
#include "cundocommands.h"
#include "drawshape/globaldefine.h"
#include "drawshape/cgraphicslineitem.h"
#include "drawshape/cgraphicsrectitem.h"
#include "drawshape/cgraphicspolygonitem.h"
#include "drawshape/cgraphicspolygonalstaritem.h"
#include <QUndoCommand>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDebug>

CMoveShapeCommand::CMoveShapeCommand(QGraphicsScene *graphicsScene, const QPointF &delta, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myItem = nullptr;
    myItems = graphicsScene->selectedItems();
    myGraphicsScene = graphicsScene;
    myDelta = delta;
    bMoved = true;
}

CMoveShapeCommand::CMoveShapeCommand(QGraphicsItem *item, const QPointF &delta, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = nullptr;
    myItem = item;
    myDelta = delta;
    bMoved = true;
}

//! [2]
void CMoveShapeCommand::undo()
{
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
}
//! [2]

//! [3]
void CMoveShapeCommand::redo()
{
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
}
//! [3]
//! [4]
CRemoveShapeCommand::CRemoveShapeCommand(QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = scene;
    items = myGraphicsScene->selectedItems();
}

CRemoveShapeCommand::~CRemoveShapeCommand()
{

}
//! [4]

//! [5]
void CRemoveShapeCommand::undo()
{
    foreach (QGraphicsItem *item, items) {
//        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(item->parentItem());
//        if ( g != nullptr )
        myGraphicsScene->addItem(item);
    }
    myGraphicsScene->update();
    //setText(QObject::tr("Undo Delete %1").arg(items.count()));
}
//! [5]

//! [6]
void CRemoveShapeCommand::redo()
{
    foreach (QGraphicsItem *item, items) {
//        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup *>(item->parentItem());
//        if ( g != nullptr )
        myGraphicsScene->removeItem(item);
    }
    //setText(QObject::tr("Redo Delete %1").arg(items.count()));
}

//! [6]

//! [7]

CAddShapeCommand::CAddShapeCommand(QGraphicsItem *item,
                                   QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    static int itemCount = 0;

    myGraphicsScene = scene;
    myDiagramItem = item;
    initialPosition = item->pos();
    ++itemCount;
}
//! [7]

CAddShapeCommand::~CAddShapeCommand()
{
    if (!myDiagramItem->scene())
        delete myDiagramItem;
}

//! [8]
void CAddShapeCommand::undo()
{
    myGraphicsScene->removeItem(myDiagramItem);
    myGraphicsScene->update();
}
//! [8]

//! [9]
void CAddShapeCommand::redo()
{
    if ( myDiagramItem->scene() == nullptr )
        myGraphicsScene->addItem(myDiagramItem);
    myDiagramItem->setPos(initialPosition);
    myGraphicsScene->update();
}

/*

QString createCommandString(QGraphicsItem *item, const QPointF &pos)
{
    return QObject::tr("Item at (%1, %2)")
           .arg(pos.x()).arg(pos.y());
}
*/

CRotateShapeCommand::CRotateShapeCommand(QGraphicsItem *item, const qreal oldAngle, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myItem = item;
    myOldAngle = oldAngle;
    newAngle = item->rotation();

}

void CRotateShapeCommand::undo()
{
    myItem->setRotation(myOldAngle);
    myItem->scene()->update();
//    setText(QObject::tr("Undo Rotate %1").arg(newAngle));
}

void CRotateShapeCommand::redo()
{
    myItem->setRotation(newAngle);
    myItem->update();
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


CResizeShapeCommand::CResizeShapeCommand(CGraphicsItem *item, CSizeHandleRect::EDirection handle, QPointF beginPos, QPointF endPos, bool bShiftPress, bool bALtPress, QUndoCommand *parent)
    : QUndoCommand(parent)
    , myItem(item)
    , m_handle(handle)
    , m_beginPos(beginPos)
    , m_endPos(endPos)
    , m_bShiftPress(bShiftPress)
    , m_bAltPress(bALtPress)
{

}

void CResizeShapeCommand::undo()
{
    myItem->resizeTo(m_handle,  m_beginPos, m_bShiftPress, m_bAltPress);
    myItem->update();
}

void CResizeShapeCommand::redo()
{
    myItem->resizeTo(m_handle, m_endPos, m_bShiftPress, m_bAltPress);
    myItem->update();
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


CSetPropertyCommand::CSetPropertyCommand(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange, QUndoCommand *parent)
    : QUndoCommand (parent)
    , m_pItem(item)
    , m_oldPen(pen)
    , m_oldBrush(brush)
    , m_bPenChange(bPenChange)
    , m_bBrushChange(bBrushChange)
{
    m_newPen = item->pen();
    m_newBrush = item->brush();
}

CSetPropertyCommand::~CSetPropertyCommand()
{

}

void CSetPropertyCommand::undo()
{
    if (m_bPenChange) {
        m_pItem->setPen(m_oldPen);
    }

    if (m_bBrushChange) {
        m_pItem->setBrush(m_oldBrush);
    }
}

void CSetPropertyCommand::redo()
{
    if (m_bPenChange) {
        m_pItem->setPen(m_newPen);
    }

    if (m_bBrushChange) {
        m_pItem->setBrush(m_newBrush);
    }
}

CSetPolygonAttributeCommand::CSetPolygonAttributeCommand(CGraphicsPolygonItem *item, int oldNum)
    : m_pItem(item)
    , m_nOldNum(oldNum)
{
    m_nNewNum = m_pItem->nPointsCount();
}

void CSetPolygonAttributeCommand::undo()
{
    m_pItem->setPointCount(m_nOldNum);
}

void CSetPolygonAttributeCommand::redo()
{
    m_pItem->setPointCount(m_nNewNum);
}

CSetPolygonStarAttributeCommand::CSetPolygonStarAttributeCommand(CGraphicsPolygonalStarItem *item, int oldNum, int oldRadius)
    : m_pItem(item)
    , m_nOldNum(oldNum)
    , m_nOldRadius(oldRadius)
{
    m_nNewNum = m_pItem->anchorNum();
    m_nNewRadius = m_pItem->innerRadius();
}

void CSetPolygonStarAttributeCommand::undo()
{
    m_pItem->updatePolygonalStar(m_nOldNum, m_nOldRadius);
}

void CSetPolygonStarAttributeCommand::redo()
{
    m_pItem->updatePolygonalStar(m_nNewNum, m_nNewRadius);
}






COneLayerUpCommand::COneLayerUpCommand(QGraphicsItem *selectedItem, QGraphicsScene *graphicsScene, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    m_selectedItem = selectedItem;
    m_scene = graphicsScene;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

COneLayerUpCommand::~COneLayerUpCommand()
{

}

void COneLayerUpCommand::undo()
{
//    qDebug() << "$$$$$$$$$$$$$$$$$$$$$$$$$upUndo";
    if (!m_isRedoExcuteSuccess) {
        return;
    }


    QList<QGraphicsItem *> itemList = m_scene->items();

//    for (int i = 0; i < itemList.length(); i++) {
//        qDebug() << "@@@@@@@@@@@@@type=" << itemList.at(i)->type() << "::i=" << i;
//    }

    int index = itemList.indexOf(m_selectedItem);

    m_isUndoExcuteSuccess = false;

    for (int i = index + 1; i < itemList.length() ; i++) {

        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            m_selectedItem->stackBefore(itemList.at(i));
            m_isUndoExcuteSuccess = true;
            break;
        }
    }

    if (m_isUndoExcuteSuccess) {
        m_scene->update();
    }
}

void COneLayerUpCommand::redo()
{
//    qDebug() << "########################upUndo";

    if (!m_isUndoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isRedoExcuteSuccess = false;
    for (int i = index - 1 ; i >= 0 ; i--) {
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            itemList.at(i)->stackBefore(m_selectedItem);
            m_isRedoExcuteSuccess = true;
            break;
        }
    }
    if (m_isRedoExcuteSuccess) {
        m_scene->update();
    }
}

COneLayerDownCommand::COneLayerDownCommand(QGraphicsItem *selectedItem, QGraphicsScene *graphicsScene, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    m_selectedItem = selectedItem;
    m_scene = graphicsScene;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

COneLayerDownCommand::~COneLayerDownCommand()
{

}

void COneLayerDownCommand::undo()
{
//    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!downUndo";
    if (!m_isRedoExcuteSuccess) {
        return;
    }


    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isUndoExcuteSuccess = false;
    for (int i = index - 1 ; i >= 0 ; i--) {
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            itemList.at(i)->stackBefore(m_selectedItem);
            m_isUndoExcuteSuccess = true;
            break;
        }
    }
    if (m_isUndoExcuteSuccess) {
        m_scene->update();
    }
}

void COneLayerDownCommand::redo()
{
//    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!downRedo";
    if (!m_isUndoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isRedoExcuteSuccess = false;

    for (int i = index + 1; i < itemList.length() ; i++) {

        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            m_selectedItem->stackBefore(itemList.at(i));
            m_isRedoExcuteSuccess = true;
            break;
        }
    }

    if (m_isRedoExcuteSuccess) {
        m_scene->update();
    }
}

CBringToFrontCommand::CBringToFrontCommand(QGraphicsItem *selectedItem, QGraphicsScene *graphicsScene, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    m_selectedItem = selectedItem;
    m_scene = graphicsScene;
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

    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isUndoExcuteSuccess = false;
    for (int i = index + 1; i < itemList.length() ; i++) {
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            m_selectedItem->stackBefore(itemList.at(i));
            m_isUndoExcuteSuccess = true;
        }
    }

    if (m_isUndoExcuteSuccess) {
        m_scene->update();
    }
}

void CBringToFrontCommand::redo()
{
    if (!m_isUndoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isRedoExcuteSuccess = false;
    for (int i = index - 1; i >= 0 ; i--) {
//        qDebug() << "@@@@@@@@@item=" << itemList.at(i)->type() << "zValue=" << "i=" << i << "::" << itemList.at(i)->zValue();
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            itemList.at(i)->stackBefore(m_selectedItem);
            m_isRedoExcuteSuccess = true;
        }
    }

    if (m_isRedoExcuteSuccess) {
        m_scene->update();
    }
}

CSendToBackCommand::CSendToBackCommand(QGraphicsItem *selectedItem, QGraphicsScene *graphicsScene, QUndoCommand *parent)
    : QUndoCommand (parent)
{
    m_selectedItem = selectedItem;
    m_scene = graphicsScene;
    m_isUndoExcuteSuccess = true;
    m_isRedoExcuteSuccess = false;
}

CSendToBackCommand::~CSendToBackCommand()
{

}

void CSendToBackCommand::undo()
{
    if (!m_isRedoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isUndoExcuteSuccess = false;
    for (int i = index - 1; i >= 0 ; i--) {
//        qDebug() << "@@@@@@@@@item=" << itemList.at(i)->type() << "zValue=" << "i=" << i << "::" << itemList.at(i)->zValue();
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            itemList.at(i)->stackBefore(m_selectedItem);
            m_isUndoExcuteSuccess = true;
        }
    }

    if (m_isUndoExcuteSuccess) {
        m_scene->update();
    }
}

void CSendToBackCommand::redo()
{
    if (!m_isUndoExcuteSuccess) {
        return;
    }

    QList<QGraphicsItem *> itemList = m_scene->items();

    int index = itemList.indexOf(m_selectedItem);

    m_isRedoExcuteSuccess = false;
    for (int i = index + 1; i < itemList.length() ; i++) {
        if (itemList.at(i)->type() > QGraphicsItem::UserType) {
            m_selectedItem->stackBefore(itemList.at(i));
            m_isRedoExcuteSuccess = true;
        }
    }

    if (m_isRedoExcuteSuccess) {
        m_scene->update();
    }
}
