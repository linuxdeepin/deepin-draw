#ifndef CDRAWSCENE_H
#define CDRAWSCENE_H
#include "cselecttool.h"
#include <QGraphicsScene>
#include <QObject>

class QGraphicsSceneMouseEvent;
class QKeyEvent;
class CDrawScene : public QGraphicsScene
{
    Q_OBJECT
    friend CSelectTool;
public:
    explicit CDrawScene(QObject *parent = nullptr);

    /**
     * @brief keyEvent 从绘图工具返回键盘事件
     * @param keyEvent
     */
    void keyEvent(QKeyEvent *keyEvent);
    /**
     * @brief setCursor 设置鼠标指针形状
     * @param cursor
     */
    void setCursor(const QCursor &cursor);

    /**
     * @brief attributeChanged 画笔画刷属性更改
     */
    void attributeChanged();

    /**
     * @brief changeAttribute 修改当前画笔画刷属性
     */
    void changeAttribute(bool flag, QGraphicsItem *selectedItem);

    /**
     * @brief mouseEvent 鼠标事件
     * @param mouseEvent
     */
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /**
     * @brief drawBackground
     * @param painter
     * @param rect
     */
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;

    /**
     * @brief sendAddSignal
     * @param item
     */
    void sendAddSignal(QGraphicsItem *item);
signals:
    /**
     * @brief signalAttributeChanged 发送属性栏更改的信号
     * @param flag
     * @param primitiveType 图元类型
     */
    void signalAttributeChanged(bool flag, int primitiveType);
    /**
     * @brief signalChangeToSelect 发送工具栏切换为选择的信号
     */
    void signalChangeToSelect();

    /**
     * @brief itemMoved 移动
     * @param item
     * @param pos
     */
    void itemMoved(QGraphicsItem *item, QPointF pos);

    void itemAdded(QGraphicsItem *item);

    void itemRotate(QGraphicsItem *item, const qreal oldAngle );

    void itemResize(QGraphicsItem *item, int handle, const QPointF &scale );

public slots:
    void picOperation(int enumstyle);





protected:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

};

#endif // CDRAWSCENE_H
