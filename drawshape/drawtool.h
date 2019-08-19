#ifndef DRAWTOOL
#define DRAWTOOL


#include "basegraphicsitem.h"


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QAbstractGraphicsShapeItem;
QT_END_NAMESPACE

class MainGraphicsScene;

class GraphicsItem;
class GraphicsPolygonItem;

enum DrawToolType {
    selection,
    rectangle,
    ellipse,
    triangle,
    star,
    polygon,
    line,
    pen,
    text,
    rotation
};

class DrawTool
{
public:
    DrawTool( DrawToolType shape );
//    virtual ~DrawTool(){}
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene );


    DrawToolType m_drawShape;
    bool m_hoverSizer;

    static DrawTool *findTool( DrawToolType drawShape );
    static QList<DrawTool *> c_tools;
    static QPointF c_down;
    static quint32 c_nDownFlags;
    static QPointF c_last;
    static DrawToolType c_drawShape;
};

class SelectTool : public DrawTool
{
public:
    SelectTool();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene );
    QPointF initialPositions;
    qreal lastAngle;
    QPointF opposite_;
    QGraphicsPathItem *dashRect;

};

class  RotationTool : public DrawTool
{
public:
    RotationTool();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene );
    qreal lastAngle;
    QGraphicsPathItem *dashRect;
};

class RectTool : public DrawTool
{
public:
    RectTool(DrawToolType drawShape);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene ) ;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene );
    BaseGraphicsItem *item;
};



#endif // DRAWTOOL

