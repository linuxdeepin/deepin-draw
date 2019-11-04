/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ccuttool.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "qgraphicsview.h"
#include "globaldefine.h"

#include <DApplication>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

DWIDGET_USE_NAMESPACE


CCutTool::CCutTool()
    : IDrawTool (cut)
    , m_pCutItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_buttonType(CButtonRect::NoneButton)

{

}

CCutTool::~CCutTool()
{

}

void CCutTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;
        m_sPointPress = event->scenePos();
        //选中图元
        if (m_pCutItem != nullptr) {
            if (CButtonRect::NoneButton != m_buttonType) {
                qApp->setOverrideCursor(m_pCutItem->getCursor(CSizeHandleRect::None, m_bMousePress));
            } else {
                qApp->setOverrideCursor(m_pCutItem->getCursor(m_dragHandle, m_bMousePress));
            }
//            qApp->setOverrideCursor(m_pCutItem->getCursor(m_dragHandle, m_bMousePress));
        }
        scene->mouseEvent(event);
    } else {
        scene->mouseEvent(event);
    }
}
void CCutTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{

    //按钮碰撞检测
//    if (nullptr != m_pCutItem && !m_bMousePress) {
//        CButtonRect::EButtonType btnType = m_pCutItem->collideTest(event->scenePos());
//        if (btnType != m_buttonType) {
//            m_buttonType = btnType;
//            if (btnType == CButtonRect::OKButton) {
//                qApp->setOverrideCursor(QCursor(m_pCutItem->getCursor(CSizeHandleRect::None, m_bMousePress)));
//            } else if (btnType == CButtonRect::CancelButton) {
//                qApp->setOverrideCursor(QCursor(m_pCutItem->getCursor(CSizeHandleRect::None, m_bMousePress)));
//            } else {
//                if (m_pCutItem->isFreeMode()) {
//                    qApp->setOverrideCursor(QCursor(m_pCutItem->getCursor(CSizeHandleRect::InRect, m_bMousePress)));
//                }
//            }
//        }
//    }

    //碰撞检测
    if (nullptr != m_pCutItem && m_pCutItem->isFreeMode() && !m_bMousePress) {
        CSizeHandleRect::EDirection dragHandle = m_pCutItem->hitTest(event->scenePos());

        if (dragHandle != m_dragHandle ) {
            m_dragHandle = dragHandle;
            qApp->setOverrideCursor(QCursor(m_pCutItem->getCursor(m_dragHandle, m_bMousePress)));
        }
    }

    if ( nullptr != m_pCutItem && m_pCutItem->isFreeMode() && m_bMousePress) {
        if (m_dragHandle != CSizeHandleRect::None  && m_dragHandle != CSizeHandleRect::InRect) {
            m_pCutItem->resizeTo(m_dragHandle, event->scenePos());
            emit static_cast<CDrawScene *>(scene)->signalUpdateCutSize();
        } else {
            scene->mouseEvent(event);
        }
    } else {
        scene->mouseEvent(event);
    }

}
void CCutTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;
        m_sPointRelease = event->scenePos();

        //重绘矩形
        QRectF rect = m_pCutItem->rect();
        QPointF centerPoint = m_pCutItem->mapToScene(m_pCutItem->rect().center());
        rect.setRect(centerPoint.rx() - rect.width() * 0.5, centerPoint.ry() - rect.height() * 0.5, rect.width(), rect.height());
        m_pCutItem->setPos(0, 0);
        m_pCutItem->setRect(rect);
        m_pCutItem->update();

        if (m_pCutItem != nullptr) {
            if (CButtonRect::NoneButton != m_buttonType) {
                qApp->setOverrideCursor(m_pCutItem->getCursor(CSizeHandleRect::None, m_bMousePress));
            } else {
                qApp->setOverrideCursor(m_pCutItem->getCursor(m_dragHandle, m_bMousePress));
            }
        }

//        if (nullptr != m_pCutItem && CButtonRect::NoneButton != m_buttonType) {
//            if (CButtonRect::OKButton == m_buttonType) {
//                //emit scene->signalDoCut(m_pCutItem->rect());
//                scene->setSceneRect(m_pCutItem->rect());
//                scene->update();
//                scene->quitCutMode();
//            } else if (CButtonRect::CancelButton == m_buttonType) {
//                scene->quitCutMode();
//            }
//        }

        scene->mouseEvent(event);
    }

}

void CCutTool::createCutItem(CDrawScene *scene)
{
    if (nullptr != m_pCutItem ) {
        return;
    }

    scene->clearSelection();

    m_pCutItem = new CGraphicsCutItem(scene->sceneRect());
    scene->addItem(m_pCutItem);

    m_dragHandle = CSizeHandleRect::None;

    m_pCutItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_pCutItem->setIsFreeMode(true);
    m_pCutItem->setSelected(true);
}

void CCutTool::deleteCutItem(CDrawScene *scene)
{
    qApp->setOverrideCursor(Qt::ArrowCursor);

    if (nullptr != m_pCutItem) {
        scene->removeItem(m_pCutItem);
        delete m_pCutItem;
        m_pCutItem = nullptr;
    }


}

void CCutTool::changeCutType(int type, CDrawScene *scene)
{
    Q_UNUSED(scene)

    if (nullptr != m_pCutItem) {
        if (cut_free == type) {
            //scene->setItemDisable(false);
            m_pCutItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            m_pCutItem->setIsFreeMode(true);
            m_pCutItem->setSelected(true);
        } else {
            m_pCutItem->setSelected(false);
            m_pCutItem->doChangeType(type);
        }
    }
}

void CCutTool::changeCutSize(const QSize &size)
{
    if (nullptr != m_pCutItem) {
        m_pCutItem->doChangeSize(size.width(), size.height());
    }
}

QRectF CCutTool::getCutRect()
{
    QRectF rect;

    qApp->setOverrideCursor(Qt::ArrowCursor);

    if (nullptr != m_pCutItem) {
        rect = m_pCutItem->rect();
    }

    return rect;
}
