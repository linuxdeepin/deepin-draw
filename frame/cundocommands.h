#ifndef CUNDOCOMMANDS_H
#define CUNDOCOMMANDS_H
#include <QUndoCommand>
#include <QPointF>
#include <QList>
class QGraphicsScene;
class QGraphicsItem;

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

/*class ResizeShapeCommand : public QUndoCommand
{
public:
    enum { Id = 1234, };
    ResizeShapeCommand(QGraphicsItem *item,
                       int handle,
                       const QPointF &scale,
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
    int opposite_;
    QPointF scale_;
    bool bResized;
};*/

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
/*

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
