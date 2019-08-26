#ifndef CDRAWSCENE_H
#define CDRAWSCENE_H

#include <QGraphicsScene>
#include "cselecttool.h"
class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CDrawScene : public QGraphicsScene
{
    friend CSelectTool;
public:
    explicit CDrawScene(QObject *parent = nullptr);
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent );
    /**
     * @brief setCursor 设置鼠标指针形状
     * @param cursor
     */
    void setCursor(const QCursor &cursor);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

};

#endif // CDRAWSCENE_H
