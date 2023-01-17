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
#include "triangletool.h"
#include "pagescene.h"
#include "triangleitem.h"
#include "pageview.h"

#include <QToolButton>
#include <QtMath>

TriangleTool::TriangleTool(QObject *parent)
    : DrawItemTool(parent)
{
    QPixmap s_cur = QPixmap(":/cursorIcons/triangle_mouse.svg");

    setCursor(QCursor(s_cur));

    auto m_triangleBtn = toolButton();
    m_triangleBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_S)));
    setWgtAccesibleName(m_triangleBtn, "Triangle tool button");
    m_triangleBtn->setToolTip(tr("Triangle (S)"));
    m_triangleBtn->setIconSize(QSize(48, 48));
    m_triangleBtn->setFixedSize(QSize(37, 37));
    m_triangleBtn->setCheckable(true);

    connect(m_triangleBtn, &QToolButton::toggled, m_triangleBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_triangle tool_normal", QIcon(RCC_DRAWBASEPATH + "ddc_triangle tool_normal_48px.svg"));
        QIcon activeIcon = QIcon::fromTheme("ddc_triangle tool_active", QIcon(RCC_DRAWBASEPATH + "ddc_triangle tool_active_48px.svg"));
        m_triangleBtn->setIcon(b ? activeIcon : icon);
    });
    m_triangleBtn->setIcon(QIcon::fromTheme("ddc_triangle tool_normal", QIcon(RCC_DRAWBASEPATH + "ddc_triangle tool_normal_48px.svg")));
}

TriangleTool::~TriangleTool()
{

}

int TriangleTool::toolType() const
{
    return triangle;
}

SAttrisList TriangleTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EBrushColor)
           << defaultAttriVar(EPenColor)
           << defaultAttriVar(EBorderWidth);
    return result;
}

void TriangleTool::drawItemUpdate(ToolSceneEvent *event, PageItem *p)
{
    TriangleItem *pItem = dynamic_cast<TriangleItem *>(p);
    if (nullptr != pItem) {
        auto startPos = event->firstEvent()->currentLayerPos();
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

PageItem *TriangleTool::drawItemStart(ToolSceneEvent *event)
{
    if (event->isNormalPressed()) {
        TriangleItem *pItem =  new TriangleItem(event->currentLayerPos().x(), event->currentLayerPos().y(), 0, 0);
        return pItem;
    }
    return nullptr;
}

