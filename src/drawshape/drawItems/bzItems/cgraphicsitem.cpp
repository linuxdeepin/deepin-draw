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
#include "global.h"
#include "cgraphicsitemevent.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QPainterPath>
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

    if (item != nullptr)
        item->loadGraphicsUnit(data);

    return item;
}

CGraphicsItem *CGraphicsItem::zItem(const QList<CGraphicsItem *> &pBzItems, int wantZitemTp)
{
    if (pBzItems.isEmpty())
        return nullptr;
    CGraphicsItem *result = nullptr;

    QList<CGraphicsItem *> allBzItems;

    for (auto p : pBzItems) {
        if (p->isBzGroup()) {
            allBzItems.append(static_cast<CGraphicsItemGroup *>(p)->getBzItems(true));
        } else if (p->isBzItem()) {
            allBzItems.append(p);
            //qDebug() << "zzzzzz = " << p->zValue();
        }
    }

    if (wantZitemTp == -1) {
        result = CDrawScene::returnSortZItems(allBzItems, CDrawScene::EAesSort).first();
    } else if (wantZitemTp == -2) {
        result = CDrawScene::returnSortZItems(allBzItems, CDrawScene::EDesSort).first();
    }
    return result;
}


CGraphicsItem::CGraphicsItem(QGraphicsItem *parent)
    : QAbstractGraphicsShapeItem(parent)
    , m_bMutiSelectFlag(false)
    , flipHorizontal(false) // 水平翻转
    , flipVertical(false)  // 垂直翻转
{
}

void CGraphicsItem::setScene(QGraphicsScene *scene, bool calZ)
{
    CDrawScene *pNewScene = qobject_cast<CDrawScene *>(scene);

    CDrawScene *pScene = this->drawScene();
    if (pScene == pNewScene)
        return;

    if (pScene != nullptr)
        pScene->removeCItem(this);

    if (pNewScene != nullptr) {
        pNewScene->addCItem(this, calZ);
    }
}

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

    // 重新获取缓存数据
    if (isCached() && !isPreview) {
        resetCachePixmap();
    }
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

    // 重新获取缓存数据
    if (isCached() && !isPreview) {
        resetCachePixmap();
    }
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
    return (this->isSelected() && scene() != nullptr && (qobject_cast<CDrawScene *>(scene()))->selectGroup()->count() > 1);
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

    _roteAgnel = head.rotate;
    this->setPos(head.pos);
    this->setZValue(head.zValue);
    blurInfos = head.blurInfos;
    this->setTransform(head.trans);

    updateShape();

    if (isCached()) {
        updateBlurPixmap(true);
        resetCachePixmap();
    }
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

void CGraphicsItem::doFilp(CPictureItem::EFilpDirect dir)
{
    if (dir == EFilpHor) {
        this->flipHorizontal = !this->flipHorizontal;
    } else if (dir == EFilpVer) {
        this->flipVertical = !this->flipVertical;
    }
    update();
}

void CGraphicsItem::setFilpBaseOrg(CPictureItem::EFilpDirect dir, bool b)
{
    if (dir == EFilpHor) {
        if (this->flipHorizontal != b) {
            doFilp(dir);
        }
    } else if (dir == EFilpVer) {
        if (this->flipVertical != b) {
            doFilp(dir);
        }
    }
}

bool CGraphicsItem::isFilped(CPictureItem::EFilpDirect dir)
{
    return (dir == EFilpHor ? this->flipHorizontal : this->flipVertical);
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

QPainterPath CGraphicsItem::getTrulyShape() const
{
    return getGraphicsItemShapePathByOrg(selfOrgShape(), pen(), true, 0, false);
}

QPixmap CGraphicsItem::getCachePixmap(bool onlyOrg)
{
    if (curView() == nullptr)
        return QPixmap();

    QPixmap pix(boundingRectTruly().size().toSize()* curView()->devicePixelRatioF());

    pix.setDevicePixelRatio(curView()->devicePixelRatio());

    pix.fill(QColor(0, 0, 0, 0));

    QPainter painter(&pix);

    painter.translate(-boundingRectTruly().topLeft());

    paintItemSelf(&painter, &_curStyleOption, EPaintForCache);

    if (!onlyOrg) {
        paintAllBlur(&painter);
    }

    return pix;
}

void CGraphicsItem::changeTransCenterTo(const QPointF &newCenter)
{
    static bool first = true;
//    const QPointF oldCenter = first ? newCenter : this->transformOriginPoint();

//    QPointF orgPosDelta = newCenter - oldCenter;

//    qDebug() << "oldCenter = " << oldCenter << "newCenter = " << newCenter << "orgPosDelta = " << orgPosDelta;

//    this->moveBy(orgPosDelta.x(), orgPosDelta.y());

//    setTransformOriginPoint(newCenter);

//2.
//    const QPointF oldCenter = first ? newCenter : this->transformOriginPoint();
//    QPointF newOrgInScene  = this->sceneTransform().map(newCenter);
//    QPointF orgPosdistance = oldCenter - newCenter;
//    QPointF oldOrgInScene  = this->sceneTransform().map(oldCenter) - orgPosdistance;

//    QPointF orgPosDelta = newOrgInScene - oldOrgInScene;

//    this->moveBy(orgPosDelta.x(), orgPosDelta.y());

//    this->setRotation(this->rotation());

//    setTransformOriginPoint(newCenter);

//    QPointF newOrgInScene  = this->sceneTransform().map(boundingRect().center());
//    QPointF orgPosdistance = this->transformOriginPoint() - boundingRect().center();
//    QPointF oldOrgInScene  = this->sceneTransform().map(transformOriginPoint()) - orgPosdistance;

//    QPointF orgPosDelta = newOrgInScene - oldOrgInScene;

//    this->moveBy(orgPosDelta.x(), orgPosDelta.y());

//    this->setRotation(this->rotation());
//    setTransformOriginPoint(boundingRect().center());


    setTransformOriginPoint(newCenter);

    first = false;
}

QRectF CGraphicsItem::boundingRect() const
{
    return m_boundingRect;
}

QRectF CGraphicsItem::boundingRectTruly() const
{
    return m_boundingRectTrue;
}

QPainterPath CGraphicsItem::shape() const
{
    return m_boundingShape;
}

void CGraphicsItem::setCacheEnable(bool enable)
{
    _useCachePixmap = enable;
    if (_useCachePixmap) {
        if (_cachePixmap == nullptr) {
            _cachePixmap = new QPixmap;
            *_cachePixmap = getCachePixmap();
        }

    } else {
        if (_cachePixmap != nullptr) {
            setAutoCache(false);
            delete _cachePixmap;
            _cachePixmap = nullptr;
        }
    }
}

bool CGraphicsItem::isCached()
{
    return (_useCachePixmap && _cachePixmap != nullptr);
}

void CGraphicsItem::setAutoCache(bool autoCache, int autoCacheMs)
{
    _autoCache = autoCache;
    _autoEplMs = autoCacheMs;
    update();
}

bool CGraphicsItem::isAutoCache()
{
    return _autoCache;
}

//QPainterPath CGraphicsItem::shapeTruly() const
//{
//    return m_boundingShapeTrue;
//}

QPainterPath CGraphicsItem::selfOrgShape() const
{
    return m_selfOrgPathShape;
}

QPainterPath CGraphicsItem::penStrokerShape() const
{
    return m_penStroerPathShape;
}

QPointF CGraphicsItem::getCenter(CSizeHandleRect::EDirection dir)
{
    QPointF center;
    CGraphicsItem *pItem = this;
    QRectF rect = pItem->rect();
    switch (dir) {
    case CSizeHandleRect::LeftTop:
        center = rect.bottomRight();
        break;
    case CSizeHandleRect::Top:
        center = QPointF(rect.center().x(), rect.bottom());
        break;
    case CSizeHandleRect::RightTop:
        center = rect.bottomLeft();
        break;
    case CSizeHandleRect::Right:
        center = QPointF(rect.left(), rect.center().y());
        break;
    case CSizeHandleRect::RightBottom:
        center = rect.topLeft();
        break;
    case CSizeHandleRect::Bottom:
        center = QPointF(rect.center().x(), rect.top());
        break;
    case CSizeHandleRect::LeftBottom:
        center = rect.topRight();
        break;
    case CSizeHandleRect::Left:
        center = QPointF(rect.right(), rect.center().y());
        break;
    case CSizeHandleRect::Rotation:
        center = rect.center();
        break;
    default:
        center = rect.center();
        break;
    }
    return center;
}

void CGraphicsItem::doChange(CGraphItemEvent *event)
{
    if (event->eventPhase() == EChangedBegin || event->eventPhase() == EChanged) {
        this->operatingBegin(event);
        if (_beginEvent == nullptr) {
            _beginEvent = new CGraphItemEvent;
            *_beginEvent = *event;

            if (event->type() == CGraphItemEvent::ERot) {
                changeTransCenterTo(event->centerPos());
            }
        }
    }

    if ((event->eventPhase() == EChangedUpdate || event->eventPhase() == EChanged) && isBzItem()) {

        doChangeSelf(event);

        //刷新特效
        if (event->type() == CGraphItemEvent::EScal && !blurInfos.isEmpty()) {
            QTransform trans = event->trans();
            for (SBlurInfo &info : blurInfos) {
                info.blurPath = trans.map(info.blurPath);
            }
            if (!isCached())
                updateBlurPixmap(true);
        }
    }

    //如果变化结束那么要进行刷新
    //1.如果是缓冲图模式,那么要刷新出新的模糊图用于模糊路径绘制
    //2.稳定旋转中心
    //3.如果是缓冲模式,重新生成缓冲图
    if (event->type() == CGraphItemEvent::EScal &&
            (event->eventPhase() == EChangedFinished || event->eventPhase() == EChanged) && isBzItem()) {

        //1.生成新的模糊图
        if (isCached()) {
            updateBlurPixmap(true);
        }

        //2.稳定旋转中心
//        QPointF newOrgInScene  = this->sceneTransform().map(boundingRect().center());
//        QPointF orgPosdistance = this->transformOriginPoint() - boundingRect().center();
//        QPointF oldOrgInScene  = this->sceneTransform().map(transformOriginPoint()) - orgPosdistance;

//        QPointF orgPosDelta = newOrgInScene - oldOrgInScene;

//        this->moveBy(orgPosDelta.x(), orgPosDelta.y());

//        this->setRotation(this->rotation());
//        setTransformOriginPoint(boundingRect().center());

        //3.如果是缓冲模式,重新生成缓冲图
        if (isBzItem()) {
            if (isCached()) {
                *_cachePixmap = getCachePixmap();
                qDebug() << "resize scale finished new cached size ==== " << _cachePixmap->size() << "device radio = " << _cachePixmap->devicePixelRatio();
            }
        }
    }

    if (event->eventPhase() == EChangedFinished || event->eventPhase() == EChanged) {
        this->operatingEnd(event);
        if (_beginEvent != nullptr) {
            delete  _beginEvent;
            _beginEvent = nullptr;
        }
    }

}

void CGraphicsItem::doChangeSelf(CGraphItemEvent *event)
{
    //Q_UNUSED(event)
    switch (event->type()) {
    case CGraphItemEvent::ERot: {
        QPointF center = this->transformOriginPoint();
        QLineF l1 = QLineF(center, event->oldPos());
        QLineF l2 = QLineF(center, event->pos());
        qreal angle = l2.angle() - l1.angle();
        QTransform trans;
        trans.translate(center.x(), center.y());
        trans.rotate(-angle);
        trans.translate(-center.x(), -center.y());
        _roteAgnel += -angle;
        int n = int(_roteAgnel) / 360;
        _roteAgnel = _roteAgnel - n * 360;
        if (_roteAgnel < 0) {
            _roteAgnel += 360;
        }
        setTransform(trans, true);
        break;
    }
    case CGraphItemEvent::EMove: {
        QPointF move = event->_scenePos - event->_oldScenePos;
        //qDebug() << "move ========= " << move << childItems();
        this->moveBy(move.x(), move.y());
        break;
    }
    default:
        break;
    }
}

bool CGraphicsItem::isBzGroup(int *groupTp)
{
    bool result = (this->type() == MgrType);
    if (result && groupTp != nullptr) {
        *groupTp = static_cast<CGraphicsItemGroup *>(this)->groupType();
    }
    return result;
}

CGraphicsItemGroup *CGraphicsItem::bzGroup(bool onlyNormal)
{
    if (onlyNormal) {
        if (CDrawScene::isNormalGroupItem(_pGroup)) {
            return _pGroup;
        }
        return nullptr;
    }
    return _pGroup;
}

CGraphicsItemGroup *CGraphicsItem::bzTopGroup(bool onlyNormal)
{
    auto fPrecondition = [ = ](CGraphicsItemGroup * p) {
        if (!onlyNormal) {return p != nullptr;}
        return CDrawScene::isNormalGroupItem(p);
    };

    CGraphicsItemGroup *pTopGroup = bzGroup();

    if (fPrecondition(pTopGroup)) {
        while (true) {
            CGraphicsItemGroup *nextBzGroup = pTopGroup->bzGroup();
            if (!fPrecondition(nextBzGroup)) {
                break;
            }
            pTopGroup = pTopGroup->bzGroup();
        }
    } else {
        pTopGroup = nullptr;
    }

    return pTopGroup;
}

CGraphicsItem *CGraphicsItem::thisBzProxyItem(bool topleve)
{
    CGraphicsItemGroup *pTg = topleve ? bzTopGroup() : bzGroup();
    if (pTg != nullptr)
        return pTg;

    return this;
}

void CGraphicsItem::setBzGroup(CGraphicsItemGroup *pGroup)
{
    //防止添加自己引起循环
    if (pGroup == this)
        return;

    //判断是否相等可终结循环
    if (pGroup == _pGroup)
        return;

    //先从原来的组合中删除
    if (_pGroup != nullptr) {
        _pGroup->remove(this);
    }
    //再添加到新的组合中
    if (pGroup != nullptr) {
        pGroup->add(this);
    }
    _pGroup = pGroup;
}

qreal CGraphicsItem::drawZValue()
{
    return zValue();
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

void CGraphicsItem::setBzZValue(qreal z)
{
    this->setZValue(z);
}

bool CGraphicsItem::isPosPenetrable(const QPointF &posLocal)
{
    bool result = false;
    bool brushIsTrans = brush().color().alpha() == 0 ? true : false;
    bool penIsTrans = (pen().color().alpha() == 0 || pen().width() == 0) ? true : false;

    if (brushIsTrans && penIsTrans) {
        return false;
    }

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

void CGraphicsItem::rotatAngle(qreal angle)
{
    QRectF r = this->boundingRect();

    if (r.isValid()) {
        QPointF center = r.center();

        this->setTransformOriginPoint(center);

        this->setRotation(angle);
    }
}

int CGraphicsItem::operatingType()
{
    return m_operatingType;
}

void CGraphicsItem::operatingBegin(CGraphItemEvent *event)
{
    m_operatingType = event->toolEventType();
}

void CGraphicsItem::operatingEnd(CGraphItemEvent *event)
{
    Q_UNUSED(event)
    m_operatingType = -1;
}

CGraphicsItem *CGraphicsItem::creatSameItem()
{
    CGraphicsUnit data = getGraphicsUnit(EDuplicate);
    CGraphicsItem *pItem = creatItemInstance(this->type(), data);
    data.release();
    return pItem;
}

void CGraphicsItem::loadGraphicsUnit(const CGraphicsUnit &data)
{
    loadHeadData(data.head);
}

qreal CGraphicsItem::incLength()const
{
    qreal scal = drawScene() == nullptr ? 1.0 : drawScene()->drawView()->getScale();
    return inccW / scal;
}

void CGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    //qDebug() << "_useCachePixmap = " << _useCachePixmap << "_cachePixmap = " << _cachePixmap;
    if (isCached()) {
        _curStyleOption = *option;
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::SmoothPixmapTransform);

        painter->save();

        //初始化翻转信息
        painter->setTransform(getFilpTransform(), true);

        //绘制缓冲图
        painter->drawPixmap(boundingRectTruly().toRect(), *_cachePixmap);

        //绘制正在操作的特效
        paintBlur(painter, curBlur);

        painter->restore();

        paintMutBoundingLine(painter, option);
    } else {
        QTime *time = nullptr;
        if (_autoCache) {
            time = new QTime;
            time->start();
        }

        //绘制图元
        paintItemSelf(painter, option, EPaintForNoCache);

        if (_autoCache) {
            int elp = time->elapsed();
            this->setCacheEnable(elp > _autoEplMs);
            delete time;
        }
    }
}


void CGraphicsItem::paintItemSelf(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                  EPaintReson paintReson)
{
    beginCheckIns(painter);

    painter->save();

    //当是动态绘制(无缓冲绘制)绘制时才实现图片的翻转(缓冲绘制是绘制到Pixmap上,需要获取到原图)
    if (paintReson == EPaintForNoCache) {
        painter->setTransform(getFilpTransform(), true);
    }

    //绘制自身
    paintSelf(painter, option);

    if (paintReson == EPaintForNoCache) {
        //绘制模糊特效
        paintAllBlur(painter);
    }

    painter->restore();

    endCheckIns(painter);

    if (paintReson == EPaintForNoCache)
        paintMutBoundingLine(painter, option);
}

void CGraphicsItem::paintSelf(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
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

CGraphicsUnit CGraphicsItem::getGraphicsUnit(EDataReason reson) const
{
    Q_UNUSED(reson)
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

    m_boundingShapeTrue  = getTrulyShape();
    m_boundingRectTrue   = m_boundingShapeTrue.controlPointRect();

    //resetCachePixmap();

    if (drawScene() != nullptr)
        drawScene()->selectGroup()->updateBoundingRect();
//    update();
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
//    //未来做多选操作，需要把刷新功能做到undoredo来统一管理
//    //全选的由其它地方处理刷新 否则会出现卡顿
//    if (change == QGraphicsItem::ItemPositionHasChanged ||
//            change == QGraphicsItem::ItemMatrixChange ||
//            change == QGraphicsItem::ItemZValueHasChanged ||
//            change == QGraphicsItem::ItemOpacityHasChanged ||
//            change == QGraphicsItem::ItemRotationHasChanged ||
//            // [BUG:45479] 将图元缩小，模糊附着该图元颜色
//            change == QGraphicsItem::ItemTransformOriginPointHasChanged) {
//        if (nullptr != scene()) {
//            //auto curScene = static_cast<CDrawScene *>(scene());
//            //curScene->updateBlurItem(this);
//        }
//    }


//    // 增加图元刷新模糊
//    if (change == QGraphicsItem::ItemSceneChange) {
//        auto curScene = qobject_cast<CDrawScene *>(scene());
//        if (curScene != nullptr) {
//            QMetaObject::invokeMethod(curScene, [ = ]() {
//                //curScene->updateBlurItem();
//            }, Qt::QueuedConnection);
//        }
//    }

//    if (QGraphicsItem::ItemSceneHasChanged == change) {
//        if (this->isBzItem()) {
//            QGraphicsScene *pScene = qvariant_cast<QGraphicsScene *>(value);
//            if (pScene == nullptr) {
//                clearHandle();
//            } else {
//                initHandle();
//            }
//        }

//        // 删除图元刷新模糊
//        auto curScene = static_cast<CDrawScene *>(scene());
//        if (curScene != nullptr) {
//            updateShape();
//            //curScene->updateBlurItem();
//        }
//    }

//    return value;


    if (QGraphicsItem::ItemSceneHasChanged == change) {
        if (this->isBzItem()) {
            QGraphicsScene *pScene = qvariant_cast<QGraphicsScene *>(value);
            if (pScene == nullptr) {
                //1.清除节点
                clearHandle();
            } else {
                //添加到场景中的时候初始化节点
                initHandle();

                if (!blurInfos.isEmpty()) {
                    updateBlurPixmap(true);
                }
            }
        }
    } else if (QGraphicsItem::ItemZValueHasChanged == change) {
        if (bzGroup(true) != nullptr) {
            bzGroup(true)->markZDirty();
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

    if (this->isSelected() && scene() != nullptr && drawScene()->selectGroup()->count() > 1) {

        painter->setClipping(false);
        QPen pen;

        painter->setRenderHint(QPainter::Antialiasing, true);

        pen.setWidthF(1 / option->levelOfDetailFromTransform(painter->worldTransform()));

        pen.setColor(QColor("#E0E0E0"));

        painter->setPen(pen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(this->boundingRectTruly());
        painter->setClipping(true);
    }


    //test
#ifdef QT_DEBUG
    QString testString;
    if (this->isBzItem()) {
        testString += QString("z = %1 ").arg(zValue());
    }
    if (this != drawScene()->selectGroup()) {
        testString += QString("pos = (%1,%2) ").arg(sceneBoundingRect().x()).arg(sceneBoundingRect().y());
    }
    QTextOption txtOption(bzGroup() == nullptr ? (Qt::AlignTop | Qt::AlignLeft) : (Qt::AlignTop | Qt::AlignRight));
    txtOption.setWrapMode(QTextOption::WrapAnywhere);
    painter->drawText(boundingRect(), testString, txtOption);

#endif
}

void CGraphicsItem::blurBegin(const QPointF &pos)
{
    curBlur.clear();

    setCacheEnable(true);

    //生成当前图源下的模糊图像(有发生过变化都要重新生成模糊)
    EBlurEffect wantedEf = this->curView()->getDrawParam()->getBlurEffect();
    if (blurPix[wantedEf].isNull() || blurPix[wantedEf].size() != rect().size()) {
        updateBlurPixmap();
    }
    //blurEfTp = wantedEf;

    curBlur.blurEfTp = wantedEf;

    s_tempblurPath = QPainterPath();
    s_tempblurPath.moveTo(getFilpTransform().map(pos));
}

void CGraphicsItem::blurUpdate(const QPointF &pos)
{
    //1.记录点到路径
    s_tempblurPath.lineTo(getFilpTransform().map(pos));

    QPen p; p.setWidth(curView()->getDrawParam()->getBlurWidth());
    curBlur.blurPath = CGraphicsItem::getGraphicsItemShapePathByOrg(s_tempblurPath, p, true, 0, false);
    update();
}

void CGraphicsItem::blurEnd()
{
    if (curBlur.isValid())
        addBlur(curBlur);
    curBlur.clear();
    resetCachePixmap();
    update();
}

void CGraphicsItem::setDrawRotatin(qreal angle)
{
    _roteAgnel = angle;
    // 进行角度取余数
    int n = int(_roteAgnel) / 360;
    _roteAgnel = _roteAgnel - n * 360;
    if (_roteAgnel < 0) {
        _roteAgnel += 360;
    }
}

void CGraphicsItem::updateShapeRecursion()
{
    updateShape();
    if (bzGroup(false) != nullptr) {
        bzGroup(false)->updateShapeRecursion();
    }
}

void CGraphicsItem::updateBlurPixmap(bool onlyOrg)
{
    QPixmap pix = getCachePixmap(onlyOrg);
    EBlurEffect blurEfTp = this->curView()->getDrawParam()->getBlurEffect();
    blurPix[blurEfTp] = NSBlur::blurPixmap(pix, 10, blurEfTp);
    update();
}

void CGraphicsItem::addBlur(const SBlurInfo &sblurInfo)
{
    blurInfos.append(sblurInfo);
    update();
}

void CGraphicsItem::paintAllBlur(QPainter *painter)
{
    for (auto info : blurInfos) {
        paintBlur(painter, info);
    }
    paintBlur(painter, curBlur);
}

void CGraphicsItem::paintBlur(QPainter *painter, const SBlurInfo &info)
{
    if (!info.isValid())
        return;

    painter->save();
    painter->setClipPath(info.blurPath, Qt::IntersectClip);
    painter->drawPixmap(boundingRect().topLeft(), blurPix[info.blurEfTp]);
    painter->restore();
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

void CGraphicsItem::resetCachePixmap()
{
    if (_useCachePixmap && _cachePixmap != nullptr && m_operatingType == -1) {
        *_cachePixmap = getCachePixmap();
    }
}

QTransform CGraphicsItem::getFilpTransform()
{
    QPointF center = boundingRect().center();
    QTransform trans(this->isFilped(CGraphicsItem::EFilpHor) ? -1 : 1, 0, 0,
                     0, this->isFilped(CGraphicsItem::EFilpVer)  ? -1 : 1, 0,
                     0, 0, 1);

    return (QTransform::fromTranslate(-center.x(), -center.y()) * trans * QTransform::fromTranslate(center.x(), center.y()));
}
