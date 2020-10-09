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
#include "cgraphicsitem.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicsproxywidget.h"
#include "cdrawscene.h"
#include "widgets/ctextedit.h"
#include "frame/cgraphicsview.h"
#include "drawItems/cgraphicsitemselectedmgr.h"
#include "application.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QVariant>
#include <QtMath>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "cgraphicscutitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicsmasicoitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicstriangleitem.h"
#include "cpictureitem.h"

const int inccW = 10;
bool CGraphicsItem::paintSelectedBorderLine = true;
QPainterPath CGraphicsItem::getGraphicsItemShapePathByOrg(const QPainterPath &orgPath,
                                                          const QPen &pen,
                                                          bool penStrokerShape,
                                                          const qreal incW, bool doSimplified)
{
    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (orgPath == QPainterPath() || pen == Qt::NoPen)
        return orgPath;
    QPainterPathStroker ps(pen);

    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF() + incW);

    QPainterPath p;

    //获取线条的填充区域路径(线条的宽度会形成一个填充区域，这个区域是实际显示是用的pen颜色的区域)
    if (penStrokerShape) {
        p = ps.createStroke(orgPath);
    } else {
        p = ps.createStroke(orgPath) + orgPath;
    }
    if (doSimplified) {
        p = p.simplified();
    }

    return p;
}

CGraphicsItem *CGraphicsItem::creatItemInstance(int itemType, const CGraphicsUnit &data)
{
    CGraphicsItem *item = nullptr;
    if (RectType == itemType) {
        item = new CGraphicsRectItem;
    } else if (EllipseType == itemType) {
        item = new CGraphicsEllipseItem;
    } else if (TriangleType == itemType) {
        item = new CGraphicsTriangleItem;
    } else if (PolygonType == itemType) {
        item = new CGraphicsPolygonItem;
    } else if (PolygonalStarType == itemType) {
        item = new CGraphicsPolygonalStarItem;
    } else if (LineType == itemType) {
        item = new CGraphicsLineItem;
    } else if (TextType == itemType) {
        item = new CGraphicsTextItem;
    } else if (PictureType == itemType) {
        item = new CPictureItem;
    } else if (PenType == itemType) {
        item = new CGraphicsPenItem;
    } else if (BlurType == itemType) {
        item = new CGraphicsMasicoItem;
    } else if (CutType == itemType) {
        item = new CGraphicsCutItem;
    } else {
        qDebug() << "!!!!!!!!!!!!!!!!!!!!!!unknoewd type !!!!!!!!!!!! = " << itemType;
    }

    item->loadGraphicsUnit(data, true);

    return item;
}


CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_bMutiSelectFlag(false)
{

}

//CGraphicsItem::CGraphicsItem(const SGraphicsUnitHead &head, QGraphicsItem *parent)
//    : QAbstractGraphicsShapeItem(parent)
//    , m_bMutiSelectFlag(false)
//{
//    loadHeadData(head);
//}

CGraphicsView *CGraphicsItem::curView() const
{
    CGraphicsView *parentView = nullptr;
    if (scene() != nullptr) {
        if (!scene()->views().isEmpty()) {
            parentView = dynamic_cast<CGraphicsView *>(scene()->views().first());
        }
    }
    return parentView;
}

CDrawScene *CGraphicsItem::drawScene() const
{
    return qobject_cast<CDrawScene *>(scene());
}

void CGraphicsItem::setPenColor(const QColor &c, bool isPreview)
{
    if (isPreview) {
        m_penPreviewColor = c;
    } else {
        QPen p = pen();
        p.setColor(c);
        setPen(p);
    }
    m_isPreviewCom[0] = isPreview;
    update();
}

void CGraphicsItem::setPenWidth(int w, bool isPreview)
{
    if (isPreview) {
        m_penWidth = w;
    } else {
        QPen p = pen();
        p.setWidth(w);
        p.setJoinStyle(Qt::MiterJoin);
        p.setStyle(Qt::SolidLine);
        p.setCapStyle(Qt::RoundCap);
        setPen(p);
    }
    m_isPreviewCom[1] = isPreview;
    update();
}

void CGraphicsItem::setBrushColor(const QColor &c, bool isPreview)
{
    if (isPreview) {
        m_brPreviewColor = c;
    } else {
        QBrush br = brush();
        br.setStyle(Qt::SolidPattern);
        br.setColor(c);
        setBrush(br);
    }
    m_isPreviewCom[2] = isPreview;
    update();
}

QBrush CGraphicsItem::paintBrush()
{
    QBrush br = brush();
    if (m_isPreviewCom[2]) {
        br.setColor(m_brPreviewColor);
    }
    return br;
}

QPen CGraphicsItem::paintPen()
{
    QPen p = pen();
    if (m_isPreviewCom[0]) {
        p.setColor(m_penPreviewColor);
    }
    if (m_isPreviewCom[1]) {
        p.setWidth(m_penWidth);
    }
    return p;
}

void CGraphicsItem::setMutiSelect(bool flag)
{
    m_bMutiSelectFlag = flag;
}

bool CGraphicsItem::getMutiSelect() const
{
    return (this->isSelected() && scene() != nullptr && (qobject_cast<CDrawScene *>(scene()))->getItemsMgr()->count() > 1);
}

QPainterPath CGraphicsItem::getHighLightPath()
{
    return selfOrgShape();
}

QRectF CGraphicsItem::scenRect()
{
    if (scene() != nullptr) {
        return sceneBoundingRect().translated(-scene()->sceneRect().topLeft());
    }
    return sceneBoundingRect();
}

void CGraphicsItem::loadHeadData(const SGraphicsUnitHead &head)
{
    this->setPen(head.pen);
    this->setBrush(head.brush);

    this->setRotation(head.rotate);
    this->setPos(head.pos);
    this->setZValue(head.zValue);
}

void CGraphicsItem::updateHandlesGeometry()
{
}

int CGraphicsItem::type() const
{
    return Type;
}

bool CGraphicsItem::isBzItem()
{
    return (this->type() >= RectType && this->type() <= BlurType);
}

bool CGraphicsItem::isSizeHandleExisted()
{
    return !m_handles.isEmpty();
}

CSizeHandleRect::EDirection CGraphicsItem::hitTest(const QPointF &point) const
{
    const Handles::const_iterator hend =  m_handles.end();
    for (Handles::const_iterator it = m_handles.begin(); it != hend; ++it) {
        if ((*it)->hitTest(point)) {
            return (*it)->dir();
        }
    }
    //检测是否在矩形内
    QPointF pt = mapFromScene(point);
    if (this->shape().contains(pt)) {
        return CSizeHandleRect::InRect;
    }

    return CSizeHandleRect::None;
}

QPainterPath CGraphicsItem::getSelfOrgShape() const
{
    QPainterPath path;
    path.addRect(rect());
    path.closeSubpath();
    return path;
}

QPainterPath CGraphicsItem::getPenStrokerShape() const
{
    return getGraphicsItemShapePathByOrg(selfOrgShape(), pen(), true, this->incLength());
}

QPainterPath CGraphicsItem::getShape() const
{
    return getGraphicsItemShapePathByOrg(selfOrgShape(), pen(), false, this->incLength());
}

QRectF CGraphicsItem::boundingRect() const
{
    return m_boundingRect;
}

QPainterPath CGraphicsItem::shape() const
{
    return m_boundingShape;
}

QPainterPath CGraphicsItem::selfOrgShape() const
{
    return m_selfOrgPathShape;
}

QPainterPath CGraphicsItem::penStrokerShape() const
{
    return m_penStroerPathShape;
}

bool CGraphicsItem::contains(const QPointF &point) const
{
    if (penStrokerShape().contains(point)) {
        return true;
    } else {
        qreal inLenth = this->incLength();
        bool isInInSide = selfOrgShape().intersects(QRectF(point - QPointF(inLenth, inLenth),
                                                           point + QPointF(inLenth, inLenth)));
        if (isInInSide)
            return true;
    }
    return false;
}

bool CGraphicsItem::isPosPenetrable(const QPointF &posLocal)
{
    bool result = false;
    bool brushIsTrans = brush().color().alpha() == 0 ? true : false;
    bool penIsTrans = (pen().color().alpha() == 0 || pen().width() == 0);

    if (penStrokerShape().contains(posLocal)) {
        result = penIsTrans;
    } else {
        result = brushIsTrans;
    }
    return result;
}

bool CGraphicsItem::isRectPenetrable(const QRectF &rectLocal)
{
    bool isPenetrable = true;
    QPainterPath shape = this->shape();
    if (shape.contains(rectLocal)) {
        bool brushIsTrans = brush().color().alpha() == 0 ? true : false;
        isPenetrable = brushIsTrans;
    } else if (shape.intersects(rectLocal)) {
        isPenetrable = false;
    }
    return isPenetrable;
}

void CGraphicsItem::newResizeTo(CSizeHandleRect::EDirection dir, const QPointF &mousePos,
                                const QPointF &offset, bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(dir)
    Q_UNUSED(mousePos)
    Q_UNUSED(offset)
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
}

void CGraphicsItem::rotatAngle(qreal angle)
{
    QRectF r = this->boundingRect();
    if (r.isValid()) {
        QPointF center = r.center();

        this->setTransformOriginPoint(center);

        this->setRotation(angle);
    }
}

void CGraphicsItem::resizeToMul(CSizeHandleRect::EDirection dir, const QPointF &offset,
                                const double &xScale, const double &yScale,
                                bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(dir)
    Q_UNUSED(offset)
    Q_UNUSED(xScale)
    Q_UNUSED(yScale)
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
}

int CGraphicsItem::operatingType()
{
    return m_operatingType;
}

void CGraphicsItem::operatingBegin(int opTp)
{
    m_operatingType = opTp;
}

void CGraphicsItem::operatingEnd(int opTp)
{
    Q_UNUSED(opTp)
    m_operatingType = -1;
}

CGraphicsItem *CGraphicsItem::creatSameItem()
{
    CGraphicsItem *pItem = duplicateCreatItem();
    if (pItem != nullptr)
        duplicate(pItem);
    return pItem;
}

void CGraphicsItem::loadGraphicsUnit(const CGraphicsUnit &data, bool allInfo)
{
    Q_UNUSED(allInfo)
    loadHeadData(data.head);
}

CGraphicsItem *CGraphicsItem::duplicateCreatItem()
{
    return nullptr;
}

void CGraphicsItem::duplicate(CGraphicsItem *item)
{
    item->setPos(pos().x(), pos().y());
    item->setPen(pen());
    item->setBrush(brush());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(zValue());
}

qreal CGraphicsItem::incLength()const
{
    qreal scal = drawScene() == nullptr ? 1.0 : drawScene()->drawView()->getScale();
    return inccW / scal;
}

bool CGraphicsItem::isGrabToolEvent()
{
    return false;
}

void CGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(false);
    //return QAbstractGraphicsShapeItem::mousePressEvent(event);
}

void CGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(false);
}

void CGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(false);
}

void CGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(false);
}

CGraphicsUnit CGraphicsItem::getGraphicsUnit(bool allInfo) const
{
    Q_UNUSED(allInfo)
    return CGraphicsUnit();
}

void CGraphicsItem::move(QPointF beginPoint, QPointF movePoint)
{
    QPointF Pos = this->pos();
    this->setPos(Pos + movePoint - beginPoint);
}

void CGraphicsItem::updateShape()
{
    m_selfOrgPathShape   = getSelfOrgShape();
    m_penStroerPathShape = getPenStrokerShape();
    m_boundingShape      = getShape();
    m_boundingRect       = m_boundingShape.controlPointRect();

    if (drawScene() != nullptr)
        drawScene()->getItemsMgr()->updateBoundingRect();
    update();
}

void CGraphicsItem::setSizeHandleRectFlag(CSizeHandleRect::EDirection dir, bool flag)
{
    foreach (CSizeHandleRect *sizeHandleRect, m_handles) {
        if (sizeHandleRect->dir() == dir) {
            sizeHandleRect->setVisible(flag);
            break;
        }
    }
}

void CGraphicsItem::setState(ESelectionHandleState st)
{
    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {

        (*it)->setState(st);
    }
}

void CGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event);
}


QVariant CGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    //    if (change == QGraphicsItem::ItemSelectedHasChanged) {
    //        setState(value.toBool() ? SelectionHandleActive : SelectionHandleOff);
    //    }

    //    if (change == QGraphicsItem::ItemSelectedHasChanged &&  this->type() == TextType && value.toBool() == false) {
    //        static_cast<CGraphicsTextItem *>(this)->getTextEdit()->hide();
    //    }

    //未来做多选操作，需要把刷新功能做到undoredo来统一管理
    //全选的由其它地方处理刷新 否则会出现卡顿
    if (change == QGraphicsItem::ItemPositionHasChanged ||
            change == QGraphicsItem::ItemMatrixChange ||
            change == QGraphicsItem::ItemZValueHasChanged ||
            change == QGraphicsItem::ItemOpacityHasChanged ||
            change == QGraphicsItem::ItemRotationHasChanged ||
            // [BUG:45479] 将图元缩小，模糊附着该图元颜色
            change == QGraphicsItem::ItemTransformOriginPointHasChanged) {
        if (nullptr != scene()) {
            auto curScene = static_cast<CDrawScene *>(scene());
            curScene->updateBlurItem(this);
        }
    }


    // 增加图元刷新模糊
    if (change == QGraphicsItem::ItemSceneChange) {
        auto curScene = qobject_cast<CDrawScene *>(scene());
        if (curScene != nullptr) {
            QMetaObject::invokeMethod(curScene, [ = ]() {
                curScene->updateBlurItem();
            }, Qt::QueuedConnection);
        }
    }

    if (QGraphicsItem::ItemSceneHasChanged == change) {
        if (this->type() >= RectType && this->type() < MgrType) {
            QGraphicsScene *pScene = qvariant_cast<QGraphicsScene *>(value);
            if (pScene == nullptr) {
                clearHandle();
            } else {
                initHandle();
            }
        }

        // 删除图元刷新模糊
        auto curScene = static_cast<CDrawScene *>(scene());
        if (curScene != nullptr) {
            curScene->updateBlurItem();
        }
    }

    return value;
}

void CGraphicsItem::beginCheckIns(QPainter *painter)
{
    if (scene() == nullptr || !rect().isValid())
        return;

    painter->save();
    QRectF sceneRct = scene()->sceneRect();
    QRectF itemRct  = mapToScene(rect()).boundingRect();
    bool hasIntersects = sceneRct.intersects(itemRct);
    if (!hasIntersects) {
        painter->setOpacity(0.2);//透明度设置
    }
    painter->setClipping(hasIntersects);
}

void CGraphicsItem::endCheckIns(QPainter *painter)
{
    if (scene() == nullptr || !rect().isValid())
        return;

//    painter->save();

//    painter->setPen(QColor(255, 0, 0));
//    painter->setBrush(Qt::NoBrush);
//    painter->drawPath(penStrokerShape());

//    painter->setPen(QColor(255, 255, 0));
//    painter->setBrush(Qt::NoBrush);
//    painter->drawPath(shape());

//    painter->restore();

    painter->restore();
}

void CGraphicsItem::clearHandle()
{
    for (CSizeHandleRect *pItem : m_handles) {
        pItem->setParentItem(nullptr);
        if (pItem->scene() != nullptr) {
            pItem->scene()->removeItem(pItem);
        }
    }
    m_handles.clear();
}

void CGraphicsItem::paintMutBoundingLine(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    if (!paintSelectedBorderLine)
        return;

    if (this->isSelected() && scene() != nullptr && drawScene()->getItemsMgr()->count() > 1) {

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
}

void CGraphicsItem::resizeTo(CSizeHandleRect::EDirection dir,
                             const QPointF &point,
                             bool bShiftPress, bool bAltPress)
{
    Q_UNUSED(dir)
    Q_UNUSED(point)
    Q_UNUSED(bShiftPress)
    Q_UNUSED(bAltPress)
}
void CGraphicsItem::initHandle()
{
    clearHandle();
    updateHandlesGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

QVector<CSizeHandleRect *> CGraphicsItem::handleNodes()
{
    return m_handles;
}

CSizeHandleRect *CGraphicsItem::handleNode(CSizeHandleRect::EDirection direction)
{
    for (CSizeHandleRect *pNode : m_handles) {
        if (pNode->dir() == direction) {
            return pNode;
        }
    }
    return nullptr;
}
