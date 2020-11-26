#include "cgraphicsitemselectedmgr.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
//#include "cgraphicsrotateangleitem.h"
#include "cgraphicsitem.h"
#include "cgraphicspenitem.h"
#include "toptoolbar.h"
#include "citemattriwidget.h"
#include "cgraphicstextitem.h"
#include "application.h"
#include "cdrawtoolmanagersigleton.h"
#include "ccuttool.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsScene>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

CGraphicsItemGroup::CGraphicsItemGroup(EGroupType tp, const QString &nam)
    : CGraphicsItem(nullptr)
{
    setGroupType(tp);

    setName(nam);

    m_listItems.clear();

    initHandle();

    static int s_indexForTest = 0;
    _indexForTest = s_indexForTest;
    ++s_indexForTest;
}

QString CGraphicsItemGroup::name()
{
    return _name;
}

void CGraphicsItemGroup::setName(const QString &name)
{
    _name = name;
}

CGraphicsItemGroup::EGroupType CGraphicsItemGroup::groupType()
{
    return _type;
}

void CGraphicsItemGroup::setGroupType(CGraphicsItemGroup::EGroupType tp)
{
    _type = tp;
}

bool CGraphicsItemGroup::isTopBzGroup()
{
    return bzGroup() == nullptr;
}

void CGraphicsItemGroup::setCancelable(bool enable)
{
    _isCancelable = enable;
}

bool CGraphicsItemGroup::isCancelable()
{
    return _isCancelable;
}

void CGraphicsItemGroup::clear()
{
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->setMutiSelect(false);
        item->setBzGroup(nullptr);
    }
    m_listItems.clear();
    updateBoundingRect();
    updateAttributes();
}

QRectF CGraphicsItemGroup::boundingRect() const
{
    return _rct;
}

void CGraphicsItemGroup::updateBoundingRect()
{
    prepareGeometryChange();

    QRectF rect(0, 0, 0, 0);

    if (m_listItems.size() > 1) {

        foreach (QGraphicsItem *item, m_listItems) {
            CGraphicsItem *pItem = dynamic_cast<CGraphicsItem *>(item);
            if (pItem != nullptr)
                rect = rect.united(item->mapRectToScene(pItem->boundingRectTruly()));
        }

        this->setTransformOriginPoint(rect.center());

        this->setRotation(0);

        _rct = mapFromScene(rect).boundingRect();

    } else if (m_listItems.size() == 1) {
        CGraphicsItem *pItem = m_listItems.first();

        //不存在节点的图元就需要多选图元进行管理
        if (!pItem->isSizeHandleExisted() || drawScene()->isNormalGroupItem(pItem)) {
            _rct = pItem->boundingRectTruly();

            this->setTransformOriginPoint(pItem->transformOriginPoint());

            this->setRotation(pItem->rotation());

            this->setPos(pItem->pos());

            this->setTransform(pItem->transform());
        }
    } else {
        _rct = rect;
    }

    m_boundingRectTrue = _rct;

    updateHandlesGeometry();
}

QPainterPath CGraphicsItemGroup::getSelfOrgShape() const
{
    QPainterPath path;

    path.addRect(this->boundingRect());

    return path;
}

qreal CGraphicsItemGroup::incLength() const
{
    return 0;
}

int CGraphicsItemGroup::count()
{
    return m_listItems.count();
}

QList<CGraphicsItem *> CGraphicsItemGroup::items(bool recursiveFind) const
{
    if (!recursiveFind)
        return m_listItems;
    QList<CGraphicsItem *> result;
    for (auto p : m_listItems) {
        result.append(p);
        if (p->isBzGroup()) {
            CGraphicsItemGroup *pGroup = static_cast<CGraphicsItemGroup *>(p);
            result.append(pGroup->items(recursiveFind));
        }
    }
    return result;
}

QList<CGraphicsItem *> CGraphicsItemGroup::getBzItems(bool recursiveFind) const
{
    QList<CGraphicsItem *> result;
    for (auto p : m_listItems) {
        if (p->isBzItem()) {
            result.append(p);
        } else if (recursiveFind && p->isBzGroup()) {
            CGraphicsItemGroup *pGroup = static_cast<CGraphicsItemGroup *>(p);
            result.append(pGroup->getBzItems(recursiveFind));
        }
    }
    return result;
}

QList<CGraphicsItemGroup *> CGraphicsItemGroup::getGroups(bool recursiveFind) const
{
    QList<CGraphicsItemGroup *> result;
    for (auto p : m_listItems) {
        if (p->isBzGroup()) {
            CGraphicsItemGroup *pGroup = static_cast<CGraphicsItemGroup *>(p);
            result.append(pGroup);

            if (recursiveFind)
                result.append(pGroup->getGroups(recursiveFind));
        }
    }
    return result;
}

void CGraphicsItemGroup::add(CGraphicsItem *item, bool updateAttri, bool updateRect)
{
    if (item == nullptr)
        return;

    //防止添加自己
    if (item == this)
        return;

    if (!m_listItems.contains(item)) {
        if (dynamic_cast<CGraphicsItem *>(item) != nullptr) {
            m_listItems.push_back(item);
            item->setMutiSelect(true);
            item->setSelected(true);
            item->setBzGroup(this);

            if (updateAttri) {
                // 如果是文字图元，需要单独先进行属性验证后再刷新属性，这样才能保证获取到的属性是正确最新的
                if (item->type() == TextType) {
                    dynamic_cast<CGraphicsTextItem *>(item)->updateSelectAllTextProperty();
                }
                updateAttributes();
            }
        }
        _addTp = EOneByOne;
    }

    if (updateRect)
        updateBoundingRect();
}

void CGraphicsItemGroup::remove(CGraphicsItem *item, bool updateAttri, bool updateRect)
{
    //防止删除自己
    if (item == this)
        return;

    if (m_listItems.contains(item)) {
        m_listItems.removeOne(item);
        item->setMutiSelect(false);
        item->setSelected(false);
        item->setBzGroup(nullptr);
        if (updateAttri)
            updateAttributes();
    }
    if (m_listItems.size() == 1) {
        m_listItems.at(0)->setMutiSelect(false);
    }

    if (updateRect)
        updateBoundingRect();
}

void CGraphicsItemGroup::newResizeTo(CSizeHandleRect::EDirection dir,
                                     const QPointF &mousePos,
                                     const QPointF &offset,
                                     bool bShiftPress, bool bAltPress)
{
    if (m_listItems.count() == 1) {
        m_listItems.first()->resizeTo(dir, mousePos, bShiftPress, bAltPress);
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

void CGraphicsItemGroup::rotatAngle(qreal angle)
{
    if (m_listItems.count() == 1) {
        m_listItems.first()->rotatAngle(angle);
        prepareGeometryChange();
        updateBoundingRect();
    }
}

void CGraphicsItemGroup::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    prepareGeometryChange();
    foreach (CGraphicsItem *item, m_listItems) {
        item->resizeTo(dir, point);
    }
}

void CGraphicsItemGroup::move(QPointF beginPoint, QPointF movePoint)
{
    foreach (CGraphicsItem *item, m_listItems) {
        item->move(beginPoint, movePoint);
    }
    updateBoundingRect();
}

int CGraphicsItemGroup::type() const
{
    return MgrType;
}

void CGraphicsItemGroup::operatingBegin(int opTp)
{
    for (CGraphicsItem *pItem : m_listItems) {
        pItem->operatingBegin(opTp);
    }
    CGraphicsItem::operatingBegin(opTp);
}

void CGraphicsItemGroup::operatingEnd(int opTp)
{
    for (CGraphicsItem *pItem : m_listItems) {
        pItem->operatingEnd(opTp);
    }
    CGraphicsItem::operatingEnd(opTp);
}

QRectF CGraphicsItemGroup::rect() const
{
    return _rct;
}

CGraphicsUnit CGraphicsItemGroup::getGraphicsUnit(EDataReason reson) const
{
    if (m_listItems.count() >= 1)
        return m_listItems.first()->getGraphicsUnit(reson);
    return CGraphicsUnit();
}

void CGraphicsItemGroup::setNoContent(bool b, bool children)
{
    setFlag(ItemHasNoContents, b);

    if (children) {
        QList<QGraphicsItem *> chidren =  childItems();
        for (QGraphicsItem *pItem : chidren) {
            pItem->setFlag(ItemHasNoContents, b);
        }
    }
}

bool CGraphicsItemGroup::isNoContent()
{
    return (flags()&ItemHasNoContents);
}

bool CGraphicsItemGroup::containItem(CGraphicsItem *pBzItem)
{
    return (m_listItems.indexOf(pBzItem) != -1);
}

CGraphicsItem::Handles CGraphicsItemGroup::nodes()
{
    return m_handles;
}

void CGraphicsItemGroup::setRecursiveScene(CDrawScene *scene)
{
    for (auto p : m_listItems) {
        if (p->isBzGroup()) {
            CGraphicsItemGroup *pGp = static_cast<CGraphicsItemGroup *>(p);
            pGp->setRecursiveScene(scene);
        }
        p->setScene(scene);
    }
    this->setScene(scene);
}

qreal CGraphicsItemGroup::getMinZ() const
{
    auto p = minZItem();
    if (p != nullptr) {
        return p->zValue();
    }
    return 0;
}

CGraphicsItem *CGraphicsItemGroup::minZItem() const
{
    auto list = getBzItems(true);
    if (list.isEmpty())
        return nullptr;
    return CDrawScene::returnSortZItems(list).last();
}

CGraphicsItem *CGraphicsItemGroup::getLogicFirst() const
{
    CGraphicsItem *pBaseItem = nullptr;
    if (this->addType() == EOneByOne) {
        auto list = items();
        if (!list.isEmpty()) {

            auto pFirst = list.first();
            if (pFirst->isBzGroup())
                pBaseItem = static_cast<CGraphicsItemGroup *>(pFirst)->minZItem();
            else {
                pBaseItem = pFirst;
            }
        }
    } else {
        pBaseItem = minZItem();
    }
    return pBaseItem;
}

void CGraphicsItemGroup::setAddType(CGraphicsItemGroup::EAddType tp)
{
    _addTp = tp;
}

CGraphicsItemGroup::EAddType CGraphicsItemGroup::addType()const
{
    return _addTp;
}

void CGraphicsItemGroup::updateHandlesGeometry()
{
    const QRectF &geom = this->boundingRect();

    //如果大小无效了那么就没有显示的必要了
    if (geom.isValid()) {
        show();
    } else {
        hide();
        return;
    }

    qreal qRoty = 0;
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
                qRoty = geom.y() - h - h / 2;
                hndl->move(geom.x() + geom.width() / 2 - w / 2, qRoty);
            }
            break;
        }
        default:
            break;
        }
    }

//    if (rotateItem != nullptr) {
//        CGraphicsView *pView = CManageViewSigleton::GetInstance()->getCurView();
//        QPoint  posInView  = pView->viewport()->mapFromGlobal(QCursor::pos());
//        QPointF posInScene = pView->mapToScene(posInView);

//        QPointF paintPos = posInScene + QPointF(30, 5);
//        qDebug() << "pppppppppp:" << paintPos;
//        rotateItem->updateRotateAngle(rotation());
//        rotateItem->setPos(paintPos);

//        qreal w = rotateItem->boundingRect().width();
//        qreal h = rotateItem->boundingRect().height();
//        qRoty = geom.y() - h - h / 2;
//        qreal scaleTotal = pView != nullptr ? pView->getDrawParam()->getScale() : 1.0;
//        rotateItem->setPos(geom.x() + (geom.width() - w) / 2, qRoty - 40 / scaleTotal);
//    }

    setHandleVisible(true, CSizeHandleRect::InRect);
    setHandleVisible(m_listItems.count() == 1, CSizeHandleRect::Rotation);
    if (m_listItems.count() == 1) {
        CGraphicsItem *pItem = m_listItems.first();
        if (pItem->type() == TextType) {
            bool visble = dynamic_cast<CGraphicsTextItem *>(pItem)->getManResizeFlag();
            this->setHandleVisible(visble, CSizeHandleRect::LeftTop);
            this->setHandleVisible(visble, CSizeHandleRect::Top);
            this->setHandleVisible(visble, CSizeHandleRect::RightTop);
            this->setHandleVisible(visble, CSizeHandleRect::RightBottom);
            this->setHandleVisible(visble, CSizeHandleRect::Bottom);
            this->setHandleVisible(visble, CSizeHandleRect::LeftBottom);
            this->setHandleVisible(visble, CSizeHandleRect::Rotation);
        } else if (pItem->type() == BlurType) {
            this->setHandleVisible(false, CSizeHandleRect::Rotation);
        }
    }
}

void CGraphicsItemGroup::updateAttributes()
{
    if (groupType() != ESelectGroup)
        return;

    TopToolbar *pBar    = drawApp->topToolbar();

    if (pBar != nullptr) {
        CComAttrWidget *pAttr = pBar->attributWidget();
        if (pAttr != nullptr) {

            EDrawToolMode model = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode();
            if (model == cut) {
                CCutTool *pTool = dynamic_cast<CCutTool *>(CDrawToolManagerSigleton::GetInstance()->getDrawTool(model));
                CGraphicsCutItem *pCutItem = pTool->getCutItem(drawScene());
                pAttr->setGraphicItem(pCutItem);
            } else {
                if (this->count() == 0) {
                    pAttr->setGraphicItem(nullptr);
                    return;
                } else if (this->count() == 1) {
                    pAttr->setGraphicItem(m_listItems.first());
                } else {
                    pAttr->setGraphicItem(this);
                }
            }
        }
    }
}

void CGraphicsItemGroup::setHandleVisible(bool visble, CSizeHandleRect::EDirection dirHandle)
{
    for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it) {
        CSizeHandleRect *hndl = *it;
        if (hndl->dir() == dirHandle || dirHandle == CSizeHandleRect::InRect) {
            hndl->setVisible(visble);

            if (hndl->type() == dirHandle)
                break;
        }
    }
}

void CGraphicsItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (groupType() == ENormalGroup) {
        paintMutBoundingLine(painter, option);
//        painter->drawText(boundingRect(), QString("index:%1 status:%2 child:%3")
//                          .arg(_indexForTest)
//                          .arg(isSelected())
//                          .arg(count()));s
        return;
    }

    bool paintBorder = (groupType() == ENormalGroup && isSelected()) || groupType() == ESelectGroup;
    if (paintBorder) {
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
//    painter->drawText(this->boundingRect(), groupType() == ENormalGroup ?
//                      QString("ENormalGroup") : QString("ESelectGroup"));
}

QVariant CGraphicsItemGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    //return CGraphicsItem::itemChange(change, value);

    if (QGraphicsItem::ItemSceneHasChanged == change) {

        // 删除图元刷新模糊
        //auto curScene = static_cast<CDrawScene *>(scene());

    }

    return value;
}

void CGraphicsItemGroup::initHandle()
{
    clearHandle();

    m_handles.reserve(CSizeHandleRect::None);

    if (groupType() == ESelectGroup) {
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
    }

    updateBoundingRect();

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}
