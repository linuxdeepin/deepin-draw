#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QGraphicsScene>

class MainGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
 public:
     explicit MainGraphicsScene(QObject *parent = nullptr);
     void setView(QGraphicsView * view );
     QGraphicsView * view();
     void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent );

 signals:
     void signalItemMoved( QGraphicsItem * item , const QPointF & oldPosition );
     void signalItemRotate(QGraphicsItem * item , const qreal oldAngle );
     void signalItemAdded(QGraphicsItem * item );
     void signalItemResize(QGraphicsItem * item , int handle , const QPointF& scale );
     void signalItemControl(QGraphicsItem * item , int handle , const QPointF & newPos , const QPointF& lastPos_ );

 protected:
     void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
     void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

     QGraphicsView * m_graphicsView;

     qreal m_dx;
     qreal m_dy;
     bool  m_moved;

};

#endif // MAINSCENE_H
