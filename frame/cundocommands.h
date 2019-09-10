#ifndef CUNDOCOMMANDS_H
#define CUNDOCOMMANDS_H
#include "drawshape/csizehandlerect.h"
#include <QUndoCommand>
#include <QPointF>
#include <QList>
class QGraphicsScene;
class QGraphicsItem;
class CGraphicsItem;

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


class RemoveShapeCommand : public QUndoCommand
{
public:
    explicit RemoveShapeCommand(QGraphicsScene *graphicsScene, QUndoCommand *parent = nullptr);
    ~RemoveShapeCommand();
    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    QList<QGraphicsItem *> items;
    QGraphicsScene *myGraphicsScene;
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
