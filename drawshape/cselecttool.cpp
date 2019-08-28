#include "cselecttool.h"
#include "cdrawscene.h"
#include "cgraphicsitem.h"
#include "cgraphicslineitem.h"
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
            QGraphicsItem *item = items.first();
            //需要区别图元或文字
            if (item->type() > QGraphicsItem::UserType) {
                m_currentSelectItem = static_cast<CGraphicsItem *>(item);
                scene->changeAttribute(true, m_currentSelectItem->pen(), m_currentSelectItem->brush());
            } else {
                m_currentSelectItem = nullptr;
            }
            //scene->changeAttribute(true, m_currentSelectItem->pen(), m_currentSelectItem->brush());
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
            QPointF center = m_currentSelectItem->rect().center();
            m_currentSelectItem->setTransformOriginPoint(center);
            QPointF mousePoint = event->scenePos();
            QPointF centerToScence = m_currentSelectItem->mapToScene(center);
            qreal len_y = mousePoint.y() - centerToScence.y();
            qreal len_x = mousePoint.x() - centerToScence.x();
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


