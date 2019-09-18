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
#include "cselecttool.h"
#include "cdrawscene.h"
#include "cgraphicsitem.h"
#include "cgraphicslineitem.h"
#include "cdrawparamsigleton.h"
#include "cgraphicsrotateangleitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cgraphicscutitem.h"
#include "widgets/ctextedit.h"
#include "cgraphicsmasicoitem.h"

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
#include <QSvgGenerator>
#include <QtMath>


CSelectTool::CSelectTool ()
    : IDrawTool (selection)
    , m_currentSelectItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bRotateAng(false)
    , m_rotateAng(0)
    , m_rotateCursor(QPixmap(":/theme/light/images/mouse_style/rotate_mouse.svg"))
    , m_initRotateItemPos(0, 0)
    , m_RotateItem(nullptr)
    , m_textEditCursor(QPixmap(":/theme/light/images/mouse_style/text_mouse.svg"))
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
        m_sLastPress = m_sPointPress;
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

                //如果是滤镜图层
                if (m_currentSelectItem->type() == BlurType) {
                    static_cast<CGraphicsMasicoItem *>(m_currentSelectItem)->setPixmap();
                } else if (m_currentSelectItem->type() == TextType) {
                    static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->cursorPositionChanged();
                }
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

//    if (m_currentSelectItem == nullptr) {
//        m_dragHandle = CSizeHandleRect::None;
//        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
//    }

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
        //scene->setCursor(Qt::ArrowCursor);
        m_currentSelectItem = nullptr;
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
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

            //旋转图形
            qreal angle = 0;
            m_bRotateAng = true;
            QPointF center = m_currentSelectItem->rect().center();
            m_currentSelectItem->setTransformOriginPoint(center);
            QPointF mousePoint = event->scenePos();
            QPointF centerToScence = m_currentSelectItem->mapToScene(center);
            qreal len_y = mousePoint.y() - centerToScence.y();
            qreal len_x = mousePoint.x() - centerToScence.x();
            angle = atan2(-len_x, len_y) * 180 / M_PI + 180;
            qDebug() << "angle" << angle << endl;
            if ( angle > 360 ) {
                angle -= 360;
            }
            if (m_currentSelectItem->type() != LineType) {
                m_currentSelectItem->setRotation(angle);
            } else {
                QLineF line = static_cast<CGraphicsLineItem *>(m_currentSelectItem)->line();
                QPointF vector = line.p2() - line.p1();
                qreal oriangle = 0;
                if (vector.x() - 0 < 0.0001 && vector.x() - 0 > -0.0001) {
                    if (line.p2().y() - line.p1().y() > 0.0001) {
                        oriangle = 90;
                    } else {
                        oriangle = -90;
                    }
                } else {
                    oriangle = (-atan(vector.y() / vector.x())) * 180 / 3.14159 + 180;
                }
                angle = angle - oriangle;
                if ( angle > 360 ) {
                    angle -= 360;
                }
                m_currentSelectItem->setRotation(angle);
            }

            //显示旋转角度
            if (m_RotateItem == nullptr) {
                m_RotateItem = new CGraphicsRotateAngleItem(angle);
                scene->addItem(m_RotateItem);
                m_initRotateItemPos.setX(centerToScence.x());

                qreal scale = CDrawParamSigleton::GetInstance()->getScale();
                qreal space = 65.;
                if (scale > 1.) {
                    qreal temp = 10 * (scale - 1.);
                    space += temp;
                } else if (scale < 1. && scale >= 0.5) {
                    qreal temp  = 80 * (1. - scale);
                    space += temp;
                } else if (scale < 0.5) {
                    qreal temp  = 130 * (1. - scale);
                    space += temp;
                }

                m_initRotateItemPos.setY(centerToScence.y() - m_currentSelectItem->rect().height() / 2 - space);
                m_RotateItem->setPos(m_initRotateItemPos);
            } else {
                qreal angleRad = qDegreesToRadians(angle);

                qreal x0 = (m_initRotateItemPos.x() - centerToScence.x()) * qCos(angleRad) - (m_initRotateItemPos.y() - centerToScence.y()) * qSin(angleRad) + centerToScence.x() ;
                qreal y0 = (m_initRotateItemPos.x() - centerToScence.x()) * qSin(angleRad) + (m_initRotateItemPos.y() - centerToScence.y()) * qCos(angleRad) + centerToScence.y();

                m_RotateItem->updateRotateAngle(angle);
                m_RotateItem->setPos(x0, y0);
            }

        } else {
            if (m_currentSelectItem != nullptr ) {
                if (m_currentSelectItem->type() != TextType) {
                    m_currentSelectItem->move(m_sLastPress, event->scenePos());
                } else {
                    //文字图元非编辑状态下是移动图元 编辑状态下是选中文字
                    if (!static_cast<CGraphicsTextItem *>(m_currentSelectItem)->isEditable()) {
                        m_currentSelectItem->move(m_sLastPress, event->scenePos());
                    } else {
                        scene->mouseEvent(event);
                    }
                }
            } else {
                scene->mouseEvent(event);
            }
            //scene->mouseEvent(event);
        }
    } else {
        scene->mouseEvent(event);
    }

    m_sLastPress = event->scenePos();

}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;
        m_sPointRelease = event->scenePos();

        QPointF vectorPoint = m_sPointRelease - m_sPointPress;

        //选中图元
        if (m_currentSelectItem != nullptr) {
            if (CSizeHandleRect::InRect == m_dragHandle && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                qApp->setOverrideCursor(m_textEditCursor);
            } else {
                qApp->setOverrideCursor(m_currentSelectItem->getCursor(m_dragHandle, m_bMousePress));
            }

            //最后需要刷新一次
            if (m_currentSelectItem->type() == BlurType) {
                static_cast<CGraphicsMasicoItem *>(m_currentSelectItem)->setPixmap();
            }
        }

        if (m_currentSelectItem != nullptr) {
            if (m_dragHandle == CSizeHandleRect::Rotation) {
                if (m_RotateItem) {
                    delete m_RotateItem;
                    m_RotateItem = nullptr;
                }
                if (qAbs(vectorPoint.x()) > 0.0001 && qAbs(vectorPoint.y()) > 0.001) {
                    emit scene->itemRotate(m_currentSelectItem, m_rotateAng);
                }
            } else if (m_dragHandle == CSizeHandleRect::InRect) {
                QList<QGraphicsItem *> items = scene->selectedItems();


                if (qAbs(vectorPoint.x()) > 0.0001 && qAbs(vectorPoint.y()) > 0.001) {
                    if (items.count() == 1) {

                        emit scene->itemMoved(m_currentSelectItem, m_sPointRelease - m_sPointPress );
                    } else {
                        emit scene->itemMoved(nullptr, m_sPointRelease - m_sPointPress );
                    }
                }

            } else {
                bool shiftKeyPress = CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
                bool altKeyPress = CDrawParamSigleton::GetInstance()->getAltKeyStatus();
                if (qAbs(vectorPoint.x()) > 0.0001 && qAbs(vectorPoint.y()) > 0.001) {
                    emit scene->itemResize(m_currentSelectItem, m_dragHandle, m_sPointPress, m_sPointRelease, shiftKeyPress, altKeyPress);
                }
            }
        }
    }
    scene->mouseEvent(event);

    /////////////////////////////////////////////////////////
    QList<QGraphicsItem *> list = scene->selectedItems();
    if (list.count() == 2) {
        for (QGraphicsItem *item : list) {
            if (item == m_currentSelectItem) {
                item->setSelected(false);
                list.removeOne(item);
            }
        }
        m_currentSelectItem = static_cast<CGraphicsItem *>(list.first());
        scene->changeAttribute(true, list.first());
    }
    /////////////////////////////////////////////////////////
}

void CSelectTool::selectionChange()
{
    if (m_currentSelectItem != nullptr) {
        if (m_currentSelectItem->type() == TextType) {
            static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getCGraphicsProxyWidget()->hide();
        }
    }
}


