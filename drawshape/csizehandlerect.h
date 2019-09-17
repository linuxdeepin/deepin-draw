#ifndef CSIZEHANDLERECT
#define CSIZEHANDLERECT

#include <QGraphicsRectItem>
#include <QList>
#include <DSvgRenderer>
#include <QGraphicsSvgItem>

QT_BEGIN_NAMESPACE
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

DWIDGET_USE_NAMESPACE

enum { SELECTION_HANDLE_SIZE = 15, SELECTION_MARGIN = 10 };
enum ESelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };

class CSizeHandleRect : public QGraphicsSvgItem
{
public:
    enum EDirection { LeftTop, Top, RightTop, Right, RightBottom, Bottom, LeftBottom, Left, Rotation, InRect, None};

    CSizeHandleRect(QGraphicsItem *parent, EDirection d);
    CSizeHandleRect(QGraphicsItem *parent, EDirection d, const QString &filename);

    EDirection dir() const
    {
        return m_dir;
    }
    void updateCursor();
    void setState(ESelectionHandleState st);
    bool hitTest( const QPointF &point );
    void move(qreal x, qreal y );
    QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    const EDirection m_dir;
    ESelectionHandleState m_state;
};


#endif // CSIZEHANDLERECT

