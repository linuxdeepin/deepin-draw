#include "cgraphicsitemselectedmgr.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
#include "cgraphicsitem.h"
#include "cgraphicspenitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
CGraphicsItemSelectedMgr::CGraphicsItemSelectedMgr(QGraphicsItem *parent)
    : CGraphicsItem(parent)
{
    m_listItems.clear();
    //假定10000是最顶层
    //this->setZValue(10000);
    initHandle();
}

void CGraphicsItemSelectedMgr::addOrRemoveToGroup(CGraphicsItem *item)
{
    if (item == nullptr)
        return;

    //防止添加自己
    if (item == this)
        return;
    if (m_listItems.size() == 1 && m_listItems.contains(item)) {
        return;
    }
    if (m_listItems.contains(item)) {
        this->removeFromGroup(item);
        if (m_listItems.size() == 1) {
            m_listItems.at(0)->setMutiSelect(false);
        }
    } else {
        this->addToGroup(item);
    }
    if (m_listItems.size() > 1) {
        foreach (QGraphicsItem *item, m_listItems) {
            static_cast<CGraphicsItem * >(item)->setMutiSelect(true);
        }
    }

    updateBoundingRect();
}

void CGraphicsItemSelectedMgr::clear()
{
    prepareGeometryChange();
    foreach (QGraphicsItem *item, m_listItems) {
        //item->setParentItem(nullptr);
        static_cast<CGraphicsItem * >(item)->setMutiSelect(false);
    }
    m_listItems.clear();
    updateBoundingRect();
}

QRectF CGraphicsItemSelectedMgr::boundingRect() const
{
    return _rct;
}

void CGraphicsItemSelectedMgr::updateBoundingRect()
{
    QRectF rect(0, 0, 0, 0);

    if (m_listItems.size() > 1) {
        CGraphicsItem::rotatAngle(0);

        foreach (QGraphicsItem *item, m_listItems) {
            rect = rect.united(item->mapRectToScene(item->boundingRect()));
        }

        rect = mapFromScene(rect).boundingRect();

    } else if (m_listItems.size() == 1) {
        CGraphicsItem *pItem = m_listItems.first();

        _rct = pItem->boundingRect();

        setPos(pItem->pos());

        CGraphicsItem::rotatAngle(pItem->rotation());

        updateGeometry();

        return;
    }

    _rct = rect;

    updateGeometry();
}

QPainterPath CGraphicsItemSelectedMgr::shape() const
{
    QPainterPath path;
//    foreach (QGraphicsItem *item, m_listItems) {
//        path = path.united(item->mapToScene(item->shape()));
//    }

    path.addRect(this->boundingRect());

    return path;
}

QList<CGraphicsItem *> CGraphicsItemSelectedMgr::getItems() const
{
    return m_listItems;
}

void CGraphicsItemSelectedMgr::addToGroup(CGraphicsItem *item)
{
    //防止添加自己
    if (item == this)
        return;
    prepareGeometryChange();
    if (!m_listItems.contains(item)) {
        if (dynamic_cast<CGraphicsItem *>(item) != nullptr) {
            m_listItems.push_back(item);
            item->setMutiSelect(true);
        }
    }

    updateBoundingRect();
}

void CGraphicsItemSelectedMgr::removeFromGroup(CGraphicsItem *item)
{
    //防止删除自己
    if (item == this)
        return;
    prepareGeometryChange();
    if (m_listItems.contains(item)) {
        m_listItems.removeOne(item);
        static_cast<CGraphicsItem * >(item)->setMutiSelect(false);
    }
    if (m_listItems.size() == 1) {
        m_listItems.at(0)->setMutiSelect(false);
    }
    updateBoundingRect();
}

void CGraphicsItemSelectedMgr::newResizeTo(CSizeHandleRect::EDirection dir,
                                           const QPointF &mousePos,
                                           const QPointF &offset,
                                           bool bShiftPress, bool bAltPress)
{
    resizeAll(dir, mousePos, offset, bShiftPress, bAltPress);
}

void CGraphicsItemSelectedMgr::rotatAngle(qreal angle)
{
    if (m_listItems.count() == 1) {
        m_listItems.first()->rotatAngle(angle);
        updateBoundingRect();
    }
}

void CGraphicsItemSelectedMgr::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    qDebug() << "CGraphicsItemSelectedMgr resizeTo 1" << endl;
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->resizeTo(dir, point);
    }
}

void CGraphicsItemSelectedMgr::resizeTo(CSizeHandleRect::EDirection dir,
                                        const QPointF &point,
                                        bool bShiftPress, bool bAltPress)
{
    prepareGeometryChange();
    QRectF pressRect = m_mapItemsRect[this];
    if (!couldResize(pressRect, point, dir, bShiftPress, bAltPress)) {
        return;
    }
    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;
    //超出可移动范围后重新计算缩放点
    QPointF mousePos = getMinPoint(pressRect, point, dir, bShiftPress, bAltPress);
    prepareGeometryChange();
    qreal xScale = 1;
    qreal yScale = 1;
    foreach (CGraphicsItem *item, m_listItems) {
        if (!shiftKeyPress && !altKeyPress) {
            switch (dir) {
            case CSizeHandleRect::LeftTop: {
                xScale = (pressRect.right() - mousePos.x()) / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y()) / pressRect.height();
            }
            break;
            case CSizeHandleRect::Top: {
                xScale = 1;
                yScale = (pressRect.bottom() - mousePos.y()) / pressRect.height();
            }
            break;
            case CSizeHandleRect::RightTop: {
                xScale = (mousePos.x() - pressRect.left()) / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y()) / pressRect.height();
            }
            break;
            case CSizeHandleRect::Right: {
                xScale = (mousePos.x() - pressRect.left()) / pressRect.width();
                yScale = 0;
            }
            break;
            case CSizeHandleRect::RightBottom: {
                xScale = (mousePos.x() - pressRect.left()) / pressRect.width();
                yScale = (mousePos.y() - pressRect.top()) / pressRect.height();
            }
            break;
            case CSizeHandleRect::Bottom: {
                xScale = 1;
                yScale = (mousePos.y() - pressRect.top()) / pressRect.height();
            }
            break;
            case CSizeHandleRect::LeftBottom: {
                xScale = (pressRect.right() - mousePos.x()) / pressRect.width();
                yScale = (mousePos.y() - pressRect.top()) / pressRect.height();
            }
            break;
            case CSizeHandleRect::Left: {
                xScale = (pressRect.right() - mousePos.x()) / pressRect.width();
                yScale = 1;
            }
            break;
            default:
                break;
            }
        }
        //按住SHIFT等比拉伸
        else if ((shiftKeyPress && !altKeyPress)) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                xScale = (pressRect.right() - mousePos.x()) / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y()) / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Top: {
                yScale = (pressRect.bottom() - mousePos.y()) / pressRect.height();
                xScale = yScale;
            }
            break;
            case CSizeHandleRect::RightTop:
                xScale = (mousePos.x() - pressRect.left()) / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y()) / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Right: {
                qDebug() << "CGraphicsItemSelectedMgr CSizeHandleRect::Right bShiftPress" <<  endl;
                xScale = (mousePos.x() - pressRect.left()) / pressRect.width();
                yScale = xScale;
            }
            break;
            case CSizeHandleRect::RightBottom:
                xScale = (mousePos.x() - pressRect.left()) / pressRect.width();
                yScale = (mousePos.y() - pressRect.top()) / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Bottom: {
                yScale = (mousePos.y() - pressRect.top()) / pressRect.height();
                xScale = yScale;
            }
            break;
            case CSizeHandleRect::LeftBottom:
                xScale = (pressRect.right() - mousePos.x()) / pressRect.width();
                yScale = (mousePos.y() - pressRect.top()) / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Left: {
                xScale = (pressRect.right() - mousePos.x()) / pressRect.width();
                yScale = xScale;
            }
            break;
            default:
                break;
            }
        }
        //中心拉伸
        else if ((!shiftKeyPress && altKeyPress)) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                xScale = (pressRect.right() - mousePos.x() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y() - pressRect.height() / 2) * 2 / pressRect.height();
                break;
            case CSizeHandleRect::Top:
                xScale = 0;
                yScale = (pressRect.bottom() - mousePos.y() - pressRect.height() / 2) * 2 / pressRect.height();
                break;
            case CSizeHandleRect::RightTop:
                xScale = (mousePos.x() - pressRect.left() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y() - pressRect.height() / 2) * 2 / pressRect.height();
                break;
            case CSizeHandleRect::Right:
                qDebug() << "CGraphicsItemSelectedMgr CSizeHandleRect::Right bAltPress" <<  endl;
                xScale = (mousePos.x() - pressRect.left() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = 0;
                break;
            case CSizeHandleRect::RightBottom:
                xScale = (mousePos.x() - pressRect.left() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (mousePos.y() - pressRect.top() - pressRect.height() / 2) * 2 / pressRect.height();
                break;
            case CSizeHandleRect::Bottom:
                xScale = 0;
                yScale = (mousePos.y() - pressRect.top() - pressRect.height() / 2) * 2 / pressRect.height();
                break;
            case CSizeHandleRect::LeftBottom:
                xScale = (pressRect.right() - mousePos.x() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (mousePos.y() - pressRect.top() - pressRect.height() / 2) * 2 / pressRect.height();
                break;
            case CSizeHandleRect::Left:
                qDebug() << "CGraphicsItemSelectedMgr CSizeHandleRect::Left bAltPress" <<  endl;
                xScale = (pressRect.right() - mousePos.x() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = 0;
                break;
            default:
                break;
            }
        }
        //等比中心拉伸
        else if ((shiftKeyPress && altKeyPress)) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                xScale = (pressRect.right() - mousePos.x() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y() - pressRect.height() / 2) * 2 / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Top:
                yScale = (pressRect.bottom() - mousePos.y() - pressRect.height() / 2) * 2 / pressRect.height();
                xScale = yScale;
                break;
            case CSizeHandleRect::RightTop:
                xScale = (mousePos.x() - pressRect.left() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (pressRect.bottom() - mousePos.y() - pressRect.height() / 2) * 2 / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Right:
                qDebug() << "CGraphicsItemSelectedMgr CSizeHandleRect::Right bAltPress" <<  endl;
                xScale = (mousePos.x() - pressRect.left() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = xScale;
                break;
            case CSizeHandleRect::RightBottom:
                xScale = (mousePos.x() - pressRect.left() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (mousePos.y() - pressRect.top() - pressRect.height() / 2) * 2 / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Bottom:
                yScale = (mousePos.y() - pressRect.top() - pressRect.height() / 2) * 2 / pressRect.height();
                xScale = yScale;
                break;
            case CSizeHandleRect::LeftBottom:
                xScale = (pressRect.right() - mousePos.x() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = (mousePos.y() - pressRect.top() - pressRect.height() / 2) * 2 / pressRect.height();
                if (qAbs(xScale) >= qAbs(yScale)) {
                    yScale = xScale;
                } else {
                    xScale = yScale;
                }
                break;
            case CSizeHandleRect::Left:
                qDebug() << "CGraphicsItemSelectedMgr CSizeHandleRect::Left bAltPress" <<  endl;
                xScale = (pressRect.right() - mousePos.x() - pressRect.width() / 2) * 2 / pressRect.width();
                yScale = xScale;
                break;
            default:
                break;
            }
        }
        item->resizeToMul_7(dir, pressRect, m_mapItemsRect[item],  xScale, yScale, bShiftPress, bAltPress);
    }
    updateBoundingRect();
}

void CGraphicsItemSelectedMgr::resizeAll(CSizeHandleRect::EDirection dir, const QPointF &mousePos,
                                         const QPointF &offset, bool bShiftPress, bool bAltPress)
{
    if (m_listItems.count() == 1) {
        m_listItems.first()->resizeTo(dir, mousePos);
        updateBoundingRect();
        return;
    }

    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;
    prepareGeometryChange();
    const QRectF &geomMult = mapRectToScene(this->boundingRect());
    double xScale = 1;
    double yScale = 1;
    QPointF rectOffset(0, 0);
    foreach (CGraphicsItem *item, m_listItems) {
        const QRectF &itemRect = item->mapToScene(item->boundingRect()).boundingRect();
        if (!shiftKeyPress && !altKeyPress) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                if ((geomMult.bottom() - mousePos.y()) > 20 && (geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    rectOffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if ((geomMult.bottom() - mousePos.y()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = 0;
                    rectOffset.setX(0);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeToMul(CSizeHandleRect::Top, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightTop:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    rectOffset.setX((itemRect.left() - geomMult.left()) * (xScale));
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom()) * (yScale));
                    item->resizeToMul(CSizeHandleRect::RightTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = 0;
                    rectOffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectOffset.setY(0);
                    item->resizeToMul(CSizeHandleRect::Right, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightBottom:
                if ((mousePos.x() - geomMult.left()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale =  offset.y() / geomMult.height();
                    rectOffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.y() - geomMult.top()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = 0;
                    rectOffset.setX(0);
                    rectOffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeToMul(CSizeHandleRect::Bottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::LeftBottom:
                if ((geomMult.right() - mousePos.x()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    rectOffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = 0;
                    rectOffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectOffset.setY(0);
                    item->resizeToMul(CSizeHandleRect::Left, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
        //按住SHIFT等比拉伸
        else if ((shiftKeyPress && !altKeyPress)) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                if ((geomMult.bottom() - mousePos.y()) > 20 && (geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = xScale;
                    } else {
                        xScale = yScale;
                    }
                    rectOffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if ((geomMult.bottom() - mousePos.y()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = yScale;
                    rectOffset.setX(-(itemRect.right() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeToMul(CSizeHandleRect::Top, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightTop:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = -xScale;
                    } else {
                        xScale = -yScale;
                    }
                    rectOffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = xScale;
                    rectOffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Right, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightBottom:
                if ((mousePos.x() - geomMult.left()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = xScale;
                    } else {
                        xScale = yScale;
                    }
                    rectOffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.y() - geomMult.top()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = yScale;
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeToMul(CSizeHandleRect::Bottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::LeftBottom:
                if ((geomMult.right() - mousePos.x()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = -xScale;
                    } else {
                        xScale = -yScale;
                    }
                    rectOffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = xScale;
                    rectOffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectOffset.setY(-(itemRect.bottom() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Left, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
        //中心拉伸
        else if ((!shiftKeyPress && altKeyPress)) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                if ((geomMult.bottom() - mousePos.y()) > 20 && (geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if ((geomMult.bottom() - mousePos.y()) > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = 0;
                    rectOffset.setX(0);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Top, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightTop:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = 0;
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY(0);
                    item->resizeToMul(CSizeHandleRect::Right, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightBottom:
                if ((mousePos.x() - geomMult.left()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.y() - geomMult.top()) > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = 0;
                    rectOffset.setX(0);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Bottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::LeftBottom:
                if ((geomMult.right() - mousePos.x()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = 0;
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY(0);
                    item->resizeToMul(CSizeHandleRect::Left, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
        //等比中心拉伸
        else if ((shiftKeyPress && altKeyPress)) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                if ((geomMult.bottom() - mousePos.y()) > 20 && (geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = xScale;
                    } else {
                        xScale = yScale;
                    }
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if (geomMult.bottom() - mousePos.y() > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = yScale;
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Top, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightTop:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = -xScale;
                    } else {
                        xScale = -yScale;
                    }
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightTop, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = xScale;
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Right, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightBottom:
                if ((mousePos.x() - geomMult.left()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = xScale;
                    } else {
                        xScale = yScale;
                    }
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::RightBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = yScale;
                    rectOffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Bottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::LeftBottom:
                if ((geomMult.right() - mousePos.x()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    if (qAbs(xScale) <= qAbs(yScale)) {
                        yScale = -xScale;
                    } else {
                        xScale = -yScale;
                    }
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::LeftBottom, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = xScale;
                    rectOffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectOffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeToMul(CSizeHandleRect::Left, rectOffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
    }
    updateBoundingRect();
}

bool CGraphicsItemSelectedMgr::isResizableWithInfo(CSizeHandleRect::EDirection dir,
                                                   QPointF &mousePos,
                                                   QPointF &offset,
                                                   bool bShiftPress,
                                                   bool bAltPress)
{
    bool accept = true;
    QPointF newMousePos = mousePos;
    QPointF outOffset   = offset;
    qreal minLen = 20;
    QRectF curScenRect = mapRectToScene(this->boundingRect());
    if (!bShiftPress && !bAltPress) {
        switch (dir) {
        case CSizeHandleRect::LeftTop: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            }
            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - minLen;
            qreal wantedTop = curTop + offset.y();
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Top: {
            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - minLen;
            qreal wantedTop = curTop + offset.y();
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
            }
            qreal realMoveY = wantedTop - curTop;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minTop);
            }
            break;
        }
        case CSizeHandleRect::RightTop: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + minLen;
            qreal wantedRight = curRight + offset.x();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            }

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - minLen;
            qreal wantedTop = curTop + offset.y();
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Right: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + minLen;
            qreal wantedRight = curRight + offset.x();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            }
            break;
        }
        case CSizeHandleRect::RightBottom: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + minLen;
            qreal wantedRight = curRight + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + minLen;
            qreal wantedBottom = curBottom + offset.y();

            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            }
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Bottom: {
            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + minLen;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
            }
            qreal realMoveY = wantedBottom - curBottom;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minBottom);
            }
            break;
        }
        case CSizeHandleRect::LeftBottom: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            }
            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + minLen;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Left: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
            }
            qreal realMoveX = wantedLeft - curLeft;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minLeft);
            }
            break;
        }
        default:
            break;
        }
    } else if (bShiftPress && !bAltPress) {
        switch (dir) {
        case CSizeHandleRect::LeftTop: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - minLen;
            qreal wantedTop = curTop + offset.y();

            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            } else if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Top: {
            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - minLen;
            qreal wantedTop = curTop + offset.y();
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
            }
            qreal realMoveY = wantedTop - curTop;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minTop);
            }
            break;
        }
        case CSizeHandleRect::RightTop: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + minLen;
            qreal wantedRight = curRight + offset.x();

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - minLen;
            qreal wantedTop = curTop + offset.y();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            } else if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Right: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + minLen;
            qreal wantedRight = curRight + offset.x();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;

            }
            qreal realMoveX = wantedRight - curRight;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minRight);
            }
            break;
        }
        case CSizeHandleRect::RightBottom: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + minLen;
            qreal wantedRight = curRight + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + minLen;
            qreal wantedBottom = curBottom + offset.y();

            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            } else if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Bottom: {
            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + minLen;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
            }
            qreal realMoveY = wantedBottom - curBottom;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minBottom);
            }
            break;
        }
        case CSizeHandleRect::LeftBottom: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + minLen;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            } else if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Left: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
            }
            qreal realMoveX = wantedLeft - curLeft;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minLeft);
            }
            break;
        }
        default:
            break;
        }
    } else if (!bShiftPress && bAltPress) {
        switch (dir) {
        case CSizeHandleRect::LeftTop: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - curScenRect.width() / 2 - minLen / 2;
            qreal wantedLeft = curLeft + offset.x();

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - curScenRect.height() / 2 - minLen / 2;
            qreal wantedTop = curTop + offset.y();

            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            }
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Top: {
            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - curScenRect.height() / 2 - minLen / 2;
            qreal wantedTop = curTop + offset.y();
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
            }
            qreal realMoveY = wantedTop - curTop;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minTop);
            }
            break;
        }
        case CSizeHandleRect::RightTop: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + curScenRect.width() / 2 + minLen / 2;
            qreal wantedRight = curRight + offset.x();

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - curScenRect.height() / 2 - minLen / 2;
            qreal wantedTop = curTop + offset.y();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            }
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Right: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + curScenRect.width() / 2 + minLen / 2;
            qreal wantedRight = curRight + offset.x();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;

            }
            qreal realMoveX = wantedRight - curRight;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minRight);
            }
            break;
        }
        case CSizeHandleRect::RightBottom: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + curScenRect.width() / 2 + minLen / 2;
            qreal wantedRight = curRight + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + curScenRect.height() / 2 + minLen / 2;
            qreal wantedBottom = curBottom + offset.y();

            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            }
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Bottom: {
            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + curScenRect.height() / 2 + minLen / 2;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
            }
            qreal realMoveY = wantedBottom - curBottom;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minBottom);
            }
            break;
        }
        case CSizeHandleRect::LeftBottom: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - curScenRect.width() / 2 - minLen / 2;
            qreal wantedLeft = curLeft + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + curScenRect.height() / 2 + minLen / 2;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            }
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Left: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - curScenRect.width() / 2 - minLen / 2;
            qreal wantedLeft = curLeft + offset.x();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
            }
            qreal realMoveX = wantedLeft - curLeft;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minLeft);
            }
            break;
        }
        default:
            break;
        }
    } else if (bShiftPress && bAltPress) {
        switch (dir) {
        case CSizeHandleRect::LeftTop: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - curScenRect.width() / 2 - minLen / 2;
            qreal wantedLeft = curLeft + offset.x();

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - curScenRect.height() / 2 - minLen / 2;
            qreal wantedTop = curTop + offset.y();

            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            } else if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Top: {
            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - curScenRect.height() / 2 - minLen / 2;
            qreal wantedTop = curTop + offset.y();
            if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
            }
            qreal realMoveY = wantedTop - curTop;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minTop);
            }
            break;
        }
        case CSizeHandleRect::RightTop: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + curScenRect.width() / 2 + minLen / 2;
            qreal wantedRight = curRight + offset.x();

            qreal curTop = curScenRect.top();
            qreal minTop = curScenRect.bottom() - curScenRect.height() / 2 - minLen / 2;
            qreal wantedTop = curTop + offset.y();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            } else if (wantedTop > minTop) {
                wantedTop = minTop;
                accept = false;
                qreal realMoveY = wantedTop - curTop;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minTop);
                }
            }
            break;
        }
        case CSizeHandleRect::Right: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + curScenRect.width() / 2 + minLen / 2;
            qreal wantedRight = curRight + offset.x();
            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;

            }
            qreal realMoveX = wantedRight - curRight;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minRight);
            }
            break;
        }
        case CSizeHandleRect::RightBottom: {
            qreal curRight = curScenRect.right();
            qreal minRight = curScenRect.left() + curScenRect.width() / 2 + minLen / 2;
            qreal wantedRight = curRight + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + curScenRect.height() / 2 + minLen / 2;
            qreal wantedBottom = curBottom + offset.y();

            if (wantedRight < minRight) {
                wantedRight = minRight;
                accept = false;
                qreal realMoveX = wantedRight - curRight;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minRight);
                }
            } else if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Bottom: {
            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + curScenRect.height() / 2 + minLen / 2;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
            }
            qreal realMoveY = wantedBottom - curBottom;
            outOffset.setY(realMoveY);
            QPointF prePos = mousePos - offset;
            newMousePos.setY(prePos.y() + outOffset.y());
            if (!accept) {
                newMousePos.setY(minBottom);
            }
            break;
        }
        case CSizeHandleRect::LeftBottom: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - curScenRect.width() / 2 - minLen / 2;
            qreal wantedLeft = curLeft + offset.x();

            qreal curBottom = curScenRect.bottom();
            qreal minBottom = curScenRect.top() + curScenRect.height() / 2 + minLen / 2;
            qreal wantedBottom = curBottom + offset.y();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
                qreal realMoveX = wantedLeft - curLeft;
                outOffset.setX(realMoveX);
                QPointF prePos = mousePos - offset;
                newMousePos.setX(prePos.x() + outOffset.x());
                if (!accept) {
                    newMousePos.setX(minLeft);
                }
            } else if (wantedBottom < minBottom) {
                wantedBottom = minBottom;
                accept = false;
                qreal realMoveY = wantedBottom - curBottom;
                outOffset.setY(realMoveY);
                QPointF prePos = mousePos - offset;
                newMousePos.setY(prePos.y() + outOffset.y());
                if (!accept) {
                    newMousePos.setY(minBottom);
                }
            }
            break;
        }
        case CSizeHandleRect::Left: {
            qreal curLeft = curScenRect.left();
            qreal minLeft = curScenRect.right() - minLen;
            qreal wantedLeft = curLeft + offset.x();
            if (wantedLeft > minLeft) {
                wantedLeft = minLeft;
                accept = false;
            }
            qreal realMoveX = wantedLeft - curLeft;
            outOffset.setX(realMoveX);
            QPointF prePos = mousePos - offset;
            newMousePos.setX(prePos.x() + outOffset.x());
            if (!accept) {
                newMousePos.setX(minLeft);
            }
            break;
        }
        default:
            break;
        }
    }
    mousePos = newMousePos;
    offset = outOffset;
    return accept;
}

void CGraphicsItemSelectedMgr::move(QPointF beginPoint, QPointF movePoint)
{
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->move(beginPoint, movePoint);
    }
    updateBoundingRect();
}

int CGraphicsItemSelectedMgr::type() const
{
    return MgrType;
}

QRectF CGraphicsItemSelectedMgr::rect() const
{
    return _rct;
}

void CGraphicsItemSelectedMgr::updateGeometry()
{
    const QRectF &geom = this->boundingRect();

    //如果大小无效了那么就没有显示的必要了
    if (geom.isValid()) {
        show();
    } else {
        hide();
        return;
    }

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
        case CSizeHandleRect::Rotation: {
            if (m_listItems.count() > 1) {
                hndl->hide();
            } else {
                hndl->show();
                hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h - h / 2);
            }
            break;
        }
        default:
            break;
        }
    }

    //    if (m_listItems.size() == 1) {
    //        m_listItems.at(0)->setSelected(true);
    //        this->setSelected(false);
    //    }
}

void CGraphicsItemSelectedMgr::recordItemsRect()
{
    foreach (CGraphicsItem *item, m_listItems) {
        m_mapItemsRect[item] = item->sceneBoundingRect();
        if (item->type() == PenType) {
            CGraphicsPenItem *penItem = dynamic_cast<CGraphicsPenItem *>(item);
            if (penItem) {
                penItem->savePathBeforResize(penItem->getPath());
            }
        }
    }
    m_mapItemsRect[this] = this->sceneBoundingRect();
}

QRectF CGraphicsItemSelectedMgr::getMultItemRect()
{
    return m_mapItemsRect[this];
}

void CGraphicsItemSelectedMgr::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipping(false);
    QPen pen;

    painter->setRenderHint(QPainter::Antialiasing, true);

    pen.setWidthF(1 / option->levelOfDetailFromTransform(painter->worldTransform()));

    pen.setColor(QColor("#E0E0E0"));

    painter->setPen(pen);
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->drawRect(this->boundingRect());
    painter->setClipping(true);
}

void CGraphicsItemSelectedMgr::initHandle()
{
    clearHandle();

    m_handles.reserve(CSizeHandleRect::None);

    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Rotation; ++i) {
        CSizeHandleRect *shr = nullptr;
        if (i == CSizeHandleRect::Rotation) {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i),
                                      QString(":/theme/light/images/mouse_style/icon_rotate.svg"));

        } else {
            shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        }
        m_handles.push_back(shr);
    }
    updateBoundingRect();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

bool CGraphicsItemSelectedMgr::couldResize(QRectF itemSceneBoundRect, QPointF mousePoint, CSizeHandleRect::EDirection dragHandle, bool bShiftPress, bool bAltPress)
{
    bool couldResize = true;
    QRectF rect = itemSceneBoundRect;
    int length = 20;
    //普通拉伸
    //按住SHIFT等比拉伸
    if ((!bShiftPress && !bAltPress) || (bShiftPress && !bAltPress)) {
        switch (dragHandle) {
        case CSizeHandleRect::Right:
            if ((mousePoint.x() - rect.left()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::RightTop:
            if ((mousePoint.x() - rect.left()) <= length ||
                    (rect.bottom() - mousePoint.y()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::RightBottom:
            if ((mousePoint.x() - rect.left()) <= length ||
                    (mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if ((rect.right() - mousePoint.x()) <= length ||
                    (mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Bottom:
            if ((mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftTop:
            if ((rect.right() - mousePoint.x()) <= length ||
                    (rect.bottom() - mousePoint.y()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Left:
            if ((rect.right() - mousePoint.x()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Top:
            if ((rect.bottom() - mousePoint.y()) <= length) {
                couldResize = false;
            }
            break;
        default:
            break;
        }
    }
    //中心拉伸
    else if ((!bShiftPress && bAltPress)) {
        switch (dragHandle) {
        case CSizeHandleRect::Right:
            if ((mousePoint.x() - rect.left() - rect.width() / 2) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::RightTop:
            if (((mousePoint.x() - rect.left() - rect.width() / 2) <= length / 2) ||
                    ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (((mousePoint.x() - rect.left() - rect.width() / 2) <= length / 2) ||
                    ((mousePoint.y() - rect.top() - rect.height() / 2) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (((rect.left() + rect.width() / 2 - mousePoint.x()) <= length / 2) || (mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Bottom:
            if ((mousePoint.y() - rect.top() - rect.height() / 2) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (((rect.left() + rect.width() / 2 - mousePoint.x()) <= length / 2) || ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Left:
            if ((rect.left() + rect.width() / 2 - mousePoint.x()) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Top:
            if ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2) {
                couldResize = false;
            }
            break;
        default:
            break;
        }
    } else if ((bShiftPress && bAltPress)) {
        switch (dragHandle) {
        case CSizeHandleRect::Right:
            if ((mousePoint.x() - rect.left() - rect.width() / 2) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::RightTop:
            if (((mousePoint.x() - rect.left() - rect.width() / 2) <= length / 2) ||
                    ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::RightBottom:
            if (((mousePoint.x() - rect.left() - rect.width() / 2) <= length / 2) ||
                    ((mousePoint.y() - rect.top() - rect.height() / 2) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftBottom:
            if (((rect.left() + rect.width() / 2 - mousePoint.x()) <= length / 2) || (mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Bottom:
            if ((mousePoint.y() - rect.top() - rect.height() / 2) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (((rect.left() + rect.width() / 2 - mousePoint.x()) <= length / 2) || ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Left:
            if ((rect.left() + rect.width() / 2 - mousePoint.x()) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Top:
            if ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2) {
                couldResize = false;
            }
            break;
        default:
            break;
        }
    }
    //等比中心拉伸


    return couldResize;
}

QPointF CGraphicsItemSelectedMgr::getMinPoint(QRectF itemSceneBoundRect, QPointF mousePoint,
                                              CSizeHandleRect::EDirection dragHandle,
                                              bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)

    QPointF point = mousePoint;
    QRectF rect = itemSceneBoundRect;
    int length = 20;
    switch (dragHandle) {
    case CSizeHandleRect::Right:
        if ((mousePoint.x() - rect.left()) <= length) {
            mousePoint.setX(rect.left() + length);
        }
        break;
    case CSizeHandleRect::RightTop:
        if ((mousePoint.x() - rect.left()) <= length ||
                (rect.bottom() - mousePoint.y()) <= length) {
            mousePoint.setX(rect.left() + length);
            mousePoint.setY(rect.bottom() - length);
        }
        break;
    case CSizeHandleRect::RightBottom:
        if ((mousePoint.x() - rect.left()) <= length ||
                (mousePoint.y() - rect.top()) <= length) {
            mousePoint.setX(rect.left() + length);
            mousePoint.setY(rect.top() + length);
        }
        break;
    case CSizeHandleRect::LeftBottom:
        if ((rect.right() - mousePoint.x()) <= length ||
                (mousePoint.y() - rect.top()) <= length) {
            mousePoint.setX(rect.right() - length);
            mousePoint.setY(rect.top() + length);
        }
        break;
    case CSizeHandleRect::Bottom:
        if ((mousePoint.y() - rect.top()) <= length) {
        }
        break;
    case CSizeHandleRect::LeftTop:
        if ((rect.right() - mousePoint.x()) <= length ||
                (rect.bottom() - mousePoint.y()) <= length) {
        }
        break;
    case CSizeHandleRect::Left:
        if ((rect.right() - mousePoint.x()) <= length) {
        }
        break;
    case CSizeHandleRect::Top:
        if ((rect.bottom() - mousePoint.y()) <= length) {
        }
        break;
    default:
        break;
    }
    return point;
}
