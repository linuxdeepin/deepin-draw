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
#include "clinetool.h"
#include "cdrawscene.h"
#include "cgraphicslineitem.h"
#include "cdrawparamsigleton.h"
#include <QGraphicsSceneMouseEvent>
CLineTool::CLineTool()
    : IDrawTool(line)
    , m_pLineItem(nullptr)
{

}

CLineTool::~CLineTool()
{

}

void CLineTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();
        m_sPointPress = event->scenePos();
        m_pLineItem = new CGraphicsLineItem(m_sPointPress, m_sPointPress);
        m_pLineItem->setPen(CDrawParamSigleton::GetInstance()->getPen());
        m_pLineItem->setBrush(CDrawParamSigleton::GetInstance()->getBrush());
        scene->addItem(m_pLineItem);

        m_bMousePress = true;
    } else if (event->button() == Qt::RightButton) {
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
        emit scene->signalChangeToSelect();
    } else {
        scene->mouseEvent(event);
    }
}

void CLineTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress) {
        QPointF pointMouse = event->scenePos();
        m_pLineItem->resizeTo(CSizeHandleRect::RightBottom, pointMouse);
    }
}

void CLineTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_sPointRelease = event->scenePos();
        //如果鼠标没有移动
        if ( m_pLineItem != nullptr) {
            if ( event->scenePos() == m_sPointPress ) {

                scene->removeItem(m_pLineItem);
                delete m_pLineItem;

            } else {
                emit scene->itemAdded(m_pLineItem);
                m_pLineItem->setSelected(true);
            }
        }

        m_pLineItem = nullptr;
        m_bMousePress = false;
    }
}
