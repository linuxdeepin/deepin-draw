// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
//class CGraphicsMasicoItem;
class PageScene;
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
    COneLayerUpCommand(PageScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    PageScene *myGraphicsScene;
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
    COneLayerDownCommand(PageScene *scene, const QList<QGraphicsItem *> &items,
                         QUndoCommand *parent = nullptr);
    ~COneLayerDownCommand() override;

    void undo() override;
    void redo() override;

private:
    PageScene *myGraphicsScene;
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
    CBringToFrontCommand(PageScene *scene, const QList<QGraphicsItem *> &items,
                         QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    PageScene *myGraphicsScene;
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
    CSendToBackCommand(PageScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);
    ~CSendToBackCommand() override;

    void undo() override;
    void redo() override;

private:
    PageScene *myGraphicsScene;
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
    CSceneCutCommand(PageScene *scene, QRectF rect, QUndoCommand *parent = nullptr, CGraphicsItem *item = nullptr);
    ~CSceneCutCommand() override;

    void undo() override;
    void redo() override;

private:
    PageScene *myGraphicsScene;
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
    CItemsAlignCommand(PageScene *scene,
                       QMap<CGraphicsItem *, QPointF> startPos,
                       QMap<CGraphicsItem *, QPointF> endPos);
    void undo() override;
    void redo() override;

private:
    PageScene *myGraphicsScene;
    QMap<CGraphicsItem *, QPointF> m_itemsStartPos;
    QMap<CGraphicsItem *, QPointF> m_itemsEndPos;
    bool m_isMoved = false;
};
#endif // CUNDOCOMMANDS_H
