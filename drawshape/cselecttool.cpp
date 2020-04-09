/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "widgets/ctextedit.h"
#include "cgraphicsmasicoitem.h"
#include "drawshape/cpictureitem.h"
#include "cgraphicsitemselectedmgr.h"
#include "cgraphicsitemhighlight.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "frame/cundocommands.h"

#include "service/cmanagerattributeservice.h"

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
//升序排列用
static bool zValueSortASC(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() >= info2->zValue();
}
//降序排列用
static bool zValueSortDES(QGraphicsItem *info1, QGraphicsItem *info2)
{
    return info1->zValue() <= info2->zValue();
}

CSelectTool::CSelectTool ()
    : IDrawTool (selection)
    , m_currentSelectItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bRotateAng(false)
    , m_rotateAng(0)
    , m_initRotateItemPos(0, 0)
    , m_RotateItem(nullptr)
    , m_textEditCursor(QPixmap(":/theme/light/images/mouse_style/text_mouse.svg"))
{
    m_frameSelectItem = new QGraphicsRectItem();
    m_frameSelectItem->setVisible(false);
    m_highlightItem = nullptr;
}

CSelectTool::~CSelectTool()
{

}

bool CSelectTool::isDragging()
{
    return m_isItemMoving;
}

void CSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    qDebug() << "mouse press" << endl;
    scene->getItemHighLight()->setVisible(false);
    if ( m_highlightItem != nullptr ) {
        m_currentSelectItem = m_highlightItem;
    }

    //鼠标点下时初始化一些标记
    m_doCopy          = false;
    m_isItemMoving    = false;
    m_isMulItemMoving = false;
    m_doResize        = false;

    if (event->button() == Qt::LeftButton) {
        bool ctrlKeyPress = scene->getDrawParam()->getCtlKeyStatus();
        if (ctrlKeyPress) {
            scene->clearSelection();
        }
        int count = scene->getItemsMgr()->getItems().size();
        qDebug() << "mouse press count = " << count << endl;
        bool altKeyPress = scene->getDrawParam()->getAltKeyStatus();

        //多选和单选复制
        if (altKeyPress && CSizeHandleRect::InRect == m_dragHandle) {
            QList<QGraphicsItem *> copyItems;
            copyItems.clear();
            QList<CGraphicsItem *> multSelectItems;
            if (count) {
                multSelectItems = scene->getItemsMgr()->getItems();
            } else if (m_currentSelectItem) {
                multSelectItems.clear();
                multSelectItems.append(static_cast<CGraphicsItem *>(m_currentSelectItem));
            }

            foreach (CGraphicsItem *multSelectItem, multSelectItems) {
                CGraphicsItem *copy = nullptr;
                switch (multSelectItem->type()) {
                case RectType:
                    copy = new CGraphicsRectItem();
                    break;
                case EllipseType:
                    copy = new CGraphicsEllipseItem();
                    break;
                case TriangleType:
                    copy = new CGraphicsTriangleItem();
                    break;
                case PolygonalStarType:
                    copy = new CGraphicsPolygonalStarItem();
                    break;

                case PolygonType:
                    copy = new CGraphicsPolygonItem();
                    break;
                case LineType:
                    copy = new CGraphicsLineItem();
                    break;

                case PenType:
                    copy = new CGraphicsPenItem();
                    break;
                case TextType:
                    copy = new CGraphicsTextItem();
                    break;

                case PictureType:
                    copy = new CPictureItem();
                    break;
                case BlurType:
                    copy = new CGraphicsMasicoItem();
                    break;

                }

                multSelectItem->duplicate(copy);
                copy->moveBy(10, 10);
                copyItems.append(copy);
                scene->addItem(static_cast<CGraphicsItem *>(copy));
                m_doCopy = true;
            }

            if (count) {
                scene->getItemsMgr()->clear();
                foreach (QGraphicsItem *copyItem, copyItems) {
                    scene->getItemsMgr()->addOrRemoveToGroup(static_cast<CGraphicsItem *>(copyItem));
                }
            } else if (copyItems.size() > 0) {
                scene->clearSelection();
                m_currentSelectItem = copyItems.at(0);
                m_currentSelectItem->setSelected(true);
            }

        }

        m_bMousePress = true;
        m_sPointPress = event->scenePos();
        m_sLastPress = m_sPointPress;
        //选中图元
        if (m_currentSelectItem != nullptr) {
            if (CSizeHandleRect::InRect == m_dragHandle && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                qApp->setOverrideCursor(m_textEditCursor);
            } else {
                qApp->setOverrideCursor(getCursor(m_dragHandle, m_bMousePress, 1));
            }
        }

        if (CSizeHandleRect::None == m_dragHandle || CSizeHandleRect::InRect == m_dragHandle) {
            //要先触发scene->mouseEvent(event);  才能获取是否有图元被选中
            scene->mouseEvent(event);
            //空白处点击，框选
            //设置颜色
            DPalette pa = scene->palette();
            //设置颜色
            QPen pen(pa.color(DPalette:: ObviousBackground));
            pen.setWidth(1);
            pen.setStyle(Qt::DotLine);
            m_frameSelectItem->setPen(pen);
            m_frameSelectItem->setBrush(pa.brush(QPalette::Active, DPalette:: Shadow));
            m_frameSelectItem->setRect(this->pointToRect(m_sPointPress, m_sPointPress));
            scene->addItem(m_frameSelectItem);
            //判断是否在画板空白处点击右键(在画板空白处点击才会出现框选)
            if (scene->items(event->scenePos()).count() == 0) {
                m_frameSelectItem->setVisible(true);
                scene->getItemsMgr()->clear();
            }

            bool shiftKeyPress = scene->getDrawParam()->getShiftKeyStatus();
            //shift键按下时
            QTransform transform;
            QList<QGraphicsItem *> selectItems = scene->items(event->scenePos());
            if (selectItems.contains(scene->getItemsMgr())) {
                selectItems.removeAll(scene->getItemsMgr());
            }
            if ( m_highlightItem != nullptr ) {
                if (!m_doCopy) {
                    m_currentSelectItem = m_highlightItem;
                }
                scene->mouseEvent(event);
                if (m_currentSelectItem && static_cast<CGraphicsItem *>(m_currentSelectItem)->type() != TextType) {
                    scene->clearSelection();
                }
                if (m_currentSelectItem) {
                    m_currentSelectItem->setSelected(true);
                }
                if (!shiftKeyPress) {
                    foreach (QGraphicsItem *selectItem, scene->selectedItems()) {
                        if (selectItem != m_currentSelectItem) {
                            selectItem->setSelected(false);
                        }
                    }
                }
                //未按下shift，选中管理图元所选之外的图元，清除管理图元中所选图元
                if (!shiftKeyPress && !altKeyPress) {
                    if (!scene->getItemsMgr()->getItems().contains(static_cast<CGraphicsItem *>(m_currentSelectItem))) {
                        scene->getItemsMgr()->clear();
                    }
                }
            } else {
                m_currentSelectItem = nullptr;
            }
            if ( m_currentSelectItem != nullptr ) {
                //需要区别图元或文字
                scene->updateBlurItem(m_currentSelectItem);
                //多选时不显示选中，拖动，旋转标志
                if (count > 1) {
                    scene->clearSelection();
                    emit scene->signalAttributeChanged(true, QGraphicsItem::UserType);
                }

                if (count <= 0) {
                    //如果是滤镜图层
                    if (m_currentSelectItem->type() == BlurType) {
                        static_cast<CGraphicsMasicoItem *>(m_currentSelectItem)->setPixmap();
                    } else if (m_currentSelectItem->type() == TextType) {
                        static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->cursorPositionChanged();
                    }
                    QGraphicsItem *currentSelectItem = nullptr;
                    for (auto curItem : selectItems) {
                        if (curItem->type() > QGraphicsItem::UserType && curItem->type() < MgrType) {
                            currentSelectItem = curItem;
                            break;
                        }
                    }
                    if (CSizeHandleRect::InRect == m_dragHandle && currentSelectItem != nullptr && currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(currentSelectItem)->getTextEdit()->isVisible()) {
                        qApp->setOverrideCursor(m_textEditCursor);
                    } else {
                        qApp->setOverrideCursor(getCursor(m_dragHandle, m_bMousePress, 1));
                    }
                    scene->changeAttribute(true, m_currentSelectItem);
                    CManagerAttributeService::getInstance()->updateSingleItemProperty(scene, m_currentSelectItem);
                }
            } else {
                m_currentSelectItem = nullptr;

                //点击空白处时要显示空白select工具的顶层属性界面(也就是一个"画板"标题)
                emit scene->signalChangeToSelect(true);

                scene->updateBlurItem();
            }
        }
        if (scene->getItemsMgr()->getItems().size() > 1) {
            scene->getItemsMgr()->setSelected(true);
        } else {
            scene->getItemsMgr()->setSelected(false);
        }
    } else if (event->button() == Qt::RightButton) {
        //弹出右键菜单时 鼠标变为箭头
        qDebug() << "right mouse pressed" << endl;
        m_dragHandle = CSizeHandleRect::None;
        qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        scene->mouseEvent(event);

        //判断是否在画板空白处点击右键(在画板空白处点击右键会自动取消选中)
        if (scene->selectedItems().count() == 0) {
            emit scene->signalChangeToSelect();
        }

    } else {
        scene->mouseEvent(event);
    }
}

void CSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //碰撞检测
    QList<QGraphicsItem *> items = scene->selectedItems();
    int multSelectItemsCount = scene->getItemsMgr()->getItems().size();
    if ( items.count() != 0 ) {
        QGraphicsItem *item = items.first();
        if (item != m_currentSelectItem) {
            m_currentSelectItem = item;
            m_currentSelectItem->setSelected(true);
            m_rotateAng = m_currentSelectItem->rotation();
            if (multSelectItemsCount <= 1) {
                scene->changeAttribute(true, item);
            }
        }
    } else {
        m_dragHandle = CSizeHandleRect::None;
        m_currentSelectItem = nullptr;
    }


    if (items.count() != 0 && nullptr != m_currentSelectItem && !m_bMousePress) {
        CGraphicsItem *itemCasted = dynamic_cast<CGraphicsItem *>(m_currentSelectItem);
        if (itemCasted) {
            CSizeHandleRect::EDirection dragHandle = itemCasted->hitTest(event->scenePos());

            if (dragHandle != m_dragHandle) {
                m_dragHandle = dragHandle;
                if (m_dragHandle == CSizeHandleRect::InRect && m_currentSelectItem->type() == TextType && static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                    qApp->setOverrideCursor(m_textEditCursor);
                } else {
                    qApp->setOverrideCursor(QCursor(getCursor(m_dragHandle, m_bMousePress, 1)));
                }
                m_rotateAng = m_currentSelectItem->rotation();
            }
        }
    }
    //鼠标移动到item边缘时item高亮
    if (  m_dragHandle < CSizeHandleRect::LeftTop || m_dragHandle > CSizeHandleRect::Rotation) {
        QPointF leftTop(event->scenePos().x() - 3, event->scenePos().y() - 3);
        QPointF bottomRight(event->scenePos().x() + 3, event->scenePos().y() + 3);
        QRectF rect(leftTop, bottomRight);
        QList<QGraphicsItem *> closeAllItems = scene->items(rect);
        for (int i = closeAllItems.size() - 1; i >= 0; i--) {
            if (closeAllItems.at(i)->zValue() == 0.0) {
                closeAllItems.removeAt(i);
                continue;
            }
            if (closeAllItems[i]->type() <= QGraphicsItem::UserType || closeAllItems[i]->type() >= EGraphicUserType::MgrType) {
                closeAllItems.removeAt(i);
            }
        }
        QGraphicsItem *closeItem = nullptr;
        m_highlightItem = nullptr;
        scene->getItemHighLight()->setVisible(false);
        QList<QGraphicsItem *> closeItems;
        if (closeAllItems.size() > 0) {
            //过滤掉有填充的图层以下的图元
            qSort(closeAllItems.begin(), closeAllItems.end(), zValueSortDES);
            for (int i = closeAllItems.size() - 1; i >= 0; i--) {
                closeItems.append(closeAllItems.at(i));
                if (static_cast<CGraphicsItem *>(closeAllItems.at(i))->brush().color().alpha() != 0 ||
                        static_cast<CGraphicsItem *>(closeAllItems.at(i))->type() == EGraphicUserType::PictureType ||
                        static_cast<CGraphicsItem *>(closeAllItems.at(i))->type() == EGraphicUserType::TextType) {
                    break;
                }
            }
            //既没填充也没秒描边
            if (closeItems.size() > 0) {
                qreal zValue = closeItems.at(0)->zValue();
                int maxZvalueIndex = 0;
                for (int i = 0; i < closeItems.size(); i++) {
                    if (closeItems.at(i)->zValue() >= zValue) {
                        zValue = closeItems.at(i)->zValue();
                        maxZvalueIndex = i;
                    }
                }
                if (static_cast<CGraphicsItem *>(closeItems.at(maxZvalueIndex))->brush().color().alpha() == 0 &&
                        (static_cast<CGraphicsItem *>(closeItems.at(maxZvalueIndex))->pen().color().alpha() == 0 ||
                         static_cast<CGraphicsItem *>(closeItems.at(maxZvalueIndex))->pen().width() == 0 ||
                         static_cast<CGraphicsItem *>(closeItems.at(maxZvalueIndex))->pen().widthF() == 0.0)) {
                    closeItem = closeItems.at(maxZvalueIndex);
                }
            }
            //QPainterPathStroker
            if (closeItem == nullptr) {
                for (int i = 0; i < closeItems.size(); i++) {
                    QPainterPathStroker stroker;
                    QPainterPath path;
                    stroker.setWidth(static_cast<CGraphicsItem *>(closeItems.at(i))->pen().width() + 2);
                    auto curItem = static_cast<CGraphicsItem *>(closeItems.at(i));
                    path = curItem->mapToScene(curItem->getHighLightPath());
                    path = stroker.createStroke(path);
                    if (path.contains(event->scenePos())) {
                        closeItem = closeItems.at(i);
                        break;
                    }
                }
            }
            //填充
            if (closeItem == nullptr) {
                for (int i = 0; i < closeItems.size(); i++) {
                    if (static_cast<CGraphicsItem *>(closeItems.at(i))->brush().color().alpha() != 0 ||
                            static_cast<CGraphicsItem *>(closeItems.at(i))->type() == EGraphicUserType::PictureType ||
                            static_cast<CGraphicsItem *>(closeItems.at(i))->type() == EGraphicUserType::TextType) {

                        closeItem = closeItems.at(i);
                        break;
                    }
                }
            }
            //最近
            if (closeItem == nullptr) {
                closeItem = getItemByMousePointToItemMinDistance(event->scenePos(), closeItems);
            }
            if (closeItem != nullptr) {
                auto curItem = static_cast<CGraphicsItem *>(closeItem);
                scene->getItemHighLight()->setPos(QPoint(0, 0));
                scene->getItemHighLight()->setPath(curItem->mapToScene(curItem->getHighLightPath()));
                scene->getItemHighLight()->setVisible(true);
                if (!m_bMousePress) {
                    m_highlightItem = closeItem;
                }

            } else {
                scene->getItemHighLight()->setVisible(false);
            }
        }
        if (m_bMousePress) {
            scene->getItemHighLight()->setVisible(false);
            scene->update();
        }
    }

    //左键按下，出现框选矩形
    if (m_bMousePress) {
        m_frameSelectItem->setRect(this->pointToRect(m_sPointPress, event->scenePos()));
    }

    // 再判断一次
    if (  m_dragHandle < CSizeHandleRect::LeftTop || m_dragHandle > CSizeHandleRect::Rotation) {
        if (m_currentSelectItem && static_cast<CGraphicsItem *>(m_currentSelectItem)->type() != TextType) {
            scene->clearSelection();
        }
        if (m_currentSelectItem) {
            items.clear();
            m_currentSelectItem->setSelected(true);
            items.append(m_currentSelectItem);
        } else {
            m_dragHandle = CSizeHandleRect::None;
            m_currentSelectItem = nullptr;
        }
    }

    if ( m_bMousePress) {
        if (m_dragHandle != CSizeHandleRect::None && m_dragHandle != CSizeHandleRect::Rotation && m_dragHandle != CSizeHandleRect::InRect) {
            if (scene->getItemsMgr()->getItems().size() > 1) {
                QPointF offsetPoint = event->scenePos() - m_sLastPress;
                bool shiftKeyPress = scene->getDrawParam()->getShiftKeyStatus();
                bool altKeyPress = scene->getDrawParam()->getAltKeyStatus();
                scene->getItemsMgr()->resizeTo(m_dragHandle, event->scenePos(), offsetPoint, shiftKeyPress, altKeyPress);
                m_doResize = true;
            } else {
                if (m_currentSelectItem) {
                    static_cast<CGraphicsItem *>(m_currentSelectItem)->resizeTo(m_dragHandle, event->scenePos());
                }
            }
        } else if (m_dragHandle == CSizeHandleRect::Rotation) {
            //旋转图形
            qreal angle = 0;
            m_bRotateAng = true;
            QPointF center = static_cast<CGraphicsItem *>(m_currentSelectItem)->rect().center();
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

                qreal scale = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getScale();

                m_RotateItem = new CGraphicsRotateAngleItem(angle, scale);
                m_RotateItem->setZValue(scene->getMaxZValue() + 1);

                scene->addItem(m_RotateItem);
                m_initRotateItemPos.setX(centerToScence.x());

                qreal space = 65. / scale;
                m_initRotateItemPos.setY(centerToScence.y() - static_cast<CGraphicsItem *>(m_currentSelectItem)->rect().height() / 2 - space);
                m_RotateItem->setPos(m_initRotateItemPos);
            } else {
                qreal angleRad = qDegreesToRadians(angle);

                qreal x0 = (m_initRotateItemPos.x() - centerToScence.x()) * qCos(angleRad) - (m_initRotateItemPos.y() - centerToScence.y()) * qSin(angleRad) + centerToScence.x() ;
                qreal y0 = (m_initRotateItemPos.x() - centerToScence.x()) * qSin(angleRad) + (m_initRotateItemPos.y() - centerToScence.y()) * qCos(angleRad) + centerToScence.y();

                m_RotateItem->updateRotateAngle(angle);
                m_RotateItem->setPos(x0, y0);

            }

        } else {
            if (scene->getItemsMgr()->getItems().size() > 1) {
                //移动
                if (!m_frameSelectItem->isVisible()) {
                    scene->getItemsMgr()->setSelected(true);
                    scene->clearSelection();
                    scene->getItemsMgr()->move(m_sLastPress, event->scenePos());
                    m_isMulItemMoving = true;
                    m_isItemMoving    = true;
                    scene->mouseEvent(event);
                }
                scene->mouseEvent(event);
            } else if (m_currentSelectItem != nullptr ) {
                if (m_currentSelectItem->type() != TextType) {
                    m_isItemMoving = true;
                    m_dragHandle = CSizeHandleRect::InRect;
                    static_cast<CGraphicsItem *>(m_currentSelectItem)->move(m_sLastPress, event->scenePos());
                } else {
                    //文字图元非编辑状态下是移动图元 编辑状态下是选中文字
                    if (!static_cast<CGraphicsTextItem *>(m_currentSelectItem)->isEditable()) {
                        m_isItemMoving = true;
                        static_cast<CGraphicsItem *>(m_currentSelectItem)->move(m_sLastPress, event->scenePos());
                    } else {
                        scene->mouseEvent(event);
                    }

                }
            }
        }
    } else {
        scene->mouseEvent(event);
    }

    if (scene->getItemsMgr()->getItems().size() > 1) {
        scene->clearSelection();
        scene->getItemsMgr()->setSelected(true);
    }
    m_sLastPress = event->scenePos();
}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    //复制添加动作入撤销返回栈
    if (m_doCopy) {
        QList<QGraphicsItem *> copyItems;
        copyItems.clear();
        QList<CGraphicsItem *> multSelectItems = scene->getItemsMgr()->getItems();
        foreach (CGraphicsItem *multSelectItem, multSelectItems) {
            copyItems.append(multSelectItem);
        }
        if (copyItems.size() == 0) {
            copyItems.append(static_cast<CGraphicsItem *>(m_currentSelectItem));
        }
        QUndoCommand *addCommand = new CAddShapeCommand(scene, copyItems);
        CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(addCommand);
    }
    m_doCopy = false;

    scene->removeItem(m_frameSelectItem);
    m_frameSelectItem->setVisible(false);
    //左键按下，出现框选矩形
    if (m_bMousePress && (m_currentSelectItem == nullptr)) {
        QList<QGraphicsItem *> items = scene->items(m_frameSelectItem->rect());
        foreach (QGraphicsItem *item, items) {
            if (item == m_frameSelectItem) {
                continue;
            }
            auto selectItem = static_cast<CGraphicsItem *>(item);
            scene->getItemsMgr()->addOrRemoveToGroup(selectItem);
        }
        int count = scene->getItemsMgr()->getItems().size();
        if (1 == count) {
            scene->getItemsMgr()->getItems().first()->setSelected(true);
            scene->getItemsMgr()->hide();
        } else if (count > 1) {
            scene->getItemsMgr()->show();
            scene->getItemsMgr()->setSelected(true);
        }
    }

    bool shiftKeyPress = scene->getDrawParam()->getShiftKeyStatus();
    bool altKeyress = scene->getDrawParam()->getAltKeyStatus();
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;
        m_sPointRelease = event->scenePos();
        QPointF vectorPoint = m_sPointRelease - m_sPointPress;

        //shift键按下时
        if (shiftKeyPress) {
            if (m_currentSelectItem) {
                auto currentSelectItem = static_cast<CGraphicsItem *>(m_currentSelectItem);
                if (currentSelectItem != nullptr) {
                    scene->getItemsMgr()->addOrRemoveToGroup(currentSelectItem);
                }
            }
            int count = scene->getItemsMgr()->getItems().size();
            if (1 == count) {
                scene->getItemsMgr()->hide();
            } else if (count > 1) {
                scene->getItemsMgr()->show();
                scene->clearSelection();
                scene->getItemsMgr()->setSelected(true);
                emit scene->signalAttributeChanged(true, QGraphicsItem::UserType);
            }
            m_bMousePress = false;
            scene->mouseEvent(event);
            if (count > 1) {
                scene->clearSelection();
                scene->getItemsMgr()->setSelected(true);
            }
        }

        if (scene->getItemsMgr()->getItems().size() <= 1) {
            if (m_currentSelectItem != nullptr) {
                if (CSizeHandleRect::InRect == m_dragHandle &&
                        m_currentSelectItem != nullptr &&
                        m_currentSelectItem->type() == TextType &&
                        static_cast<CGraphicsTextItem *>(m_currentSelectItem)->getTextEdit()->isVisible()) {
                    qApp->setOverrideCursor(m_textEditCursor);
                } else {
                    qApp->setOverrideCursor(getCursor(m_dragHandle, m_bMousePress, 1));
                }

                //最后需要刷新一次
                if (m_currentSelectItem && m_currentSelectItem->type() == BlurType) {
                    static_cast<CGraphicsMasicoItem *>(m_currentSelectItem)->setPixmap();
                }

                if (m_dragHandle == CSizeHandleRect::Rotation) {
                    if (m_RotateItem) {
                        delete m_RotateItem;
                        m_RotateItem = nullptr;
                    }
                    if (qAbs(vectorPoint.x()) > 0.0001 && qAbs(vectorPoint.y()) > 0.001) {
                        emit scene->itemRotate(m_currentSelectItem, m_rotateAng);
                    }
                } else if (m_dragHandle == CSizeHandleRect::InRect) {
                    if (qAbs(vectorPoint.x()) > 0.0001 && qAbs(vectorPoint.y()) > 0.001) {
                        CGraphicsTextItem *textItem = dynamic_cast<CGraphicsTextItem *>(m_currentSelectItem);

                        if (textItem == nullptr || !textItem->isEditable()) {
                            emit scene->itemMoved(m_currentSelectItem, m_sPointRelease - m_sPointPress );
                        }
                    }
                } else {
                    bool shiftKeyPress = scene->getDrawParam()->getShiftKeyStatus();
                    bool altKeyPress = scene->getDrawParam()->getAltKeyStatus();
                    if (qAbs(vectorPoint.x()) > 0.0001 && qAbs(vectorPoint.y()) > 0.001) {
                        emit scene->itemResize(static_cast<CGraphicsItem *>(m_currentSelectItem), m_dragHandle, m_sPointPress, m_sPointRelease, shiftKeyPress, altKeyPress);
                    }
                }
            }

            scene->mouseEvent(event);
            if ( m_currentSelectItem != nullptr ) {
                if (static_cast<CGraphicsItem *>(m_currentSelectItem)->type() != TextType) {
                    scene->clearSelection();
                }
                m_currentSelectItem->setSelected(true);
                //显示所选图元素属性
                scene->changeAttribute(true, m_currentSelectItem);
                CManagerAttributeService::getInstance()->updateSingleItemProperty(scene, m_currentSelectItem);
            }
            if (scene->getItemsMgr()->getItems().size() > 1) {
                scene->getItemsMgr()->setSelected(true);
            } else {
                scene->getItemsMgr()->setSelected(false);
            }
        } else {
            if (m_isMulItemMoving) {
                QUndoCommand *addCommand = new CMultMoveShapeCommand(scene, m_sPointPress, m_sPointRelease);
                CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(addCommand);
                m_isMulItemMoving = false;
            } else if (m_doResize) {
                bool shiftKeyPress = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getShiftKeyStatus();
                bool altKeyPress = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getAltKeyStatus();
                QUndoCommand *addCommand = new CMultResizeShapeCommand(scene, m_dragHandle, m_sPointPress, m_sPointRelease, shiftKeyPress, altKeyPress);
                CManageViewSigleton::GetInstance()->getCurView()->pushUndoStack(addCommand);
                m_doResize = false;
            }

            if (scene->getItemsMgr()->getItems().size() > 1) {
                scene->clearSelection();
                scene->getItemsMgr()->setSelected(true);
            }
        }

        m_isMulItemMoving = false;
        m_isItemMoving    = false;
    }

    //更新模糊图元
    QList<QGraphicsItem *> allitems = scene->items();
    foreach (QGraphicsItem *item, allitems) {
        if (item->type() == BlurType) {
            static_cast<CGraphicsMasicoItem *>(item)->setPixmap();
        }
    }
    if (m_RotateItem) {
        delete m_RotateItem;
        m_RotateItem = nullptr;
    }
    scene->mouseEvent(event);
}

QRectF CSelectTool::pointToRect(QPointF point1, QPointF point2)
{
    QRectF rectangle;
    QPointF leftTop(0, 0);
    QPointF bottomRight(0, 0);
    if (point1.x() <= point2.x()) {
        leftTop.setX(point1.x());
        bottomRight.setX(point2.x());
    } else {
        leftTop.setX(point2.x());
        bottomRight.setX(point1.x());
    }
    if (point1.y() <= point2.y()) {
        leftTop.setY(point1.y());
        bottomRight.setY(point2.y());
    } else {
        leftTop.setY(point2.y());
        bottomRight.setY(point1.y());
    }
    rectangle.setTopLeft(leftTop);
    rectangle.setBottomRight(bottomRight);
    return  rectangle;
}

QGraphicsItem *CSelectTool::getItemByMousePointToItemMinDistance(QPointF mousePoint, QList<QGraphicsItem *> detectItems)
{
    if (detectItems.size() <= 0) {
        return nullptr;
    }
    QGraphicsItem *minDistanceItem = detectItems.at(0);
    double minDistance = getItemMinDistanceByMousePointToItem(mousePoint, minDistanceItem);
    for (int i = 1; i < detectItems.count(); i++) {
        double detectDistance = getItemMinDistanceByMousePointToItem(mousePoint, detectItems.at(i));
        if (detectDistance < minDistance) {
            minDistanceItem = detectItems.at(i);
        }
    }
    return minDistanceItem;
}

double CSelectTool::getItemMinDistanceByMousePointToItem(QPointF mousePoint, QGraphicsItem *detectItems)
{
    // 定义从path中取出的比较点个数
    double detectPointCount = 100;

    double min_distance = 100.0;
    for (double j = 0.01; j < 1.0; j = j + 1.0 / detectPointCount) {
        // 获取当前点的位置
        //QPointF current_shap_point = detectItems->shape().pointAtPercent(j);
        QPointF current_shap_point = static_cast<CGraphicsItem *>(detectItems)->getHighLightPath().pointAtPercent(j);

        // 计算当前点到鼠标的距离
        double current_mouse_to_shape_distance = sqrt(pow(mousePoint.x() - current_shap_point.x(), 2) + pow(mousePoint.y() - current_shap_point.y(), 2));
        // 获取当前形状的最小距离
        min_distance = qMin(min_distance, current_mouse_to_shape_distance);
    }
    return min_distance;
}

