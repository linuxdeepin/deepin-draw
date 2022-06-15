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
#include "polygontool.h"
#include "pagescene.h"
#include "polygonitem.h"

#include <QtMath>
#include <QToolButton>

PolygonTool::PolygonTool(QObject *parent)
    : DrawItemTool(parent)
{
    QPixmap s_cur = QPixmap(":/cursorIcons/pengaton_mouse.svg");
    setCursor(QCursor(s_cur));

    auto m_polygonBtn = toolButton();
    m_polygonBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_H)));
    setWgtAccesibleName(m_polygonBtn, "Polygon tool button");
    m_polygonBtn->setToolTip(tr("Polygon (H)"));
    m_polygonBtn->setIconSize(QSize(20, 20));
    m_polygonBtn->setFixedSize(QSize(37, 37));
    m_polygonBtn->setCheckable(true);
    m_polygonBtn->setIcon(QIcon::fromTheme("polyons_normal"));
}

PolygonTool::~PolygonTool()
{

}

int PolygonTool::toolType() const
{
    return polygon;
}
SAttrisList PolygonTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EBrushColor)
           << defaultAttriVar(EPenColor)
           << defaultAttriVar(EPenWidth)
           << defaultAttriVar(EPolygonLineSep)
           << defaultAttriVar(EPolygonSides)
           << defaultAttriVar(ERotProperty);
    return result;
}

PageItem *PolygonTool::drawItemStart(ToolSceneEvent *event)
{
    if (event->isNormalPressed()) {
        return new PolygonItem(defaultAttriVar(EPolygonSides).var.toInt(), event->currentLayerPos().x(), event->currentLayerPos().y(), 0, 0);
    }
    return nullptr;
}

void PolygonTool::drawItemUpdate(ToolSceneEvent *event, PageItem *p)
{
    PolygonItem *pItem = dynamic_cast<PolygonItem *>(p);
    if (nullptr != pItem) {
        QPointF pointMouse = event->currentLayerPos();
        QPointF startPos   = event->firstEvent()->currentLayerPos();
        bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
        bool altKeyPress   = event->keyboardModifiers() & Qt::AltModifier;
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


