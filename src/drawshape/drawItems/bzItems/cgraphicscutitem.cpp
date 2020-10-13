/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "cgraphicscutitem.h"
#include "csizehandlerect.h"
//#include "frame/ccutwidget.h"
#include "globaldefine.h"
#include "cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QPushButton>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

//const int CORNER_WITH = 20;

CGraphicsCutItem::CGraphicsCutItem(CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    //initHandle();
}

CGraphicsCutItem::CGraphicsCutItem(const QRectF &rect, CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();

    m_originalRect = QRectF(0, 0, 0, 0);
    m_originalRect.setSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutDefaultSize());

    //initHandle();
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutSize(rect.size().toSize());
}

CGraphicsCutItem::CGraphicsCutItem(qreal x, qreal y, qreal w, qreal h, CGraphicsItem *parent)
    : CGraphicsItem(parent)
    , m_isFreeMode(false)
{
    QRectF rect(x, y, w, h);
    rect = rect.normalized();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    m_originalRect = QRectF(0, 0, 0, 0);
    m_originalRect.setSize(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutDefaultSize());
    //initHandle();
}

CGraphicsCutItem::~CGraphicsCutItem()
{

}

int CGraphicsCutItem::type() const
{
    return CutType;
}

void CGraphicsCutItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_topLeftPoint = rect.topLeft();
    m_bottomRightPoint = rect.bottomRight();
    updateHandlesGeometry();

    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCutSize(rect.size().toSize());
}

void CGraphicsCutItem::initHandle()
{
    clearHandle();
    // 子handles 用于处理重设大小
    m_handles.reserve(CSizeHandleRect::None);
    for (int i = CSizeHandleRect::LeftTop; i <= CSizeHandleRect::Left; ++i) {
        CSizeHandleRect *shr = nullptr;
        shr = new CSizeHandleRect(this, static_cast<CSizeHandleRect::EDirection>(i));
        shr->setJustExitLogicAbility(true);
        m_handles.push_back(shr);
    }
    updateHandlesGeometry();
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

void CGraphicsCutItem::updateHandlesGeometry()
{
    const QRectF &geom = this->rect();

    const Handles::iterator hend =  m_handles.end();
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
}

QRectF CGraphicsCutItem::rect() const
{
    return QRectF(m_topLeftPoint, m_bottomRightPoint);
}

QRectF CGraphicsCutItem::boundingRect() const
{
    if (scene() != nullptr) {
        return scene()->sceneRect();
    }
    return QRectF(0, 0, 0, 0);
}

void CGraphicsCutItem::resizeTo(CSizeHandleRect::EDirection dir, const QPointF &point)
{
    // 被弃用但必须实现的缩放函数
    Q_UNUSED(dir)
    Q_UNUSED(point)
}

void CGraphicsCutItem::move(QPointF beginPoint, QPointF movePoint)
{
    QPointF adjust = movePoint - beginPoint;
    setRect(rect().adjusted(adjust.x(), adjust.y(), adjust.x(), adjust.y()));
}

CSizeHandleRect::EDirection CGraphicsCutItem::hitTest(const QPointF &point) const
{
    const Handles::const_iterator hend =  m_handles.end();
    for (Handles::const_iterator it = m_handles.begin(); it != hend; ++it) {
        if ((*it)->hitTest(point)) {
            return (*it)->dir();
        }
    }
    //检测是否在矩形内
    QPointF pt = mapFromScene(point);
    if (this->rect().contains(pt)) {
        return CSizeHandleRect::InRect;
    }

    return CSizeHandleRect::None;
}

QPainterPath CGraphicsCutItem::getHighLightPath()
{
    return QPainterPath();
}

bool CGraphicsCutItem::isPosPenetrable(const QPointF &posLocal)
{
    Q_UNUSED(posLocal)
    return false;
}

void CGraphicsCutItem::showControlRects(bool flag)
{
    setState(flag ? SelectionHandleActive : SelectionHandleOff);
}

bool CGraphicsCutItem::isFreeMode() const
{
    return m_isFreeMode;
}

void CGraphicsCutItem::setIsFreeMode(bool isFreeMode)
{
    m_isFreeMode = isFreeMode;
    showControlRects(true/*isFreeMode*/);
}

void CGraphicsCutItem::duplicate(CGraphicsItem *item)
{
    Q_UNUSED(item)
}

void CGraphicsCutItem::resizeCutSize(CSizeHandleRect::EDirection dir,
                                     const QPointF &prePoint,
                                     const QPointF &point, QPointF *outAcceptPos)
{
    //得到在自身坐标系内的当前鼠标位置
    QPointF preLocalPos = mapFromScene(prePoint);
    QPointF curLocalPos = mapFromScene(point);
    QRectF  curRect     = rect();
    qreal   qWHRadio    = getWHRadio();

    qreal moveX = (curLocalPos - preLocalPos).x();
    qreal moveY = (curLocalPos - preLocalPos).y();
    qreal adjust[4] = {0, 0, 0, 0};


    bool getBorder = false;

    qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
    qreal minH = (qWHRadio < 1.0 && qWHRadio > 0.0) ? 10 / qWHRadio : 10;

    //根据dir的位置判断应该改变到的大小
    switch (dir) {
    case CSizeHandleRect::Top: {
        adjust[1] = moveY;

        //边界检查
        if (curRect.height() - adjust[1] < minH) {
            adjust[1] = curRect.height() - minH;
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[2] = -adjust[1] * qWHRadio;
        }
        break;
    }

    case CSizeHandleRect::Bottom: {
        adjust[3] = moveY;

        //边界检查
        if (curRect.height() + adjust[3] < minH) {
            adjust[3] = minH - curRect.height();
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[2] = adjust[3] * qWHRadio;
        }
        break;
    }
    case CSizeHandleRect::Left: {
        adjust[0] = moveX;

        //边界检查
        if (curRect.width() - adjust[0] < minW) {
            adjust[0] = curRect.width() - minW;
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[3] = -adjust[0] / qWHRadio;
        }

        break;
    }
    case CSizeHandleRect::Right: {
        adjust[2] = moveX;

        //边界检查
        if (curRect.width() + adjust[2] < minW) {
            adjust[2] = minW - curRect.width();
            getBorder = true;
        }

        if (qWHRadio > 0) {
            adjust[3] = adjust[2] / qWHRadio;
            getBorder = true;
        }
        break;
    }
    case CSizeHandleRect::RightTop: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[2] = moveX;

                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() + adjust[2] < minW) {
                    adjust[2] = minW - curRect.width();
                    getBorder = true;
                }

                adjust[1] = -adjust[2] / qWHRadio;
            } else {
                adjust[1] = moveY;

                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() - adjust[1] < minH) {
                    adjust[1] = curRect.height() - minH;
                    getBorder = true;
                }

                adjust[2] = -adjust[1] * qWHRadio;
            }
        } else {

            adjust[2] = moveX;
            adjust[1] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() + adjust[2] < minW) {
                adjust[2] = minW - curRect.width();
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
            if (curRect.height() - adjust[1] < minH) {
                adjust[1] = curRect.height() - minH;
                getBorder = true;
            }
        }

        break;
    }
    case CSizeHandleRect::LeftTop: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[0] = moveX;
                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() - adjust[0] < minW) {
                    adjust[0] = curRect.width() - minW;
                    getBorder = true;
                }
                adjust[1] = adjust[0] / qWHRadio;
            } else {
                adjust[1] = moveY;
                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() - adjust[1] < minH) {
                    adjust[1] = curRect.height() - minH;
                    getBorder = true;
                }
                adjust[0] = adjust[1] * qWHRadio;
            }
        } else {
            adjust[0] = moveX;
            adjust[1] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() - adjust[0] < minW) {
                adjust[0] = curRect.width() - minW;
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
            if (curRect.height() - adjust[1] < minH) {
                adjust[1] = curRect.height() - minH;
                getBorder = true;
            }
        }
        break;
    }

    case CSizeHandleRect::RightBottom: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[2] = moveX;

                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() + adjust[2] < minW) {
                    adjust[2] = minW - curRect.width();
                    getBorder = true;
                }

                adjust[3] = adjust[2] / qWHRadio;
            } else {
                adjust[3] = moveY;

                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() + adjust[3] < minH) {
                    adjust[3] = minH - curRect.height();
                    getBorder = true;
                }

                adjust[2] = adjust[3] * qWHRadio;
            }
        } else {
            adjust[2] = moveX;
            adjust[3] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() + adjust[2] < minW) {
                adjust[2] = minW - curRect.width();
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
            if (curRect.height() + adjust[3] < minH) {
                adjust[3] = minH - curRect.height();
                getBorder = true;
            }
        }

        break;
    }

    case CSizeHandleRect::LeftBottom: {
        if (qWHRadio > 0) {
            if (qAbs(moveX) > qAbs(moveY)) {
                adjust[0] = moveX;

                //边界检查
                //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
                if (curRect.width() - adjust[0] < minW) {
                    adjust[0] = curRect.width() - minW;
                    getBorder = true;
                }

                adjust[3] = -adjust[0] / qWHRadio;
            } else {
                adjust[3] = moveY;

                //边界检查
                //qreal minH = qWHRadio < 1.0 ? 10 / qWHRadio : 10;
                if (curRect.height() + adjust[3] < minH) {
                    adjust[3] = minH - curRect.height();
                    getBorder = true;
                }

                adjust[0] = -adjust[3] * qWHRadio;
            }
        } else {
            adjust[0] = moveX;
            adjust[3] = moveY;

            //边界检查
            //qreal minW = qWHRadio > 1.0 ? 10 * qWHRadio : 10;
            if (curRect.width() - adjust[0] < minW) {
                adjust[0] = curRect.width() - minW;
                getBorder = true;
            }

            //qreal minH = qWHRadio < 1.0 ? 10 * qWHRadio : 10;
            if (curRect.height() + adjust[3] < minH) {
                adjust[3] = minH - curRect.height();
                getBorder = true;
            }

        }
        break;
    }
    default:
        break;
    }

    if (outAcceptPos != nullptr) {
        if (getBorder) {
            *outAcceptPos = (prePoint + QPointF(adjust[0] + adjust[2], adjust[1] + adjust[3]));
        } else {
            *outAcceptPos = point;
        }
    }

    curRect.adjust(adjust[0], adjust[1], adjust[2], adjust[3]);

    prepareGeometryChange();

    this->setRect(curRect);

    updateHandlesGeometry();
}

qreal CGraphicsCutItem::getWHRadio()
{
    qreal   qwhRadio    = -1;
    CGraphicsView *pView = curView();
    if (pView != nullptr) {
        if (!isFreeMode()) {
            ECutType cutTp = pView->getDrawParam()->getCutType();
            switch (cutTp) {
            case cut_1_1:
                qwhRadio = 1.0;
                break;
            case cut_2_3:
                qwhRadio = 2.0 / 3.0;
                break;
            case cut_8_5:
                qwhRadio = 8.0 / 5.0;
                break;
            case cut_16_9:
                qwhRadio = 16.0 / 9.0;
                break;
            case cut_original:
                qwhRadio = scene()->sceneRect().width() / scene()->sceneRect().height();
                break;
            default:
                break;
            }
        }
    }
    return qwhRadio;
}

CGraphicsItem::Handles CGraphicsCutItem::nodes()
{
    return m_handles;
}

void CGraphicsCutItem::doChangeType(int type)
{
    m_cutType = type;

    QRectF rect = this->scene()->sceneRect();
    QPointF centerPos = rect.center();
    qreal w = rect.width();
    qreal h = rect.height();
    QPointF topLeft;
    QPointF rightBottom;
    qreal bigW = w;
    qreal bigH = h;

    if (type == cut_original) {
//        bigW = m_originalRect.width();
//        bigH = m_originalRect.height();
        bigW = rect.width();
        bigH = rect.height();
    } else {
        switch (type) {
        case cut_1_1:
            bigH = bigW;
            break;
        case cut_2_3:
            bigH = bigW * 3 / 2;
            break;
        case cut_8_5:
            bigH = bigW * 5 / 8;
            break;
        case cut_16_9:
            bigH = bigW * 9 / 16;
            break;
        default:
            break;

        }


        qreal scale = (bigW / bigH);


        if (bigW > w || bigH > h) {
            if (scale >= (w / h)) {
                bigW = w;
                bigH = (bigW / scale);
            } else {
                bigH = h;
                bigW = (bigH * scale);
            }

        }

    }

    topLeft = centerPos - QPointF(bigW / 2, bigH / 2);
    rightBottom = centerPos + QPointF(bigW / 2, bigH / 2);
    this->setRect(QRectF(topLeft, rightBottom));

    setIsFreeMode(false);
}

int CGraphicsCutItem::getCutType() const
{
    return m_cutType;
}

void CGraphicsCutItem::doChangeSize(int w, int h)
{
    //    if (w > m_originalRect.width() || h > m_originalRect.height()) {
    //        return;
    //    }
    setRect(QRectF(rect().x(), rect().y(), w, h));
}

QVariant CGraphicsCutItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    Q_UNUSED(change);
    if (scene() != nullptr && !scene()->views().isEmpty()) {
        scene()->views().first()->viewport()->update();
    }

    if (QGraphicsItem::ItemSceneHasChanged == change) {
        QGraphicsScene *pScene = qvariant_cast<QGraphicsScene *>(value);
        if (pScene == nullptr) {
            clearHandle();
        } else {
            initHandle();
        }
    }
    return value;
}

void CGraphicsCutItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    updateHandlesGeometry();

    QColor penColor = QColor("#ffffff");
    penColor.setAlpha(qRound(255 * 0.7));
    //    int themValue = CManageViewSigleton::GetInstance()->getThemeType();
    //    if (themValue == 1) {
    //        //浅色主题
    //        penColor = QColor("#979797");
    //    } else if (themValue == 2) {
    //        //深色主题
    //        penColor = QColor("#FFFFFF");
    //    }

    //先绘制一层阴影
    QColor bgColor(0, 0, 0, int(255.0 * 40.0 / 100.0));
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);

    painter->translate(-pos());

    QPainterPath fillPath;
    QRectF itemRect = mapRectToScene(rect());
    QRectF unitRct  = boundingRect();
    fillPath.addRect(unitRct);
    if (unitRct.intersects(itemRect)) {
        fillPath.addRect(itemRect);
    }
    painter->drawPath(fillPath);

    painter->restore();

    qreal penWidth = 1.0 / painter->worldTransform().m11();
    QRectF rct = rect().adjusted(penWidth, penWidth, -penWidth, -penWidth);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(penColor);
    pen.setWidthF(penWidth);
    painter->setPen(pen);

    //画三等分矩形的直线
    drawTrisectorRect(painter);

    //画矩形
    painter->save();
    QPen rectPen(pen);
    rectPen.setStyle(Qt::DashLine);
    painter->setPen(rectPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rct);
    painter->restore();

    drawFourConner(painter);
}

void CGraphicsCutItem::drawTrisectorRect(QPainter *painter)
{
    qreal penWidth = 1.0 / painter->worldTransform().m11();
    QPainterPath path;
    QRectF rct = rect().adjusted(penWidth, penWidth, -penWidth, -penWidth);
    path.moveTo(rct.x(), rct.y() + rct.height() / 3);
    path.lineTo(rct.x() + rct.width(), rct.y() + rct.height() / 3);

    path.moveTo(rct.x(), rct.y() + rct.height() / 3 * 2);
    path.lineTo(rct.x() + rct.width(), rct.y() + rct.height() / 3 * 2);

    path.moveTo(rct.x() + rct.width() / 3, rct.y());
    path.lineTo(rct.x() + rct.width() / 3, rct.y() + rct.height());

    path.moveTo(rct.x() + rct.width() / 3 * 2, rct.y());
    path.lineTo(rct.x() + rct.width() / 3 * 2, rct.y() + rct.height());

    painter->drawPath(path);
}

void CGraphicsCutItem::drawFourConner(QPainter *painter/*, QPainterPath &path, const int penWidth*/)
{
    qreal penWidth = 1.0 / painter->worldTransform().m11();
    QRectF rct = rect().adjusted(penWidth, penWidth, -penWidth, -penWidth);
    //QRectF rct = boundingRect();
    bool isMinSize = (qFuzzyIsNull(rect().width() - 10.0) && qFuzzyIsNull(rect().height() - 10.0));
    if (isMinSize) {
        QPen pen(painter->pen());
        pen.setWidthF(1.0);
        painter->setPen(pen);
        pen.setStyle(Qt::SolidLine);
        painter->setBrush(Qt::gray);
        painter->drawRect(rct);

        QLineF topLine   = QLineF(rct.topLeft(), rct.topRight());
        QLineF botLine   = QLineF(rct.bottomLeft(), rct.bottomRight());
        QLineF leftLine  = QLineF(rct.topLeft(), rct.bottomLeft());
        QLineF rightLine = QLineF(rct.topRight(), rct.bottomRight());

        painter->setPen(Qt::gray);
        painter->drawLine(topLine.p1(), topLine.center());
        painter->drawLine(rightLine.p1(), rightLine.center());
        painter->drawLine(botLine.center(), botLine.p2());
        painter->drawLine(leftLine.center(), leftLine.p2());

        painter->setPen(Qt::black);
        painter->drawLine(topLine.center(), topLine.p2());
        painter->drawLine(rightLine.center(), rightLine.p2());
        painter->drawLine(botLine.center(), botLine.p1());
        painter->drawLine(leftLine.center(), leftLine.p1());
    } else {
        QPainterPath path;
        QPen pen(painter->pen());
        pen.setStyle(Qt::SolidLine);
        pen.setWidthF(2.0 / painter->worldTransform().m11());
        pen.setColor(Qt::white);
        painter->setPen(pen);
        qreal penWidth = 0;
        const qreal sameLen = 6.0 / painter->worldTransform().m11();
        qreal CORNER_W = sameLen;
        qreal CORNER_H = sameLen;

        if (rct.width() < 2 * CORNER_W) {
            CORNER_W = rct.width() / 2.0;
        }
        if (rct.height() < 2 * CORNER_H) {
            CORNER_H = rct.height() / 2.0;
        }
        qreal CORNER_WITH = qMin(CORNER_W, CORNER_H);

        //左上角
        path.moveTo(rct.x() + penWidth / 2, rct.y() + CORNER_WITH);
        path.lineTo(rct.x() + penWidth / 2, rct.y()  + penWidth / 2);
        path.lineTo(rct.x() + CORNER_WITH, rct.y()  + penWidth / 2);
        //右上角
        path.moveTo(rct.x() + rct.width() - CORNER_WITH, rct.y() + penWidth / 2);
        path.lineTo(rct.x() + rct.width() - penWidth / 2, rct.y()  + penWidth / 2);
        path.lineTo(rct.x() + rct.width() - penWidth / 2, rct.y() + CORNER_WITH);
        //右下角
        path.moveTo(rct.x() + rct.width() - penWidth / 2, rct.y() + rct.height() - CORNER_WITH);
        path.lineTo(rct.x() + rct.width() - penWidth / 2, rct.y()  + rct.height() - penWidth / 2);
        path.lineTo(rct.x() + rct.width() - CORNER_WITH, rct.y() + rct.height() - penWidth / 2);

        //左下角
        path.moveTo(rct.x() + CORNER_WITH + penWidth / 2, rct.y() + rct.height() - penWidth / 2);
        path.lineTo(rct.x() + penWidth / 2, rct.y()  + rct.height() - penWidth / 2);
        path.lineTo(rct.x() + penWidth / 2, rct.y() + rct.height() - CORNER_WITH);

        painter->drawPath(path);
    }
}
