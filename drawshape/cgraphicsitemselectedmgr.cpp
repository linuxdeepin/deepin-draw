#include "cgraphicsitemselectedmgr.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
#include "cgraphicsitem.h"
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
    initRect();
}

void CGraphicsItemSelectedMgr::addOrRemoveToGroup(CGraphicsItem *item)
{
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
        CManagerAttributeService::getInstance()->showSelectedCommonProperty(static_cast<CDrawScene *>(scene()), m_listItems);
    }
    if (m_listItems.size() > 1) {
        foreach (QGraphicsItem *item, m_listItems) {
            static_cast<CGraphicsItem * >(item)->setMutiSelect(true);
        }
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
        //rect = rect.united(item->boundingRect());
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
        m_listItems.push_back(item);
        static_cast<CGraphicsItem * >(item)->setMutiSelect(true);
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
    //qDebug() << "CGraphicsItemSelectedMgr resizeTo 2" << endl;
    return;
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->resizeTo(dir, point, bShiftPress, bAltPress);
    }
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
                if ((mousePos.x() - geomMult.left()) > 20) {
                    xScale = offset.x() / geomMult.width();
                    yScale = 0;
                    rectCffset.setX((itemRect.left() - geomMult.left()) * xScale);
                    rectCffset.setY(0);
                    item->resizeTo(CSizeHandleRect::Right, rectCffset, xScale, yScale, shiftKeyPress, altKeyPress);
                }
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

//void CGraphicsItemSelectedMgr::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    if (CDrawParamSigleton::GetInstance()->getShiftKeyStatus() && event->button() == Qt::LeftButton) {
//        QPointF pos = event->scenePos();
//        foreach (QGraphicsItem *item, this->childItems()) {
//            //需要坐标转换
//            if (item->shape().contains(item->mapFromScene(pos))) {
//                this->removeFromGroup(item);
//                break;
//            }
//        }
//    }
//}


void CGraphicsItemSelectedMgr::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    updateGeometry();
    if (m_listItems.size() > 1) {
        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(this->boundingRect());
    }
}

void CGraphicsItemSelectedMgr::initRect()
{
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
