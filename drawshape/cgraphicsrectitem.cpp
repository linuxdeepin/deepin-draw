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
#include "cgraphicsrectitem.h"
#include "csizehandlerect.h"
#include <QPainter>
#include <QPixmap>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsBlurEffect>

CGraphicsRectItem::CGraphicsRectItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    initRect();
}

CGraphicsRectItem::CGraphicsRectItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    initRect();
}

CGraphicsRectItem::CGraphicsRectItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    initRect();
}

CGraphicsRectItem::CGraphicsRectItem(const SGraphicsRectUnitData &rectData, const SGraphicsUnitHead &head, CGraphicsItem *parent)
    : CGraphicsItem(head, parent)
{
    //    QPointF leftTop;
    //    QPointF rightBottom;

    //    if (RectType == unit.head.dataType) {
    //        leftTop = QPointF(unit.data.pRect->leftTopX, unit.data.pRect->leftTopY);
    //        rightBottom = QPointF(unit.data.pRect->rightBottomX, unit.data.pRect->rightBottomY);
    //    } else if (PictureType == unit.head.dataType) {
    //        leftTop = QPointF(unit.data.pPic->rect.leftTopX, unit.data.pPic->rect.leftTopY);
    //        rightBottom = QPointF(unit.data.pPic->rect.rightBottomX, unit.data.pPic->rect.rightBottomY);
    //    }


    this->m_topLeftPoint = rectData.topLeft;
    this->m_bottomRightPoint =  rectData.bottomRight;
    this->setTransformOriginPoint(this->rect().center());


    initRect();
}

CGraphicsRectItem::~CGraphicsRectItem()
{

}

int CGraphicsRectItem::type() const
{
    return RectType;
}

void CGraphicsRectItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    updateGeometry();
}

void CGraphicsRectItem::initRect()
{
    // handles
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Rotation; ++i) {
        CSizeHandleRect *shr = nullptr;
        if (i == CSizeHandleRect::Rotation) {
            shr   = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i), QString(":/theme/light/images/mouse_style/icon_rotate.svg"));

        } else {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        }
        m_handles.push_back(shr);

    }
    updateGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    updateGeometry();
    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRect(rect());

    if (this->isSelected()) {
        painter->setClipping(false);
        QPen pen;
        pen.setWidthF(1 / CDrawParamSigleton::GetInstance()->getScale());
        if ( CDrawParamSigleton::GetInstance()->getThemeType() == 1) {
            pen.setColor(QColor(224, 224, 224));
        } else {
            pen.setColor(QColor(69, 69, 69));
        }
        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }

    /*QGraphicsScene *scene = this->scene();
    //绘制滤镜
    if (scene != nullptr) {
        //QPixmap pixmap(int(this->scene()->width()), int(this->scene()->height()));
        //pixmap.fill();

        QPixmap pixmap(this->scene()->sceneRect().width(), this->scene()->sceneRect().height());

        QPainter painterd(&pixmap);
        painterd.setRenderHint(QPainter::Antialiasing);
        painterd.setRenderHint(QPainter::SmoothPixmapTransform);
        this->scene()->render(&painterd);
        painter->drawPixmap(this->rect(), pixmap, QRectF());





        QPainter painterd(&pixmap);
        painterd.setRenderHint(QPainter::Antialiasing);
        painterd.setRenderHint(QPainter::SmoothPixmapTransform);
        //painterd.setClipRect(this->mapRectToScene(this->rect()));

        bool flag = this->isSelected();
        //this->setVisible(false);
        scene->render(&painterd);
        //this->setVisible(true);
        this->setSelected(flag);

        QPixmap tmpPixmap = pixmap.copy(this->mapRectToScene(this->rect()).toRect());
        int imgWidth = tmpPixmap.width();
        int imgHeigth = tmpPixmap.height();
        int radius = 5;
        tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter->drawPixmap(this->rect(), tmpPixmap, QRectF());

        //        painter->drawPixmap(this->rect(), pixmap, this->rect());
    }*/

}

void CGraphicsRectItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    bool shiftKeyPress = CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
    bool altKeyPress = CDrawParamSigleton::GetInstance()->getAltKeyStatus();
    resizeTo(dir, point, shiftKeyPress, altKeyPress);

}

void CGraphicsRectItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
{
    QPointF local = mapFromScene(point);
    QRectF rect = this->rect();
    QPointF topLeft = rect.topLeft();
    QPointF centerPoint = rect.center();
    qreal w = rect.width();
    qreal h = rect.height();
    qreal scale = w / h;//长宽比

    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;

    if (!shiftKeyPress && !altKeyPress) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                rect.setRight(local.x());
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                rect.setTopRight(local);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomRight(local);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomLeft(local);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                rect.setBottom(local.y());
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                rect.setTopLeft(local);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                rect.setLeft(local.x());
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                rect.setTop(local.y());
            }
            break;
        default:
            break;
        }
    }
    //按住SHIFT等比拉伸
    else if ((shiftKeyPress && !altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                //变换后的宽度和高度
                qreal w2 = local.x() - rect.left();
                qreal h2 = w2 / scale;
                topLeft.setY(topLeft.y() - (h2 - h) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                //rect.setTopRight(local);
                QPointF bottomLeft = rect.bottomLeft();
                qreal w2 = local.x() - bottomLeft.x();
                qreal h2 = bottomLeft.y() - local.y();

                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                topLeft.setY(topLeft.y() - (h2 - h));

                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                //rect.setBottomRight(local);

                qreal w2 = local.x() - topLeft.x();
                qreal h2 = local.y() - topLeft.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
                //rect.setBottomLeft(local);
                QPointF topRight = rect.topRight();
                qreal w2 = topRight.x() - local.x();
                qreal h2 = local.y() - topRight.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                topLeft.setX(topRight.x() - w2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);

            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                //rect.setBottom(local.y());
                qreal h2 = local.y() - topLeft.y();
                qreal w2 = h2 * scale;
                topLeft.setX(topLeft.x() - (w2 - w) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                //rect.setTopLeft(local);
                QPointF bottomRight = rect.bottomRight();
                qreal w2 = bottomRight.x() - local.x();
                qreal h2 = bottomRight.y() - local.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                topLeft = bottomRight - QPointF(w2, h2);
                rect.setTopLeft(topLeft);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                //rect.setLeft(local.x());
                qreal w2 = rect.right() - local.x();
                qreal h2 = w2 / scale;
                topLeft.setX(local.x());
                topLeft.setY(topLeft.y() - (h2 - h) / 2);
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                //rect.setTop(local.y());

                qreal h2 = rect.bottom() - local.y();
                qreal w2 = h2 * scale;
                topLeft.setX(topLeft.x() - (w2 - w) / 2);
                topLeft.setY(local.y());
                rect.setTopLeft(topLeft);
                rect.setWidth(w2);
                rect.setHeight(h2);
            }
            break;
        default:
            break;
        }
    }
    //中心拉伸
    else if ((!shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                //rect.setRight(local.x());
                rect.setRight(local.x());
                rect.setLeft(centerPoint.x() * 2 - local.x());
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                rect.setTopRight(local);
                rect.setBottomLeft(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomRight(local);
                rect.setTopLeft(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {
                rect.setBottomLeft(local);
                rect.setTopRight(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                //rect.setBottom(local.y());
                rect.setBottom(local.y());
                rect.setTop(centerPoint.y() * 2 - local.y());
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                rect.setTopLeft(local);
                rect.setBottomRight(2 * centerPoint - local);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                rect.setLeft(local.x());
                rect.setRight(2 * centerPoint.x() - local.x());
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                rect.setTop(local.y());
                rect.setBottom(2 * centerPoint.y() - local.y());
            }
            break;
        default:
            break;
        }
    }
    //等比中心拉伸
    else if ((shiftKeyPress && altKeyPress) ) {
        switch (dir) {
        case CSizeHandleRect::Right:
            if (local.x() - rect.left() > 0.1 ) {
                //变换后的宽度和高度
                qreal h2 = local.x() - centerPoint.x();
                qreal w2 = h2 * scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::RightTop:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.bottom() < 0.1) {
                qreal w2 = local.x() - centerPoint.x();
                qreal h2 = centerPoint.y() - local.y();

                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (local.x() - rect.left() > 0.1 && local.y() - rect.top() > 0.1) {
                //rect.setBottomRight(local);

                qreal w2 = local.x() - centerPoint.x();
                qreal h2 = local.y() - centerPoint.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.top() > 0.1) {

                qreal w2 = centerPoint.x() - local.x();
                qreal h2 = local.y() - centerPoint.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::Bottom:
            if (local.y() - rect.top() > 0.1 ) {
                qreal h2 = local.y() - centerPoint.y();
                qreal w2 = h2 * scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (local.x() - rect.right() < 0.1 && local.y() - rect.bottom() < 0.1 ) {
                qreal w2 = centerPoint.x() - local.x();
                qreal h2 = centerPoint.y() - local.y();
                if (h2 * scale >= w2) {
                    w2 = h2 * scale;
                } else {
                    h2 = w2 / scale;
                }
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::Left:
            if (rect.right() - local.x() > 0.1 ) {
                qreal w2 = centerPoint.x() - local.x();
                qreal h2 = w2 / scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        case CSizeHandleRect::Top:
            if (local.y() - rect.bottom() < 0.1 ) {
                qreal h2 = centerPoint.y() - local.y();
                qreal w2 = h2 * scale;
                QPointF topLeft = centerPoint - QPointF(w2, h2);
                QPointF rightBottom = centerPoint + QPointF(w2, h2);
                rect = QRectF(topLeft, rightBottom);
            }
            break;
        default:
            break;
        }
    }

    rect = rect.normalized();
    centerPoint = mapToScene(rect.center());
    rect.setRect(centerPoint.rx() - rect.width() * 0.5, centerPoint.ry() - rect.height() * 0.5, rect.width(), rect.height());
    prepareGeometryChange();
    setTransformOriginPoint(centerPoint);
    this->setPos(0, 0);

    this->setRect(rect);
    updateGeometry();
}

void CGraphicsRectItem::duplicate(CGraphicsItem *item)
{
    static_cast<CGraphicsRectItem * >(item)->setRect(this->rect());
    CGraphicsItem::duplicate(item);
}

CGraphicsUnit CGraphicsRectItem::getGraphicsUnit() const
{
    CGraphicsUnit unit;

    unit.head.dataType = this->type();
    unit.head.dataLength = sizeof(SGraphicsRectUnitData);
    unit.head.pen = this->pen();
    unit.head.brush = this->brush();
    unit.head.pos = this->pos();
    unit.head.rotate = this->rotation();
    unit.head.zValue = this->zValue();


    unit.data.pRect = new SGraphicsRectUnitData();
    unit.data.pRect->topLeft = this->m_topLeftPoint;
    unit.data.pRect->bottomRight = this->m_bottomRightPoint;

    return unit;
}


void CGraphicsRectItem::updateGeometry()
{
    const QRectF &geom = this->boundingRect();

    const Handles::iterator hend =  m_handles.end();
    QPointF pos;
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        CSizeHandleRect *hndl = *it;
        qreal w = hndl->boundingRect().width();
        qreal h = hndl->boundingRect().height();
        switch (hndl->dir()) {
        case CSizeHandleRect::LeftTop:
            hndl->move(geom.x() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Top:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::RightTop:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() - h / 2);
            break;
        case CSizeHandleRect::Right:
            pos = QPointF(geom.x() + geom.width() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::RightBottom:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Bottom:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::LeftBottom:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case CSizeHandleRect::Left:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case CSizeHandleRect::Rotation:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h - h / 2);
            break;
        default:
            break;
        }
    }
}

QRectF CGraphicsRectItem::rect() const
{
    return QRectF(m_topLeftPoint, m_bottomRightPoint);
}

QPainterPath CGraphicsRectItem::shape() const
{
    QPainterPath path;
    path.addRect(rect());
    path.closeSubpath();
    return qt_graphicsItem_shapeFromPath(path, pen());
}

QRectF CGraphicsRectItem::boundingRect() const
{
//    QRectF rect = this->rect();
//    QRectF bounding = QRectF(rect.x() - pen().width() / 2, rect.y() - pen().width() / 2,
//                             rect.width() + pen().width(), rect.height() + pen().width());
//    return bounding;

    return shape().controlPointRect();
}

//void CGraphicsRectItem::setState(SelectionHandleState st)
//{
//    const Handles::iterator hend =  m_handles.end();
//    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
//        (*it)->setState(st);
//    }
//}
