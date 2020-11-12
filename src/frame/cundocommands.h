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
#ifndef CUNDOCOMMANDS_H
#define CUNDOCOMMANDS_H

#include "csizehandlerect.h"
#include "drawshape/globaldefine.h"

#include <QUndoCommand>
#include <QPointF>
#include <QList>
#include <QBrush>
#include <QPen>

class QGraphicsScene;
class QGraphicsItem;
class CGraphicsItem;
class CGraphicsRectItem;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;
class CGraphicsPenItem;
class CGraphicsLineItem;
class CGraphicsMasicoItem;
class CDrawScene;
class CGraphicsTextItem;
class CTextEdit;

Q_DECLARE_METATYPE(ELineType);
Q_DECLARE_METATYPE(EPenType);


/**
 * @brief The COneLayerUpCommand class 图元向上一层撤消重做命令
 */
class COneLayerUpCommand : public QUndoCommand
{
public:
    COneLayerUpCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *>>> m_items;
    QList<QGraphicsItem *> m_selectItems;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    bool m_isRedoExcuteSuccess;
    bool m_isUndoExcuteSuccess;
};

/**
 * @brief The COneLayerDownCommand class 图元向下一层撤消重做命令
 */
class COneLayerDownCommand : public QUndoCommand
{
public:
    COneLayerDownCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                         QUndoCommand *parent = nullptr);
    ~COneLayerDownCommand() override;

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *>>> m_items;
    QList<QGraphicsItem *> m_selectItems;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    bool m_isRedoExcuteSuccess;
    bool m_isUndoExcuteSuccess;
};

/**
 * @brief The CBringToFrontCommand class 图元置顶撤消重做命令graphicsScene
 */
class CBringToFrontCommand : public QUndoCommand
{
public:
    CBringToFrontCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                         QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *>>> m_changedItems;
    QList<QGraphicsItem *> m_items;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    QList<QGraphicsItem *> m_selectItems;
};

/**
 * @brief The CSendToBackCommand class 图元置底撤消重做命令
 */
class CSendToBackCommand : public QUndoCommand
{
public:
    CSendToBackCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);
    ~CSendToBackCommand() override;

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *>>> m_changedItems;
    QList<QGraphicsItem *> m_items;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    QList<QGraphicsItem *> m_selectItems;
    QVector<QGraphicsItem *> m_movedItems;
};

/**
 * @brief The CSceneCutCommand class  裁剪撤消重做命令
 */
class CSceneCutCommand : public QUndoCommand
{
public:
    CSceneCutCommand(CDrawScene *scene, QRectF rect, QUndoCommand *parent = nullptr, CGraphicsItem *item = nullptr);
    ~CSceneCutCommand() override;

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QRectF m_newRect;
    QRectF m_oldRect;
    CGraphicsItem *m_item;
};

/**
 * @brief The CItemsAlignCommand class  设置图元对齐
 */
class CItemsAlignCommand : public QUndoCommand
{
public:
    CItemsAlignCommand(CDrawScene *scene,
                       QMap<CGraphicsItem *, QPointF> startPos,
                       QMap<CGraphicsItem *, QPointF> endPos);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QMap<CGraphicsItem *, QPointF> m_itemsStartPos;
    QMap<CGraphicsItem *, QPointF> m_itemsEndPos;
    bool m_isMoved = false;
};
#endif // CUNDOCOMMANDS_H
