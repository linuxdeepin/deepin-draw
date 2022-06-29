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
#include "linetool.h"
#include "pagescene.h"
#include "lineitem.h"
#include "pagecontext.h"
#include "pageview.h"
#include "attributewidget.h"
#include <QtMath>
#include <QToolButton>

LineTool::LineTool(QObject *parent)
    : DrawItemTool(parent)
{
    QPixmap s_cur = QPixmap(":/cursorIcons/line_mouse.svg");
    setCursor(QCursor(s_cur));

    auto m_lineBtn = toolButton();
    m_lineBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_L)));
    setWgtAccesibleName(m_lineBtn, "Line tool button");
    m_lineBtn->setToolTip(tr("Line (L)"));
    m_lineBtn->setIconSize(TOOL_ICON_RECT);
    m_lineBtn->setFixedSize(TOOL_BUTTON_RECT);
    m_lineBtn->setCheckable(true);
    m_lineBtn->setIcon(QIcon::fromTheme("line_normal"));
    connect(m_lineBtn, &QToolButton::toggled, m_lineBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("line_normal");
        QIcon activeIcon = QIcon::fromTheme("line_highlight");
        m_lineBtn->setIcon(b ? activeIcon : icon);
    });
}

LineTool::~LineTool()
{

}

int LineTool::toolType() const
{
    return line;
}

SAttrisList LineTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EPenColor)
           << defaultAttriVar(EPenWidth)
           <<  SAttri(1775)
           << defaultAttriVar(EEnablePenStyle)
           << defaultAttriVar(EStreakStyle)
           << defaultAttriVar(EStreakBeginStyle)
           << defaultAttriVar(EStreakEndStyle);
    return result;
}

PageItem *LineTool::drawItemStart(ToolSceneEvent *event)
{
    if (event->isNormalPressed()) {
        return new LineItem(event->currentLayerPos(), event->currentLayerPos());
    }
    return nullptr;
}

void LineTool::drawItemUpdate(ToolSceneEvent *event, PageItem *p)
{
    LineItem *pItem = dynamic_cast<LineItem *>(p);
    if (nullptr != pItem) {
        bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
        if (!shiftKeyPress) {
            pItem->setLine(QLineF(event->firstEvent()->currentLayerPos(), event->currentLayerPos()));
        } else {
            QPointF local = pItem->mapFromScene(event->currentLayerPos());
            QPointF p1;
            QPointF p2;

            p1 = pItem->line().p1();
            p2 = local;

            QLineF v = QLineF(p1, p2);
            if (fabs(v.dx()) - fabs(v.dy()) > 0.0001) {
                p2.setY(p1.y());
            } else {
                p2.setX(p1.x());
            }

            p1 = pItem->mapToScene(p1);
            p2 = pItem->mapToScene(p2);
            pItem->setRotation(0);
            pItem->setPos(0, 0);
            pItem->setLine(p1, p2);
        }
    }
}

void LineTool::drawItemFinish(ToolSceneEvent *event, PageItem *p)
{
    LineItem *m_pItem = dynamic_cast<LineItem *>(p);
    if (nullptr != m_pItem) {
        if (m_pItem->scene() == nullptr) {
            m_pItem->pageScene()->addPageItem(m_pItem);
        }
    }
}



