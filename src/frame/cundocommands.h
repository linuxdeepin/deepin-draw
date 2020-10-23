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
 * @brief The CRotateShapeCommand class 旋转撤销重做命令
 */
class CRotateShapeCommand : public QUndoCommand
{
public:
    CRotateShapeCommand(CDrawScene *scene, QGraphicsItem *item, const qreal oldAngle,
                        QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QGraphicsItem *myItem;
    qreal myOldAngle;
    qreal newAngle;
};

/**
 * @brief The CSetPropertyCommand class 设置图元属性撤消重做命令
 */
class CSetPropertyCommand : public QUndoCommand
{
public:
    explicit CSetPropertyCommand(CDrawScene *scene, CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange, QUndoCommand *parent = nullptr);
    ~CSetPropertyCommand() override;

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsItem *m_pItem;
    QPen m_oldPen;
    QBrush m_oldBrush;
    QPen m_newPen;
    QBrush m_newBrush;
    bool m_bPenChange;
    bool m_bBrushChange;
};

/**
 * @brief The CSetRectXRediusCommand class 设置矩形圆角属性撤消重做命令
 */
class CSetRectXRediusCommand : public QUndoCommand
{
public:
    explicit CSetRectXRediusCommand(CDrawScene *scene, CGraphicsRectItem *item, int redius, bool bRediusChange, QUndoCommand *parent = nullptr);
    ~CSetRectXRediusCommand() override;

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsRectItem *m_pItem;
    int m_oldRectXRedius;
    int m_newRectXRedius;
    bool m_bRectXRediusChange;
};

/**
 * @brief The CSetPolygonAttributeCommand class 设置多边形撤消重做命令
 */
class CSetPolygonAttributeCommand : public QUndoCommand
{
public:
    explicit CSetPolygonAttributeCommand(CDrawScene *scene, CGraphicsPolygonItem *item, int newNum);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsPolygonItem *m_pItem;
    int m_nOldNum;
    int m_nNewNum;
};

/**
 * @brief The CSetPolygonStarAttributeCommand class 设置星形属性撤消重做命令
 */
class CSetPolygonStarAttributeCommand : public QUndoCommand
{
public:
    explicit CSetPolygonStarAttributeCommand(CDrawScene *scene, CGraphicsPolygonalStarItem *item, int newNum, int newRadius);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsPolygonalStarItem *m_pItem;
    int m_nOldNum;
    int m_nNewNum;
    int m_nOldRadius;
    int m_nNewRadius;
};

/**
 * @brief The CSetPenAttributeCommand class 设置画笔属性命令
 */
class CSetPenAttributeCommand : public QUndoCommand
{
public:
    explicit CSetPenAttributeCommand(CDrawScene *scene, CGraphicsPenItem *item, bool isStart, ELineType type);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsPenItem *m_pItem;
    ELineType m_newStartType; // 起始点样式
    ELineType m_newEndType; // 终点样式
    ELineType m_oldStartType; // 起始点样式
    ELineType m_oldEndType; // 终点样式
};

/**
 * @brief The CSetLineAttributeCommand class 线属性设置
 */
class CSetLineAttributeCommand : public QUndoCommand
{
public:
    explicit CSetLineAttributeCommand(CDrawScene *scene, CGraphicsLineItem *item, bool isStart, ELineType type);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsLineItem *m_pItem;
    ELineType m_newStartType; // 起始点样式
    ELineType m_newEndType; // 终点样式
    ELineType m_oldStartType; // 起始点样式
    ELineType m_oldEndType; // 终点样式
    bool m_isStart;
};

/**
 * @brief The COneLayerUpCommand class 图元向上一层撤消重做命令
 */
class COneLayerUpCommand : public QUndoCommand
{
public:
    COneLayerUpCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);
    ~COneLayerUpCommand() override;

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
    ~CBringToFrontCommand() override;

    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *>>> m_changedItems;
    QList<QGraphicsItem *> m_items;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    QList<QGraphicsItem *> m_selectItems;
    //    bool m_isRedoExcuteSuccess;
    //    bool m_isUndoExcuteSuccess;
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
 * @brief The CSetBlurAttributeCommand class 模糊图元撤消重做命令
 */
class CSetBlurAttributeCommand : public QUndoCommand
{
public:
    explicit CSetBlurAttributeCommand(CDrawScene *scene, CGraphicsMasicoItem *item, int newType, int newRadio, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsMasicoItem *m_pItem;
    int m_nOldType;
    int m_nNewType;
    int m_nOldRadius;
    int m_nNewRadius;
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

/**
 * @brief The CItemRotationCommand class  设置图元旋转
 */
class CItemRotationCommand : public QUndoCommand
{
public:
    /*
    * @bref: CItemsRotationCommand 设置图元旋转
    * @param: QMap<CGraphicsItem *, QMap<ERotationType, QVariant>> 单个图元对应的属性
    */
    CItemRotationCommand(CDrawScene *scene, CGraphicsItem *item,
                         //                         ERotationType startType,
                         ERotationType endType);
    void undo() override;
    void redo() override;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsItem *m_item;
    ERotationType m_startType;
    ERotationType m_endType;
};
#endif // CUNDOCOMMANDS_H
