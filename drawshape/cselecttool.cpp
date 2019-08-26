#include "cselecttool.h"
#include "cdrawscene.h"
#include "cgraphicsitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QtMath>
CSelectTool::CSelectTool ()
    : IDrawTool (selection)
    , m_currentSelectItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
{

}

CSelectTool::~CSelectTool()
{

}

void CSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    m_bMousePress = true;
    m_sPointPress = event->scenePos();
    //选中图元

    if (CSizeHandleRect::None == m_dragHandle) {
        //要先触发scene->mouseEvent(event);  才能获取是否有图元被选中
        scene->mouseEvent(event);

        QList<QGraphicsItem *> items = scene->selectedItems();

        if ( items.count() != 0 ) {
            m_currentSelectItem = static_cast<CGraphicsItem *>(items.first());
        }
    }
}

void CSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //碰撞检测
    if (nullptr != m_currentSelectItem && !m_bMousePress) {
        CSizeHandleRect::EDirection dragHandle = m_currentSelectItem->hitTest(event->scenePos());

        if (dragHandle != m_dragHandle) {
            m_dragHandle = dragHandle;
            scene->setCursor(QCursor(m_currentSelectItem->getCursor(m_dragHandle)));
        }
    }

    if ( m_bMousePress ) {
        if (m_dragHandle != CSizeHandleRect::None && m_dragHandle != CSizeHandleRect::Rotation) {
            m_currentSelectItem->resizeTo(m_dragHandle, event->scenePos());
        } else if (m_dragHandle == CSizeHandleRect::Rotation) {
            //旋转图形 有BUG
            QPointF center = m_currentSelectItem->boundingRect().center();
            QPointF pos = m_currentSelectItem->pos();
            QPointF origin = m_currentSelectItem->mapToScene(center);
            qreal scale = m_currentSelectItem->scale();
            m_currentSelectItem->setTransformOriginPoint(origin);
            QPointF mousePoint = event->scenePos();
            qreal len_y = mousePoint.y() - origin.y();
            qreal len_x = mousePoint.x() - origin.x();
            qreal angle = atan2(-len_x, len_y) * 180 / M_PI + 180;
            qDebug() << "angle" << angle << endl;
            if ( angle > 360 ) {
                angle -= 360;
            }

            m_currentSelectItem->setRotation(angle);

        } else {
            scene->mouseEvent(event);
        }
    } else {
        scene->mouseEvent(event);
    }
}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    m_bMousePress = false;
    m_sPointRelease = event->scenePos();
    scene->mouseEvent(event);
}


