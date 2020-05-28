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
CGraphicsItemSelectedMgr::CGraphicsItemSelectedMgr(QGraphicsItem *parent)
    : CGraphicsItem (parent)
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

    updateGeometry();
}

void CGraphicsItemSelectedMgr::clear()
{
    prepareGeometryChange();
    foreach (QGraphicsItem *item, m_listItems) {
        //item->setParentItem(nullptr);
        static_cast<CGraphicsItem * >(item)->setMutiSelect(false);
    }
    m_listItems.clear();
    updateGeometry();
}

QRectF CGraphicsItemSelectedMgr::boundingRect() const
{
    QRectF rect;
    foreach (QGraphicsItem *item, m_listItems) {
        rect = rect.united(item->mapRectToScene(item->boundingRect()));
    }

    rect = mapFromScene(rect).boundingRect();
    return rect;
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

    updateGeometry();
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
    updateGeometry();
}

void CGraphicsItemSelectedMgr::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    qDebug() << "CGraphicsItemSelectedMgr resizeTo 1" << endl;
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->resizeTo(dir, point);
    }
}

void CGraphicsItemSelectedMgr::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point, bool bShiftPress, bool bAltPress)
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
        else if ((shiftKeyPress && !altKeyPress) ) {
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
        else if ((!shiftKeyPress && altKeyPress) ) {
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
        else if ((shiftKeyPress && altKeyPress) ) {
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
        item->resizeTo(dir, pressRect, m_mapItemsRect[item],  xScale, yScale, bShiftPress, bAltPress);
    }
    updateGeometry();
}

void CGraphicsItemSelectedMgr::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &mousePos, const QPointF &offset, bool bShiftPress, bool bAltPress)
{
    bool shiftKeyPress = bShiftPress;
    bool altKeyPress = bAltPress;
    prepareGeometryChange();
    const QRectF &geomMult = mapRectToScene(this->boundingRect());
    double xScale = 1;
    double yScale = 1;
    QPointF rectCffset(0, 0);
    foreach (CGraphicsItem *item, m_listItems) {
        const QRectF &itemRect = item->mapToScene(item->boundingRect()).boundingRect();
        if (!shiftKeyPress && !altKeyPress) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                if ((geomMult.bottom() - mousePos.y()) > 20 && (geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    rectCffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if ((geomMult.bottom() - mousePos.y()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = 0;
                    rectCffset.setX(0);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeTo(CSizeHandleRect::Top, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightTop:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    rectCffset.setX((itemRect.left() - geomMult.left()) * (xScale ));
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom()) * (yScale));
                    item->resizeTo(CSizeHandleRect::RightTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                xScale = offset.x() / geomMult.width();
                yScale = 0;
                rectCffset.setX((itemRect.left() - geomMult.left()) * xScale);
                rectCffset.setY(0);
                item->resizeTo(CSizeHandleRect::Right, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                break;
            case CSizeHandleRect::RightBottom:
                if ((mousePos.x() - geomMult.left()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale =  offset.y() / geomMult.height();
                    rectCffset.setX((itemRect.left() - geomMult.left()) * xScale );
                    rectCffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeTo(CSizeHandleRect::RightBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.y() - geomMult.top()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = 0;
                    rectCffset.setX(0);
                    rectCffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeTo(CSizeHandleRect::Bottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::LeftBottom:
                if ((geomMult.right() - mousePos.x()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = offset.y() / geomMult.height();
                    rectCffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = 0;
                    rectCffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectCffset.setY(0);
                    item->resizeTo(CSizeHandleRect::Left, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
        //按住SHIFT等比拉伸
        else if ((shiftKeyPress && !altKeyPress) ) {
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
                    rectCffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if ((geomMult.bottom() - mousePos.y()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = yScale;
                    rectCffset.setX(-(itemRect.right() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeTo(CSizeHandleRect::Top, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
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
                    rectCffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom()) * yScale);
                    item->resizeTo(CSizeHandleRect::RightTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = xScale;
                    rectCffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Right, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
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
                    rectCffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeTo(CSizeHandleRect::RightBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.y() - geomMult.top()) > 20) {
                    yScale = offset.y() / geomMult.height();
                    xScale = yScale;
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeTo(CSizeHandleRect::Bottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
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
                    rectCffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top()) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = xScale;
                    rectCffset.setX((geomMult.right() - itemRect.right()) * xScale);
                    rectCffset.setY(-(itemRect.bottom() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Left, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
        //中心拉伸
        else if ((!shiftKeyPress && altKeyPress) ) {
            switch (dir) {
            case CSizeHandleRect::LeftTop:
                if ((geomMult.bottom() - mousePos.y()) > 20 && (geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if ((geomMult.bottom() - mousePos.y()) > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = 0;
                    rectCffset.setX(0);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Top, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightTop:
                if ((mousePos.x() - geomMult.left()) > 20 && (geomMult.bottom() - mousePos.y()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::RightTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = 0;
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY(0);
                    item->resizeTo(CSizeHandleRect::Right, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::RightBottom:
                if ((mousePos.x() - geomMult.left()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::RightBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.y() - geomMult.top()) > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = 0;
                    rectCffset.setX(0);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Bottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::LeftBottom:
                if ((geomMult.right() - mousePos.x()) > 20 && (mousePos.y() - geomMult.top()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = offset.y() * 2 / geomMult.height();
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = 0;
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY(0);
                    item->resizeTo(CSizeHandleRect::Left, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }
        //等比中心拉伸
        else if ((shiftKeyPress && altKeyPress) ) {
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
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Top:
                if (geomMult.bottom() - mousePos.y() > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = yScale;
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Top, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
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
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::RightTop, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Right:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = xScale;
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Right, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
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
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::RightBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Bottom:
                if ((mousePos.x() - geomMult.left()) > 20) {
                    yScale = offset.y() * 2 / geomMult.height();
                    xScale = yScale;
                    rectCffset.setX((itemRect.left() - geomMult.left() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Bottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
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
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((itemRect.top() - geomMult.top() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::LeftBottom, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            case CSizeHandleRect::Left:
                if ((geomMult.right() - mousePos.x()) > 20) {
                    xScale = offset.x() * 2 / geomMult.width();
                    yScale = xScale;
                    rectCffset.setX((geomMult.right() - itemRect.right() - geomMult.width() / 2) * xScale);
                    rectCffset.setY((geomMult.bottom() - itemRect.bottom() - geomMult.height() / 2) * yScale);
                    item->resizeTo(CSizeHandleRect::Left, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
                break;
            default:
                break;
            }
        }

    }
    updateGeometry();
}


void CGraphicsItemSelectedMgr::move(QPointF beginPoint, QPointF movePoint)
{
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->move(beginPoint, movePoint);
    }
}

int CGraphicsItemSelectedMgr::type() const
{
    return MgrType;
}

QRectF CGraphicsItemSelectedMgr::rect() const
{
    return QRectF();
}

void CGraphicsItemSelectedMgr::updateGeometry()
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
        default:
            break;
        }
    }
    if (m_listItems.size() == 1) {
        m_listItems.at(0)->setSelected(true);
        this->setSelected(false);
    }
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
    updateGeometry();
    if (m_listItems.size() > 1) {
        painter->setClipping(false);
        QPen pen;
        pen.setColor(QColor("#E0E0E0"));
        qreal setValue = 1 / painter->worldTransform().m11();
        painter->setRenderHint(QPainter::Antialiasing, false);
        pen.setWidthF(setValue);
        //qDebug() << "finale value0 = " << (setValue * painter->worldTransform().m11());
        //qDebug() << "finale value1 = " << (pen.widthF() * painter->worldTransform().m11());
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(this->boundingRect());
        painter->setClipping(true);
    }
}

void CGraphicsItemSelectedMgr::initHandle()
{
    clearHandle();

    m_handles.reserve(CSizeHandleRect::None);

    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Left; ++i) {
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

bool CGraphicsItemSelectedMgr::couldResize(QRectF itemSceneBoundRect, QPointF mousePoint, CSizeHandleRect::EDirection dragHandle, bool bShiftPress, bool bAltPress)
{
    bool couldResize = true;
    QRectF rect = itemSceneBoundRect;
    int length = 20;
    //普通拉伸
    //按住SHIFT等比拉伸
    if ((!bShiftPress && !bAltPress ) ||
            (bShiftPress && !bAltPress)) {
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
    else if ((!bShiftPress && bAltPress) ) {
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
            if (((rect.left() + rect.width() / 2 - mousePoint.x() ) <= length / 2) ||
                    (mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Bottom:
            if ((mousePoint.y() - rect.top() - rect.height() / 2) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (((rect.left() + rect.width() / 2 - mousePoint.x() ) <= length / 2) ||
                    ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Left:
            if ((rect.left() + rect.width() / 2 - mousePoint.x() ) <= length / 2) {
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
    } else if ((bShiftPress && bAltPress) ) {
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
            if (((rect.left() + rect.width() / 2 - mousePoint.x() ) <= length / 2) ||
                    (mousePoint.y() - rect.top()) <= length) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Bottom:
            if ((mousePoint.y() - rect.top() - rect.height() / 2) <= length / 2) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::LeftTop:
            if (((rect.left() + rect.width() / 2 - mousePoint.x() ) <= length / 2) ||
                    ((rect.top() + rect.height() / 2 - mousePoint.y()) <= length / 2)) {
                couldResize = false;
            }
            break;
        case CSizeHandleRect::Left:
            if ((rect.left() + rect.width() / 2 - mousePoint.x() ) <= length / 2) {
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

QPointF CGraphicsItemSelectedMgr::getMinPoint(QRectF itemSceneBoundRect, QPointF mousePoint, CSizeHandleRect::EDirection dragHandle, bool bShiftPress, bool bAltPress)
{
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
