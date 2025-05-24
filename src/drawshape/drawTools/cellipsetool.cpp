// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cellipsetool.h"
#include "cdrawscene.h"
#include "cgraphicsellipseitem.h"
#include "cattributeitemwidget.h"

#include "frame/cgraphicsview.h"
#include "application.h"

#include <QtMath>
#include <DToolButton>
#include <QDebug>

CEllipseTool::CEllipseTool()
    : IDrawTool(ellipse)
{
    qDebug() << "Creating ellipse tool";
}

CEllipseTool::~CEllipseTool()
{
    qDebug() << "Destroying ellipse tool";
}

DrawAttribution::SAttrisList CEllipseTool::attributions()
{
    qDebug() << "Getting ellipse tool attributions";
    DrawAttribution::SAttrisList result;
    result << defaultAttriVar(DrawAttribution::EBrushColor)
           << defaultAttriVar(DrawAttribution::EPenColor)
           << defaultAttriVar(DrawAttribution::EBorderWidth);
    return result;
}

QAbstractButton *CEllipseTool::initToolButton()
{
    qDebug() << "Initializing ellipse tool button";
    DToolButton *m_roundBtn = new DToolButton;
    m_roundBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_O)));
    setWgtAccesibleName(m_roundBtn, "Ellipse tool button");
    m_roundBtn->setToolTip(tr("Ellipse (O)"));
    m_roundBtn->setIconSize(QSize(48, 48));
    m_roundBtn->setFixedSize(QSize(37, 37));
    m_roundBtn->setCheckable(true);
    connect(m_roundBtn, &DToolButton::toggled, m_roundBtn, [ = ](bool b) {
        qDebug() << "Ellipse tool button toggled:" << b;
        QIcon icon       = QIcon::fromTheme("ddc_round tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_round tool_active");
        m_roundBtn->setIcon(b ? activeIcon : icon);
    });
    m_roundBtn->setIcon(QIcon::fromTheme("ddc_round tool_normal"));
    return m_roundBtn;
}

QCursor CEllipseTool::cursor() const
{
    static QPixmap s_cur = QPixmap(":/cursorIcons/round_mouse.svg");
    return QCursor(s_cur);
}

void CEllipseTool::toolCreatItemUpdate(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsEllipseItem *pItem = dynamic_cast<CGraphicsEllipseItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            QPointF pointMouse = event->pos();
            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
            bool altKeyPress = event->keyboardModifiers() & Qt::AltModifier;
            QRectF resultRect;

            qDebug() << "Updating ellipse item - Mouse pos:" << pointMouse 
                     << "Shift:" << shiftKeyPress 
                     << "Alt:" << altKeyPress;

            //按下SHIFT键
            if (shiftKeyPress && !altKeyPress) {
                qDebug() << "Shift key pressed - creating square ellipse";
                QPointF resultPoint = pointMouse;
                qreal w = resultPoint.x() - pInfo->_startPos.x();
                qreal h = resultPoint.y() - pInfo->_startPos.y();
                qreal abslength = abs(w) - abs(h);
                if (abslength >= 0.1) {
                    if (h >= 0) {
                        resultPoint.setY(pInfo->_startPos.y() + abs(w));
                    } else {
                        resultPoint.setY(pInfo->_startPos.y() - abs(w));
                    }
                } else {
                    if (w >= 0) {
                        resultPoint.setX(pInfo->_startPos.x() + abs(h));
                    } else {
                        resultPoint.setX(pInfo->_startPos.x() - abs(h));
                    }
                }
                QRectF rectF(pInfo->_startPos, resultPoint);
                resultRect = rectF.normalized();
            }
            //按下ALT键
            else if (!shiftKeyPress && altKeyPress) {
                qDebug() << "Alt key pressed - creating centered ellipse";
                QPointF point1 = pointMouse;
                QPointF centerPoint = pInfo->_startPos;
                QPointF point2 = 2 * centerPoint - point1;
                QRectF rectF(point1, point2);
                resultRect = rectF.normalized();
            }
            //ALT SHIFT都按下
            else if (shiftKeyPress && altKeyPress) {
                qDebug() << "Shift and Alt keys pressed - creating centered square ellipse";
                QPointF resultPoint = pointMouse;
                qreal w = resultPoint.x() - pInfo->_startPos.x();
                qreal h = resultPoint.y() - pInfo->_startPos.y();
                qreal abslength = abs(w) - abs(h);
                if (abslength >= 0.1) {
                    if (h >= 0) {
                        resultPoint.setY(pInfo->_startPos.y() + abs(w));
                    } else {
                        resultPoint.setY(pInfo->_startPos.y() - abs(w));
                    }
                } else {
                    if (w >= 0) {
                        resultPoint.setX(pInfo->_startPos.x() + abs(h));
                    } else {
                        resultPoint.setX(pInfo->_startPos.x() - abs(h));
                    }
                }

                QPointF point1 = resultPoint;
                QPointF centerPoint = pInfo->_startPos;
                QPointF point2 = 2 * centerPoint - point1;
                QRectF rectF(point1, point2);
                resultRect = rectF.normalized();
            }
            //都没按下
            else {
                qDebug() << "No modifier keys - creating normal ellipse";
                QPointF resultPoint = pointMouse;
                QRectF rectF(pInfo->_startPos, resultPoint);
                resultRect = rectF.normalized();
            }

            qDebug() << "Setting ellipse rect:" << resultRect;
            pItem->setRect(resultRect);
        }
    }
}

void CEllipseTool::toolCreatItemFinish(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsEllipseItem *m_pItem = dynamic_cast<CGraphicsEllipseItem *>(pInfo->businessItem);
        if (nullptr != m_pItem) {
            if (!pInfo->hasMoved()) {
                qDebug() << "Item not moved, removing from scene";
                event->scene()->removeCItem(m_pItem, true);
                pInfo->businessItem = nullptr;
            } else {
                qDebug() << "Item moved, finalizing ellipse creation";
                if (m_pItem->scene() == nullptr) {
                    m_pItem->drawScene()->addCItem(m_pItem);
                }
                m_pItem->setSelected(true);
            }
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CEllipseTool::creatItem(CDrawToolEvent *eventpInfo, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    if ((eventpInfo->eventType() == CDrawToolEvent::EMouseEvent && eventpInfo->mouseButtons() == Qt::LeftButton)
            || eventpInfo->eventType() == CDrawToolEvent::ETouchEvent) {

        qDebug() << "Creating new ellipse item at position:" << eventpInfo->pos();
        CGraphicsEllipseItem *m_pItem =  new CGraphicsEllipseItem(eventpInfo->pos().x(), eventpInfo->pos().y(), 0, 0);
        eventpInfo->scene()->addCItem(m_pItem);
        return m_pItem;
    }
    qDebug() << "No valid event for ellipse creation";
    return nullptr;
}


