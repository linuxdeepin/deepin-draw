#ifndef CDRAWSCENE_H
#define CDRAWSCENE_H

#include <QGraphicsScene>
#include "cselecttool.h"
class CDrawScene : public QGraphicsScene
{
    friend CSelectTool;
public:
    explicit CDrawScene(QObject *parent = nullptr);
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent );
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

private:
    bool    m_bMousePress;
    QPointF m_pressPoint;
    QGraphicsRectItem    *m_pSelectedItem;

    bool m_bFirst;
};

#endif // CDRAWSCENE_H
