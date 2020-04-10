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

#include "drawshape/csizehandlerect.h"
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

Q_DECLARE_METATYPE(ELineType);
Q_DECLARE_METATYPE(EPenType);
//Q_DECLARE_METATYPE(QMetaType);

/**
 * @brief The CMoveShapeCommand class 移动图元撤消重做命令
 */
class CMoveShapeCommand : public QUndoCommand
{
public:
    CMoveShapeCommand(CDrawScene *scene, const QPointF &delta, QUndoCommand *parent = nullptr);
    CMoveShapeCommand(CDrawScene *scene, QGraphicsItem *item, const QPointF &delta, QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
private:
    CDrawScene *myGraphicsScene;
    QGraphicsItem  *myItem;
    QList<QGraphicsItem *> myItems;
    QPointF myDelta;
    bool bMoved;
};

/**
 * @brief The CResizeShapeCommand class  拉伸图元撤消重做命令
 */
class CResizeShapeCommand : public QUndoCommand
{
public:
    CResizeShapeCommand(CDrawScene *scene,
                        CGraphicsItem *item,
                        CSizeHandleRect::EDirection handle,
                        QPointF beginPos,
                        QPointF endPos,
                        bool bShiftPress,
                        bool bAltPress,
                        QUndoCommand *parent = nullptr );
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsItem  *myItem;
    CSizeHandleRect::EDirection m_handle;
    QPointF m_beginPos;
    QPointF m_endPos;
    bool m_bShiftPress;
    bool m_bAltPress;
};

/**
 * @brief The CMultResizeShapeCommand class  拉伸多图元撤消重做命令
 */
class CMultResizeShapeCommand : public QUndoCommand
{
public:
    CMultResizeShapeCommand(CDrawScene *scene,
                            CSizeHandleRect::EDirection handle,
                            QPointF beginPos,
                            QPointF endPos,
                            bool bShiftPress,
                            bool bAltPress,
                            QUndoCommand *parent = nullptr );
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    CSizeHandleRect::EDirection m_handle;
    QPointF m_beginPos;
    QPointF m_endPos;
    QPointF m_offsetPos;
    bool m_bShiftPress;
    bool m_bAltPress;
    bool m_bResized;
    QList<CGraphicsItem * > m_listItems;
};

/**
 * @brief The CMultMoveShapeCommand class  移动多图元撤消重做命令
 */
class CMultMoveShapeCommand : public QUndoCommand
{
public:
    CMultMoveShapeCommand(CDrawScene *scene,
                          QPointF beginPos,
                          QPointF endPos,
                          QUndoCommand *parent = nullptr );
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QPointF m_beginPos;
    QPointF m_endPos;
    bool m_bMoved;
    QList<CGraphicsItem * > m_listItems;
};

/*

class ControlShapeCommand : public QUndoCommand
{
public:
    enum { Id = 1235, };
    ControlShapeCommand(QGraphicsItem *item,
                        int handle,
                        const QPointF &newPos,
                        const QPointF &lastPos,
                        QUndoCommand *parent = nullptr );
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

    bool mergeWith(const QUndoCommand *command) Q_DECL_OVERRIDE;
    int id() const Q_DECL_OVERRIDE
    {
        return Id;
    }

private:
    QGraphicsItem  *myItem;
    int handle_;
    QPointF lastPos_;
    QPointF newPos_;
    bool bControled;
};

*/

/**
 * @brief The CRotateShapeCommand class 旋转撤销重做命令
 */
class CRotateShapeCommand : public QUndoCommand
{
public:
    CRotateShapeCommand(CDrawScene *scene, QGraphicsItem *item, const qreal oldAngle,
                        QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
private:
    CDrawScene *myGraphicsScene;
    QGraphicsItem *myItem;
    qreal myOldAngle;
    qreal newAngle;
};

/**
 * @brief The CDeleteShapeCommand class 全选删除图元撤消重做命令
 */
class CDeleteShapeCommand : public QUndoCommand
{
public:
    explicit CDeleteShapeCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items, QUndoCommand *parent = nullptr);
    ~CDeleteShapeCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<QGraphicsItem *> m_items;
    int m_oldIndex;
};


/**
 * @brief The CRemoveShapeCommand class 删除单个图元撤消重做命令
 */
class CRemoveShapeCommand : public QUndoCommand
{
public:
    explicit CRemoveShapeCommand(CDrawScene *scene, QUndoCommand *parent = nullptr);
    ~CRemoveShapeCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<QGraphicsItem *> items;
};

/**
 * @brief The CSetPropertyCommand class 设置图元属性撤消重做命令
 */
class CSetPropertyCommand : public QUndoCommand
{
public:
    explicit CSetPropertyCommand(CDrawScene *scene, CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange, QUndoCommand *parent = nullptr);
    ~CSetPropertyCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;


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
    ~CSetRectXRediusCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;


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
class CSetPolygonAttributeCommand: public QUndoCommand
{
public:
    explicit CSetPolygonAttributeCommand(CDrawScene *scene, CGraphicsPolygonItem *item, int newNum);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsPolygonItem *m_pItem;
    int m_nOldNum;
    int m_nNewNum;
};

/**
 * @brief The CSetPolygonStarAttributeCommand class 设置星形属性撤消重做命令
 */
class CSetPolygonStarAttributeCommand: public QUndoCommand
{
public:
    explicit CSetPolygonStarAttributeCommand(CDrawScene *scene, CGraphicsPolygonalStarItem *item, int newNum, int newRadius);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

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
class CSetPenAttributeCommand: public QUndoCommand
{
public:
    explicit CSetPenAttributeCommand(CDrawScene *scene, CGraphicsPenItem *item, bool isStart, ELineType type);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

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
class CSetLineAttributeCommand: public QUndoCommand
{
public:
    explicit CSetLineAttributeCommand(CDrawScene *scene, CGraphicsLineItem *item, bool isStart, ELineType type);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    CGraphicsLineItem *m_pItem;
    ELineType m_newStartType; // 起始点样式
    ELineType m_newEndType; // 终点样式
    ELineType m_oldStartType; // 起始点样式
    ELineType m_oldEndType; // 终点样式
    bool m_isStart;
};


/*
class GroupShapeCommand : public QUndoCommand
{
public:
    explicit GroupShapeCommand( QGraphicsItemGroup *group, QGraphicsScene *graphicsScene,
                                QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
private:
    QList<QGraphicsItem *> items;
    QGraphicsItemGroup *myGroup;
    QGraphicsScene *myGraphicsScene;
    bool b_undo;
};

class UnGroupShapeCommand : public QUndoCommand
{
public:
    explicit UnGroupShapeCommand( QGraphicsItemGroup *group, QGraphicsScene *graphicsScene,
                                  QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
private:
    QList<QGraphicsItem *> items;
    QGraphicsItemGroup *myGroup;
    QGraphicsScene *myGraphicsScene;
};*/

/**
 * @brief The CAddShapeCommand class 新增图元撤消重做命令
 */
class CAddShapeCommand : public QUndoCommand
{
public:
    CAddShapeCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                     QUndoCommand *parent = nullptr);
    ~CAddShapeCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    //QGraphicsItem *myDiagramItem;
    QList<QGraphicsItem *> m_items;
    QPointF initialPosition;
};

/**
 * @brief The COneLayerUpCommand class 图元向上一层撤消重做命令
 */
class COneLayerUpCommand : public QUndoCommand
{
public:
    COneLayerUpCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);
    ~COneLayerUpCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *> > > m_items;
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
    ~COneLayerDownCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *> > > m_items;
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
    ~CBringToFrontCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *> > > m_changedItems;
    QList<QGraphicsItem *> m_items;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    QList<QGraphicsItem *> m_selectItems;
    bool m_isRedoExcuteSuccess;
    bool m_isUndoExcuteSuccess;
};

/**
 * @brief The CSendToBackCommand class 图元置底撤消重做命令
 */
class CSendToBackCommand : public QUndoCommand
{
public:
    CSendToBackCommand(CDrawScene *scene, const QList<QGraphicsItem *> &items,
                       QUndoCommand *parent = nullptr);
    ~CSendToBackCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<QPair<QGraphicsItem *, QList<QGraphicsItem *> > > m_changedItems;
    QList<QGraphicsItem *> m_items;
    QMap<QGraphicsItem *, qreal> m_oldItemZValue;
    QList<QGraphicsItem *> m_selectItems;
    bool m_isRedoExcuteSuccess;
    bool m_isUndoExcuteSuccess;
    QVector<QGraphicsItem *> m_movedItems;
};

/**
 * @brief The CSetBlurAttributeCommand class 模糊图元撤消重做命令
 */
class CSetBlurAttributeCommand: public QUndoCommand
{
public:
    explicit CSetBlurAttributeCommand(CDrawScene *scene, CGraphicsMasicoItem *item, int newType, int newRadio, QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

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
    CSceneCutCommand(CDrawScene *scene, QRectF rect, QUndoCommand *parent = nullptr);
    ~CSceneCutCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QRectF m_newRect;
    QRectF m_oldRect;
};

/**
 * @brief The CSetItemsCommonPropertyValueCommand class 设置多选公共属性
 */
class CSetItemsCommonPropertyValueCommand: public QUndoCommand
{
public:
    explicit CSetItemsCommonPropertyValueCommand(CDrawScene *scene, QList<CGraphicsItem *> items, EDrawProperty property, QVariant value);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CDrawScene *myGraphicsScene;
    QList<CGraphicsItem *> m_items;
    EDrawProperty m_property;
    QVariant m_value;
    QMap<CGraphicsItem *, QVariant> m_oldValues;
};

#endif // CUNDOCOMMANDS_H
