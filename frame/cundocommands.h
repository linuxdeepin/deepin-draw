#ifndef CUNDOCOMMANDS_H
#define CUNDOCOMMANDS_H
#include "drawshape/csizehandlerect.h"
#include <QUndoCommand>
#include <QPointF>
#include <QList>
#include <QBrush>
#include <QPen>
class QGraphicsScene;
class QGraphicsItem;
class CGraphicsItem;
class CGraphicsPolygonItem;
class CGraphicsPolygonalStarItem;

class CMoveShapeCommand : public QUndoCommand
{
public:
    CMoveShapeCommand(QGraphicsScene *graphicsScene, const QPointF &delta,
                      QUndoCommand *parent = nullptr);
    CMoveShapeCommand(QGraphicsItem *item, const QPointF &delta, QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
private:
    QGraphicsScene *myGraphicsScene;
    QGraphicsItem  *myItem;
    QList<QGraphicsItem *> myItems;
    QPointF myDelta;
    bool bMoved;
};

class CResizeShapeCommand : public QUndoCommand
{
public:
    CResizeShapeCommand(CGraphicsItem *item,
                        CSizeHandleRect::EDirection handle,
                        QPointF beginPos,
                        QPointF endPos,
                        bool bShiftPress,
                        bool bAltPress,
                        QUndoCommand *parent = nullptr );
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CGraphicsItem  *myItem;
    CSizeHandleRect::EDirection m_handle;
    QPointF m_beginPos;
    QPointF m_endPos;
    bool m_bShiftPress;
    bool m_bAltPress;
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
class CRotateShapeCommand : public QUndoCommand
{
public:
    CRotateShapeCommand(QGraphicsItem *item, const qreal oldAngle,
                        QUndoCommand *parent = nullptr);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;
private:
    QGraphicsItem *myItem;
    qreal myOldAngle;
    qreal newAngle;
};


class CRemoveShapeCommand : public QUndoCommand
{
public:
    explicit CRemoveShapeCommand(QGraphicsScene *graphicsScene, QUndoCommand *parent = nullptr);
    ~CRemoveShapeCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    QList<QGraphicsItem *> items;
    QGraphicsScene *myGraphicsScene;
};

class CSetPropertyCommand : public QUndoCommand
{
public:
    explicit CSetPropertyCommand(CGraphicsItem *item, QPen pen, QBrush brush, bool bPenChange, bool bBrushChange, QUndoCommand *parent = nullptr);
    ~CSetPropertyCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;


private:
    CGraphicsItem *m_pItem;
    QPen m_oldPen;
    QBrush m_oldBrush;
    QPen m_newPen;
    QBrush m_newBrush;
    bool m_bPenChange;
    bool m_bBrushChange;
};

class CSetPolygonAttributeCommand: public QUndoCommand
{
public:
    explicit CSetPolygonAttributeCommand(CGraphicsPolygonItem *item, int oldNum);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CGraphicsPolygonItem *m_pItem;
    int m_nOldNum;
    int m_nNewNum;
};

class CSetPolygonStarAttributeCommand: public QUndoCommand
{
public:
    explicit CSetPolygonStarAttributeCommand(CGraphicsPolygonalStarItem *item, int oldNum, int oldRadius);
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    CGraphicsPolygonalStarItem *m_pItem;
    int m_nOldNum;
    int m_nNewNum;
    int m_nOldRadius;
    int m_nNewRadius;
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

class CAddShapeCommand : public QUndoCommand
{
public:
    CAddShapeCommand(QGraphicsItem *item, QGraphicsScene *graphicsScene,
                     QUndoCommand *parent = nullptr);
    ~CAddShapeCommand() Q_DECL_OVERRIDE;

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    QGraphicsItem *myDiagramItem;
    QGraphicsScene *myGraphicsScene;
    QPointF initialPosition;
};




#endif // CUNDOCOMMANDS_H
