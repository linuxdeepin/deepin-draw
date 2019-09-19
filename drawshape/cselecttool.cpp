#include "cselecttool.h"
#include "cdrawscene.h"
#include "cgraphicsitem.h"
#include "cgraphicslineitem.h"
#include "cdrawparamsigleton.h"
#include "cgraphicsrotateangleitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"

#include <DApplication>

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QtMath>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QPixmap>
#include <QGraphicsView>
#include <QTextEdit>

CSelectTool::CSelectTool ()
    : IDrawTool (selection)
    , m_currentSelectItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bRotateAng(false)
    , m_rotateAng(0)
    , m_rotateCursor(QPixmap(":/theme/resources/rotate_mouse.svg"))
    , m_initRotateItemPos(0, 0)
    , m_RotateItem(nullptr)
    , m_textEditCursor(QPixmap(":/theme/resources/text_mouse.svg"))
{

}

CSelectTool::~CSelectTool()
{

}

void CSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    qDebug() << "mouse press" << endl;
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;
        m_sPointPress = event->scenePos();
        //选中图元
        if (m_currentSelectItem != nullptr) {
            if (CSizeHandleRect::InRect == m_dragHandle && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                qApp->setOverrideCursor(m_textEditCursor);
            } else {
                qApp->setOverrideCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress));
            }
        }

        if (CSizeHandleRect::None == m_dragHandle || CSizeHandleRect::InRect == m_dragHandle) {
            //要先触发scene->mouseEvent(event);  才能获取是否有图元被选中
            scene->mouseEvent(event);

            QList<QGraphicsItem *> items = scene->selectedItems();

            if ( items.count() != 0 ) {
                QGraphicsItem *item = items.first();
                //需要区别图元或文字
                m_currentSelectItem = static_cast<CGraphicsItem *>(item);
                m_dragHandle = CSizeHandleRect::InRect;
                if (CSizeHandleRect::InRect == m_dragHandle && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                    qApp->setOverrideCursor(m_textEditCursor);
                } else {
                    qApp->setOverrideCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress));
                }
                //qApp->setOverrideCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress));
                scene->changeAttribute(true, item);

                //scene->changeAttribute(true, m_currentSelectItem->pen(), m_currentSelectItem->brush());
            } else {
                m_currentSelectItem = nullptr;
            }
        }
    } else {
        scene->mouseEvent(event);
    }
}

void CSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    // 再判断一次
    QList<QGraphicsItem *> items = scene->selectedItems();
    //if (m_currentSelectItem == nullptr) {

    if ( items.count() != 0 ) {
        QGraphicsItem *item = items.first();
        CGraphicsItem *currentItem = static_cast<CGraphicsItem *>(item);
        if (currentItem != m_currentSelectItem) {
            m_currentSelectItem = currentItem;
            m_rotateAng = m_currentSelectItem->rotation();
            scene->changeAttribute(true, item);
        }
    } else {
        m_dragHandle = CSizeHandleRect::None;
        qApp->setOverrideCursor(Qt::ArrowCursor);
        m_currentSelectItem = nullptr;
    }
    // }
    //碰撞检测
    if (items.count() != 0 && nullptr != m_currentSelectItem && !m_bMousePress) {
        CSizeHandleRect::EDirection dragHandle = m_currentSelectItem->hitTest(event->scenePos());

        if (dragHandle != m_dragHandle) {
            m_dragHandle = dragHandle;
            if (m_dragHandle == CSizeHandleRect::InRect && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                qApp->setOverrideCursor(m_textEditCursor);
            } else {
                qApp->setOverrideCursor(QCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress)));
            }
            //scene->setCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress));
            m_rotateAng = m_currentSelectItem->rotation();
        }
    }

    if ( m_bMousePress ) {
        if (m_dragHandle != CSizeHandleRect::None && m_dragHandle != CSizeHandleRect::Rotation && m_dragHandle != CSizeHandleRect::InRect) {
            m_currentSelectItem->resizeTo(m_dragHandle, event->scenePos());
        } else if (m_dragHandle == CSizeHandleRect::Rotation) {
            //旋转图形 有BUG
            m_bRotateAng = true;
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

            //显示旋转角度
            if (m_RotateItem == nullptr) {
                m_RotateItem = new CGraphicsRotateAngleItem(angle);
                scene->addItem(m_RotateItem);
                m_initRotateItemPos.setX(centerToScence.x());
                m_initRotateItemPos.setY(centerToScence.y() - m_currentSelectItem->rect().height() / 2 - 65);
                m_RotateItem->setPos(m_initRotateItemPos);
            } else {
                qreal angleRad = qDegreesToRadians(angle);

                qreal x0 = (m_initRotateItemPos.x() - centerToScence.x()) * qCos(angleRad) - (m_initRotateItemPos.y() - centerToScence.y()) * qSin(angleRad) + centerToScence.x() ;
                qreal y0 = (m_initRotateItemPos.x() - centerToScence.x()) * qSin(angleRad) + (m_initRotateItemPos.y() - centerToScence.y()) * qCos(angleRad) + centerToScence.y();

                m_RotateItem->updateRotateAngle(angle);
                m_RotateItem->setPos(x0, y0);
            }

        } else {
            scene->mouseEvent(event);
        }
    } else {
        scene->mouseEvent(event);
    }
}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;
        m_sPointRelease = event->scenePos();

        //选中图元
        if (m_currentSelectItem != nullptr) {
            if (CSizeHandleRect::InRect == m_dragHandle && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                qApp->setOverrideCursor(m_textEditCursor);
            } else {
                qApp->setOverrideCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress));
            }
        }

        if (m_currentSelectItem != nullptr) {
            if (m_dragHandle == CSizeHandleRect::Rotation) {
                if (m_RotateItem) {
                    delete m_RotateItem;
                    m_RotateItem = nullptr;
                }
                emit scene->itemRotate(m_currentSelectItem, m_rotateAng);
            } else if (m_dragHandle == CSizeHandleRect::InRect) {
                QList<QGraphicsItem *> items = scene->selectedItems();
                if (items.count() == 1) {
                    emit scene->itemMoved(m_currentSelectItem, m_sPointRelease - m_sPointPress );
                } else {
                    emit scene->itemMoved(nullptr, m_sPointRelease - m_sPointPress );
                }

            } else {
                bool shiftKeyPress = CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
                bool altKeyPress = CDrawParamSigleton::GetInstance()->getAltKeyStatus();
                emit scene->itemResize(m_currentSelectItem, m_dragHandle, m_sPointPress, m_sPointRelease, shiftKeyPress, altKeyPress);
            }
        }
    }
    scene->mouseEvent(event);
}

void CSelectTool::selectionChange()
{
    if (m_currentSelectItem != nullptr) {
        if (m_currentSelectItem->type() == TextType) {
            static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getCGraphicsProxyWidget()->hide();
        }
    }
}


