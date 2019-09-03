#ifndef CSIZEHANDLERECT
#define CSIZEHANDLERECT

#include <QGraphicsRectItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE


enum { SELECTION_HANDLE_SIZE = 6, SELECTION_MARGIN = 10 };
enum ESelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };

class CSizeHandleRect : public QGraphicsRectItem
{
public:
    enum EDirection { LeftTop, Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left, Rotation, None};

    CSizeHandleRect(QGraphicsItem *parent, EDirection d, QGraphicsItem *resizable);
    CSizeHandleRect(QGraphicsItem *parent, EDirection d, QGraphicsItem *resizable, const QPixmap &pixMap);

    EDirection dir() const
    {
        return m_dir;
    }
    void updateCursor();
    void setState(ESelectionHandleState st);
    bool hitTest( const QPointF &point );
    void move(qreal x, qreal y );

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const EDirection m_dir;
    QPoint m_startPos;
    QPoint m_curPos;
    QSize m_startSize;
    QSize m_curSize;
    QGraphicsItem *m_resizable;
    ESelectionHandleState m_state;
    QPixmap m_rotaImage;
};


#endif // CSIZEHANDLERECT

