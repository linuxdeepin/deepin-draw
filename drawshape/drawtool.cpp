#include "drawtool.h"
#include "maingraphicsscene.h"
#include "rectgraphicsitem.h"
#include "ccursorshapemanager.h"


#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QtMath>

#define PI 3.1416

QList<DrawTool *> DrawTool::c_tools;
QPointF DrawTool::c_down;
QPointF DrawTool::c_last;
quint32 DrawTool::c_nDownFlags;

DrawToolType DrawTool::c_drawShape = selection;

static SelectTool selectTool;
static RectTool   rectTool(rectangle);


enum SelectMode {
    none,
    netSelect,
    move,
    size,
    rotate
};

SelectMode selectMode = none;

int nDragHandle = Handle_None;

static void setCursor(MainGraphicsScene *scene, const QCursor &cursor )
{
    QGraphicsView *view = scene->view();
    if (view)
        view->setCursor(cursor);
}

DrawTool::DrawTool(DrawToolType shape)
{
    m_drawShape = shape ;
    m_hoverSizer = false;
    c_tools.push_back(this);
}

void DrawTool::mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    Q_UNUSED(scene)
    c_down = event->scenePos();
    c_last = event->scenePos();
}

void DrawTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    Q_UNUSED(scene)
    c_last = event->scenePos();
}

void DrawTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    if (event->scenePos() == c_down )
        c_drawShape = selection;
    setCursor(scene, Qt::ArrowCursor);
}

DrawTool *DrawTool::findTool(DrawToolType drawShape)
{
    QList<DrawTool *>::const_iterator iter = c_tools.constBegin();
    for ( ; iter != c_tools.constEnd() ; ++iter ) {
        if ((*iter)->m_drawShape == drawShape )
            return (*iter);
    }
    return nullptr;
}

SelectTool::SelectTool()
    : DrawTool(selection)
{
    dashRect = nullptr;
    opposite_ = QPointF();
}

void SelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    DrawTool::mousePressEvent(event, scene);

    if ( event->button() != Qt::LeftButton ) return;

    if (!m_hoverSizer)
        scene->mouseEvent(event);

    nDragHandle = Handle_None;
    selectMode = none;
    QList<QGraphicsItem *> items = scene->selectedItems();
    AbstractShape *item = nullptr;

    if ( items.count() == 1 )
        item = qgraphicsitem_cast<AbstractShape *>(items.first());

    if ( item != nullptr ) {
        nDragHandle = item->collidesWithHandle(event->scenePos());
        if ( nDragHandle != Handle_None && nDragHandle <= Left ) {
            selectMode = size;
            opposite_ = item->opposite(nDragHandle);
            if ( opposite_.x() == 0 )
                opposite_.setX(1);
            if (opposite_.y() == 0 )
                opposite_.setY(1);
            setCursor(scene, Qt::ClosedHandCursor);
        } else if ( nDragHandle == OutTop ) {
            selectMode = rotate;
            setCursor(scene, CCursorShapeManager::getRotateCursor());
        } else {
            selectMode =  move;
            setCursor(scene, Qt::ClosedHandCursor);
        }
    } else if ( items.count() > 1 )
        selectMode =  move;

    if ( selectMode == none ) {
        selectMode = netSelect;
        if ( scene->view() ) {
            QGraphicsView *view = scene->view();
            view->setDragMode(QGraphicsView::RubberBandDrag);
        }
    }

    if ( (selectMode == move  || selectMode == rotate) && items.count() == 1 ) {

        if (dashRect ) {
            scene->removeItem(dashRect);
            delete dashRect;
            dashRect = nullptr;
        }

        dashRect = new QGraphicsPathItem(item->shape());
        dashRect->setPen(Qt::DashLine);
        dashRect->setPos(item->pos());
        dashRect->setTransformOriginPoint(item->transformOriginPoint());
        dashRect->setTransform(item->transform());
        dashRect->setRotation(item->rotation());
        dashRect->setScale(item->scale());
        dashRect->setZValue(item->zValue());
        scene->addItem(dashRect);
        if (selectMode == move)
            initialPositions = item->pos();
        else if (selectMode == rotate) {
            QPointF origin = item->mapToScene(item->boundingRect().center());
            qreal len_y = c_last.y() - origin.y();
            qreal len_x = c_last.x() - origin.x();
            qreal angle = atan2(len_y, len_x) * 180 / PI;
            lastAngle = angle;
        }
    }
}
void SelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    DrawTool::mouseMoveEvent(event, scene);
    QList<QGraphicsItem *> items = scene->selectedItems();
    AbstractShape *item = nullptr;
    if ( items.count() == 1 ) {
        item = qgraphicsitem_cast<AbstractShape *>(items.first());
        if ( item != nullptr ) {
            if ( nDragHandle != Handle_None && selectMode == size ) {
                if (opposite_.isNull()) {
                    opposite_ = item->opposite(nDragHandle);
                    if ( opposite_.x() == 0 )
                        opposite_.setX(1);
                    if (opposite_.y() == 0 )
                        opposite_.setY(1);
                }

                QPointF new_delta = item->mapFromScene(c_last) - opposite_;
                QPointF initial_delta = item->mapFromScene(c_down) - opposite_;

                double sx = new_delta.x() / initial_delta.x();
                double sy = new_delta.y() / initial_delta.y();

                item->stretch(nDragHandle, sx, sy, opposite_);

                emit scene->signalItemResize(item, nDragHandle, QPointF(sx, sy));

//                scene->update();
                //  qDebug()<<"scale:"<<nDragHandle<< item->mapToScene(opposite_)<< sx << " ，" << sy
                //         << new_delta << item->mapFromScene(c_last)
                //         << initial_delta << item->mapFromScene(c_down) << item->boundingRect();

            } else if ( nDragHandle == OutTop  && selectMode == rotate ) {
                QPointF origin = item->mapToScene(item->boundingRect().center());

                qreal len_y = c_last.y() - origin.y();
                qreal len_x = c_last.x() - origin.x();
                qreal angle = atan2(len_y, len_x) * 180 / PI;

                angle = item->rotation() + int(angle - lastAngle) ;

                if ( angle > 360 )
                    angle -= 360;
                if ( angle < -360 )
                    angle += 360;

                if ( dashRect ) {
                    dashRect->setRotation( angle );
                }
//                setCursor(scene, QCursor((QPixmap(":/icons/rotate.png"))));
            } else if (nDragHandle == Handle_None ) {
                int handle = item->collidesWithHandle(event->scenePos());
                if ( handle != Handle_None) {
                    if (handle == OutTop)
                        setCursor(scene, CCursorShapeManager::getRotateCursor());
                    else
                        setCursor(scene, Qt::OpenHandCursor);
                    m_hoverSizer = true;
                } else {
                    setCursor(scene, Qt::ArrowCursor);
                    m_hoverSizer = false;
                }
            }
        }
    }

    if ( selectMode == move ) {
        setCursor(scene, Qt::ClosedHandCursor);
        if ( dashRect ) {
            dashRect->setPos(initialPositions + c_last - c_down);
        }
    }

    if ( selectMode != size  && items.count() > 1) {
        scene->mouseEvent(event);
    }

}

void SelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{

    DrawTool::mouseReleaseEvent(event, scene);

    if ( event->button() != Qt::LeftButton ) return;

    QList<QGraphicsItem *> items = scene->selectedItems();
    if ( items.count() == 1 ) {
        AbstractShape *item = qgraphicsitem_cast<AbstractShape *>(items.first());
        if ( item != nullptr  && selectMode == move && c_last != c_down ) {
            item->setPos(initialPositions + c_last - c_down);
//             scene->update();
            emit scene->signalItemMoved(item, c_last - c_down );
        } else if ( item != nullptr && (selectMode == size /* || selectMode == editor*/) && c_last != c_down ) {
            item->updateCoordinate();
        } else if (item != nullptr  && nDragHandle == OutTop && selectMode == rotate) {
            QPointF origin = item->mapToScene(item->boundingRect().center());
            qreal len_y = c_last.y() - origin.y();
            qreal len_x = c_last.x() - origin.x();
            qreal angle = atan2(len_y, len_x) * 180 / PI, oldAngle = item->rotation();
            angle = item->rotation() + int(angle - lastAngle) ;

            if ( angle > 360 )
                angle -= 360;
            if ( angle < -360 )
                angle += 360;

            item->setRotation( angle );

            lastAngle = 0;

            emit scene->signalItemRotate(item, oldAngle);

        }
    } else if ( items.count() > 1 && selectMode == move && c_last != c_down ) {
        emit scene->signalItemMoved(NULL, c_last - c_down );
    }

    if (selectMode == netSelect ) {


        if ( scene->view() ) {
            QGraphicsView *view = scene->view();
            view->setDragMode(QGraphicsView::NoDrag);
        }
#if 0
        if ( scene->selectedItems().count() > 1 ) {
            selLayer = scene->createGroup(scene->selectedItems());
            selLayer->setSelected(true);
        }
#endif
    }
    if (dashRect ) {
        scene->removeItem(dashRect);
        delete dashRect;
        dashRect = nullptr;
    }
    selectMode = none;
    nDragHandle = Handle_None;
    m_hoverSizer = false;
    opposite_ = QPointF();
    scene->mouseEvent(event);

}

RectTool::RectTool(DrawToolType drawShape)
    : DrawTool(drawShape)
{
    item = nullptr;
}

void RectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{

    if ( event->button() != Qt::LeftButton ) return;

    scene->clearSelection();
    DrawTool::mousePressEvent(event, scene);
    switch ( c_drawShape ) {
    case rectangle:
        item = new RectGraphicsItem(QRect(1, 1, 1, 1));
        QPen pen;
        pen.setWidth(8);
        pen.setColor(Qt::red);
        item->setPen(pen);

        break;
    }
    if ( item == 0) return;
    c_down += QPoint(2, 2);
    item->setPos(event->scenePos());
    scene->addItem(item);
    item->setSelected(true);

    selectMode = size;
    nDragHandle = RightBottom;

}

void RectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    setCursor(scene, Qt::CrossCursor);

    selectTool.mouseMoveEvent(event, scene);
}

void RectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, MainGraphicsScene *scene)
{
    selectTool.mouseReleaseEvent(event, scene);

    if ( event->scenePos() == (c_down - QPoint(2, 2))) {

        if ( item != nullptr) {
            item->setSelected(false);
            scene->removeItem(item);
            delete item ;
            item = nullptr;
        }
        qDebug() << "RectTool removeItem:";
    } else if ( item ) {
        emit scene->signalItemAdded( item );
    }
    c_drawShape = selection;

}

