/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "ellipsetool.h"
#include "pagescene.h"
#include "ellipseitem.h"
#include "attributewidget.h"

#include "pageview.h"

#include <QtMath>
#include <QToolButton>

EllipseTool::EllipseTool(QObject *parent)
    : DrawItemTool(parent)
{
    QPixmap s_cur = QPixmap(":/cursorIcons/round_mouse.svg");
    setCursor(QCursor(s_cur));

    auto m_roundBtn = toolButton();
    m_roundBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_O)));
    setWgtAccesibleName(m_roundBtn, "Ellipse tool button");
    m_roundBtn->setToolTip(tr("Ellipse (O)"));
    m_roundBtn->setIconSize(QSize(20, 20));
    m_roundBtn->setFixedSize(QSize(37, 37));
    m_roundBtn->setCheckable(true);
//    connect(m_roundBtn, &QToolButton::toggled, m_roundBtn, [ = ](bool b) {
//        QIcon icon       = QIcon::fromTheme("ddc_round tool_normal", QIcon(RCC_DRAWBASEPATH + "ddc_round tool_normal_48px.svg"));
//        QIcon activeIcon = QIcon::fromTheme("ddc_round tool_active", QIcon(RCC_DRAWBASEPATH + "ddc_round tool_active_48px.svg"));
//        m_roundBtn->setIcon(b ? activeIcon : icon);
//    });
//    m_roundBtn->setIcon(QIcon::fromTheme("ddc_round tool_normal", QIcon(RCC_DRAWBASEPATH + "ddc_round tool_normal_48px.svg")));
    m_roundBtn->setIcon(QIcon::fromTheme("circle", QIcon(RCC_DRAWBASEPATH + "circle_20px.svg")));
}

EllipseTool::~EllipseTool()
{

}

int EllipseTool::toolType() const
{
    return ellipse;
}

SAttrisList EllipseTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EBrushColor)
           << defaultAttriVar(EPenColor)
           << defaultAttriVar(EPenWidth)
           << SAttri(EStyleProper, QVariant());
    return result;
}

PageItem *EllipseTool::drawItemStart(ToolSceneEvent *eventpInfo)
{
    if (eventpInfo->isNormalPressed()) {
        EllipseItem *item =  new EllipseItem(eventpInfo->currentLayerPos().x(), eventpInfo->currentLayerPos().y(), 0, 0);
        return item;
    }
    return nullptr;
}

void EllipseTool::drawItemUpdate(ToolSceneEvent *event, PageItem *p)
{
    EllipseItem *pItem = dynamic_cast<EllipseItem *>(p);
    QPointF startPos = event->firstEvent()->currentLayerPos();
    if (nullptr != pItem) {
        QPointF pointMouse = event->currentLayerPos();
        bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
        bool altKeyPress = event->keyboardModifiers() & Qt::AltModifier;
        QRectF resultRect;
        //按下SHIFT键
        if (shiftKeyPress && !altKeyPress) {
            QPointF resultPoint = pointMouse;
            qreal w = resultPoint.x() - startPos.x();
            qreal h = resultPoint.y() - startPos.y();
            qreal abslength = abs(w) - abs(h);
            if (abslength >= 0.1) {
                if (h >= 0) {
                    resultPoint.setY(startPos.y() + abs(w));
                } else {
                    resultPoint.setY(startPos.y() - abs(w));
                }
            } else {
                if (w >= 0) {
                    resultPoint.setX(startPos.x() + abs(h));
                } else {
                    resultPoint.setX(startPos.x() - abs(h));
                }
            }
            QRectF rectF(startPos, resultPoint);
            resultRect = rectF.normalized();
        }
        //按下ALT键
        else if (!shiftKeyPress && altKeyPress) {
            QPointF point1 = pointMouse;
            QPointF centerPoint = startPos;
            QPointF point2 = 2 * centerPoint - point1;
            QRectF rectF(point1, point2);
            resultRect = rectF.normalized();
        }
        //ALT SHIFT都按下
        else if (shiftKeyPress && altKeyPress) {
            QPointF resultPoint = pointMouse;
            qreal w = resultPoint.x() - startPos.x();
            qreal h = resultPoint.y() - startPos.y();
            qreal abslength = abs(w) - abs(h);
            if (abslength >= 0.1) {
                if (h >= 0) {
                    resultPoint.setY(startPos.y() + abs(w));
                } else {
                    resultPoint.setY(startPos.y() - abs(w));
                }
            } else {
                if (w >= 0) {
                    resultPoint.setX(startPos.x() + abs(h));
                } else {
                    resultPoint.setX(startPos.x() - abs(h));
                }
            }

            QPointF point1 = resultPoint;
            QPointF centerPoint = startPos;
            QPointF point2 = 2 * centerPoint - point1;
            QRectF rectF(point1, point2);
            resultRect = rectF.normalized();
        }
        //都没按下
        else {
            QPointF resultPoint = pointMouse;
            QRectF rectF(startPos, resultPoint);
            resultRect = rectF.normalized();
        }
        pItem->setRect(resultRect);
    }
}

void EllipseTool::drawItemFinish(ToolSceneEvent *event, PageItem *pItem)
{
    EllipseItem *m_pItem = dynamic_cast<EllipseItem *>(pItem);
    if (nullptr != m_pItem) {
        if (m_pItem->scene() == nullptr) {
            m_pItem->pageScene()->addPageItem(m_pItem);
        }
        m_pItem->setSelected(true);
    }
}




