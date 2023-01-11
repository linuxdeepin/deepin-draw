/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "globaldefine.h"
#include "pagecontext.h"
#include "pagescene.h"
#include "pageview.h"

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QBrush>
#include <QDebug>

CSceneCutCommand::CSceneCutCommand(PageScene *scene, QRectF rect, QUndoCommand *parent, PageItem *item)
    : QUndoCommand(parent)
    , m_newRect(rect)
    , m_oldRect(scene->sceneRect())
{
    myGraphicsScene = scene;
    m_item = item;
}

CSceneCutCommand::~CSceneCutCommand()
{
}

void CSceneCutCommand::undo()
{
    myGraphicsScene->setSceneRect(m_oldRect);
    myGraphicsScene->update();
}

void CSceneCutCommand::redo()
{
    myGraphicsScene->setSceneRect(m_newRect);
    myGraphicsScene->update();
}

CItemsAlignCommand::CItemsAlignCommand(PageScene *scene, QMap<PageItem *, QPointF> startPos,
                                       QMap<PageItem *, QPointF> endPos)
    : myGraphicsScene(scene)
    , m_itemsStartPos(startPos)
    , m_itemsEndPos(endPos)
    , m_isMoved(true)
{
}

void CItemsAlignCommand::undo()
{
    qDebug() << "CItemsAlignCommand::undo";
    if (m_isMoved) {
        QList<PageItem *> allItems = m_itemsStartPos.keys();
        QList<QPointF> itemsStartPos = m_itemsStartPos.values();
        QList<QPointF> itemsEndPos = m_itemsEndPos.values();

        for (int i = 0; i < allItems.size(); i++) {
            allItems.at(i)->move(itemsEndPos.at(i), itemsStartPos.at(i));
        }
    }
    m_isMoved = false;
}

void CItemsAlignCommand::redo()
{
    qDebug() << "CItemsAlignCommand::redo";
    if (!m_isMoved) {
        QList<PageItem *> allItems = m_itemsStartPos.keys();
        QList<QPointF> itemsStartPos = m_itemsStartPos.values();
        QList<QPointF> itemsEndPos = m_itemsEndPos.values();

        for (int i = 0; i < allItems.size(); i++) {
            allItems.at(i)->move(itemsStartPos.at(i), itemsEndPos.at(i));
        }
    }
    m_isMoved = true;
}
