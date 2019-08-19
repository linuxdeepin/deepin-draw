#ifndef GRAPHICSBASEITEM_H
#define GRAPHICSBASEITEM_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "sizehandle.h"


static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (path == QPainterPath() || pen == Qt::NoPen)
        return path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;
}

static void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter,
                                              const QStyleOptionGraphicsItem *option)
{

    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
        return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
        return;

    qreal itemPenWidth;
    switch (item->type()) {
    case QGraphicsEllipseItem::Type:
        itemPenWidth = static_cast<QGraphicsEllipseItem *>(item)->pen().widthF();
        break;
    case QGraphicsPathItem::Type:
        itemPenWidth = static_cast<QGraphicsPathItem *>(item)->pen().widthF();
        break;
    case QGraphicsPolygonItem::Type:
        itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
        break;
    case QGraphicsRectItem::Type:
        itemPenWidth = static_cast<QGraphicsRectItem *>(item)->pen().widthF();
        break;
    case QGraphicsSimpleTextItem::Type:
        itemPenWidth = static_cast<QGraphicsSimpleTextItem *>(item)->pen().widthF();
        break;
    case QGraphicsLineItem::Type:
        itemPenWidth = static_cast<QGraphicsLineItem *>(item)->pen().widthF();
        break;
    default:
        itemPenWidth = 1.0;
    }
    const qreal pad = itemPenWidth / 2;


    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);


    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(-pad, -pad, pad, pad));

    painter->setPen(QPen(QColor("lightskyblue"), 0, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(-pad, -pad, pad, pad));

}


template < typename BaseType = QGraphicsItem >

class AbstractShapeType : public BaseType
{
public:
    explicit AbstractShapeType(QGraphicsItem *parent = nullptr )
        : BaseType(parent)
    {
        m_pen = QPen(Qt::SolidLine);
        m_brush = QBrush(QColor(rand() % 32 * 8, rand() % 32 * 8, rand() % 32 * 8));
        m_width = m_height = 0;
    }
    virtual ~AbstractShapeType() {}
    virtual void stretch( int, double, double, const QPointF & ) {}
    virtual QRectF  rect() const
    {
        return m_localRect;
    }
    virtual void updateCoordinate () {}
    virtual void move( const QPointF &point )
    {
        Q_UNUSED(point);
    }
    virtual QGraphicsItem *duplicate() const
    {
        return nullptr;
    }
    virtual int handleCount() const
    {
        return m_handles.size();
    }

    int collidesWithHandle( const QPointF &point ) const
    {
        const Handles::const_reverse_iterator hend =  m_handles.rend();
        for (Handles::const_reverse_iterator it = m_handles.rbegin(); it != hend; ++it) {
            QPointF pt = (*it)->mapFromScene(point);
            if ((*it)->contains(pt) ) {

                return (*it)->dir();
            }
        }

        QPointF pt = m_rotateHandle->mapFromScene(point);
        if (m_rotateHandle->contains(pt) ) {

            return m_rotateHandle->dir();
        }

        return Handle_None;
    }
    virtual QPointF handlePos( int handle ) const
    {
        const Handles::const_reverse_iterator hend =  m_handles.rend();
        for (Handles::const_reverse_iterator it = m_handles.rbegin(); it != hend; ++it) {
            if ((*it)->dir() == handle ) {
                return (*it)->pos();
            }
        }
        return QPointF();
    }
    int swapHandle(int handle, const QPointF &scale ) const
    {
        int dir = Handle_None;
        if ( scale.x() < 0 || scale.y() < 0 ) {
            switch (handle) {
            case RightTop:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = LeftBottom;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = RightBottom;
                else
                    dir = LeftTop;
                break;
            case RightBottom:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = LeftTop;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = RightTop;
                else
                    dir = LeftBottom;
                break;
            case LeftBottom:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = RightTop;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = LeftTop;
                else
                    dir = RightBottom;
                break;
            case LeftTop:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = RightBottom;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = LeftBottom;
                else
                    dir = RightTop;
                break;
            case Right:
                if (scale.x() < 0 )
                    dir = Left;
                break;
            case Left:
                if (scale.x() < 0 )
                    dir = Right;
                break;
            case Top:
                if (scale.y() < 0)
                    dir = Bottom;
                break;
            case Bottom:
                if (scale.y() < 0)
                    dir = Top;
                break;
            }
        }
        return dir;
    }
    virtual QPointF opposite( int handle )
    {
        QPointF pt;
        switch (handle) {
        case Right:
            pt = m_handles.at(Left - 1)->pos();
            break;
        case RightTop:
            pt = m_handles[LeftBottom - 1]->pos();
            break;
        case RightBottom:

            pt = m_handles[LeftTop - 1]->pos();
            break;
        case LeftBottom:
            pt = m_handles[RightTop - 1]->pos();
            break;
        case Bottom:
            pt = m_handles[Top - 1]->pos();
            break;
        case LeftTop:
            pt = m_handles[RightBottom - 1]->pos();
            break;
        case Left:
            pt = m_handles[Right - 1]->pos();
            break;
        case Top:
            pt = m_handles[Bottom - 1]->pos();
            break;
        }
        return pt;
    }

    QColor brushColor() const
    {
        return m_brush.color();
    }
    QBrush brush() const
    {
        return m_brush;
    }
    QPen   pen() const
    {
        return m_pen;
    }
    QColor penColor() const
    {
        return m_pen.color();
    }
    void   setPen(const QPen &pen )
    {
        m_pen = pen;
    }
    void   setBrush( const QBrush &brush )
    {
        m_brush = brush ;
    }
    void   setBrushColor( const QColor &color )
    {
        m_brush.setColor(color);
    }
    qreal  width() const
    {
        return m_width ;
    }
    void   setWidth( qreal width )
    {
        m_width = width ;
        updateCoordinate();
    }
    qreal  height() const
    {
        return m_height;
    }
    void   setHeight ( qreal height )
    {
        m_height = height ;
        updateCoordinate();
    }

protected:
    virtual void updatehandles() {}
    void setState(SelectionHandleState st)
    {
        m_rotateHandle->setState(st);
        const Handles::iterator hend =  m_handles.end();
        for (Handles::iterator it = m_handles.begin(); it != hend; ++it)
            (*it)->setState(st);
    }
    QBrush m_brush;
    QPen   m_pen ;
    typedef std::vector<SizeHandleRect *> Handles;
    Handles m_handles;
    RotateHandle *m_rotateHandle;
    QRectF m_localRect;
    qreal m_width;
    qreal m_height;
};

typedef  AbstractShapeType< QGraphicsItem > AbstractShape;

class BaseGraphicsItem : public QObject, public AbstractShapeType<QGraphicsItem>
{
    Q_OBJECT
    Q_PROPERTY(QColor pen READ penColor WRITE setPen )
    Q_PROPERTY(QColor brush READ brushColor WRITE setBrushColor )
    Q_PROPERTY(qreal  width READ width WRITE setWidth )
    Q_PROPERTY(qreal  height READ height WRITE setHeight )
    Q_PROPERTY(QPointF  position READ pos WRITE setPos )

public:
    BaseGraphicsItem(QGraphicsItem *parent );
    enum {Type = UserType + 1};
    int  type() const
    {
        return Type;
    }
    virtual QPixmap image() ;
signals:
    void selectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    void updatehandles();
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
};

#endif // GRAPHICSBASEITEM_H
