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
#include "startool.h"
#include "pagescene.h"
#include "staritem.h"
#include "pageview.h"

#include <QToolButton>
#include <QtMath>

StarTool::StarTool(QObject *parent)
    : DrawItemTool(parent)
{
    QPixmap s_cur = QPixmap(":/cursorIcons/star_mouse.svg");
    setCursor(QCursor(s_cur));

    auto m_starBtn = toolButton();
    m_starBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_F)));
    setWgtAccesibleName(m_starBtn, "Star tool button");
    m_starBtn->setToolTip(tr("Star (F)"));
    m_starBtn->setIconSize(QSize(20, 20));
    m_starBtn->setFixedSize(QSize(37, 37));
    m_starBtn->setCheckable(true);
    m_starBtn->setIcon(QIcon::fromTheme("star_normal"));
}

int StarTool::toolType() const
{
    return polygonalStar;
}

StarTool::~StarTool()
{

}

SAttrisList StarTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EBrushColor)
           << defaultAttriVar(EPenColor)
           << defaultAttriVar(EPenWidth)
           << defaultAttriVar(EStartLineSep)
           << defaultAttriVar(EStarAnchor)
           << defaultAttriVar(ERotProperty)
           //<< defaultAttriVar(EGroupWgt)
           //<< defaultAttriVar(EOrderProperty)
           << defaultAttriVar(EStarInnerOuterRadio);
    return result;
}

void StarTool::drawItemUpdate(ToolSceneEvent *event, PageItem *p)
{
    StarItem *pItem = dynamic_cast<StarItem *>(p);
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

PageItem *StarTool::drawItemStart(ToolSceneEvent *event)
{
    if (event->isNormalPressed()) {
        StarItem *m_pItem = new StarItem;
        return m_pItem;
    }
    return nullptr;
}

