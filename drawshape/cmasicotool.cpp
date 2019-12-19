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

#include "cmasicotool.h"
#include "cgraphicsmasicoitem.h"
#include "cdrawscene.h"
#include "cdrawparamsigleton.h"
#include "cdrawtoolmanagersigleton.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>


CMasicoTool::CMasicoTool()
    : IDrawTool (blur)
    , m_pBlurItem(nullptr)
{

}

CMasicoTool::~CMasicoTool()
{

}

void CMasicoTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->clearSelection();
        m_sPointPress = event->scenePos();

        m_pBlurItem = new CGraphicsMasicoItem(m_sPointPress);
        QPen pen;
        pen.setWidth(CDrawParamSigleton::GetInstance()->getBlurWidth());
        QColor color(255, 255, 255, 0);
        pen.setColor(color);
        m_pBlurItem->setPen(pen);
        m_pBlurItem->setBrush(Qt::NoBrush);
        scene->addItem(m_pBlurItem);

        m_pBlurItem->setPixmap();

        m_bMousePress = true;
    } /*else if (event->button() == Qt::RightButton) {
        CDrawParamSigleton::GetInstance()->setCurrentDrawToolMode(selection);
        emit scene->signalChangeToSelect();
    } */else {
        scene->mouseEvent(event);
    }
}

void CMasicoTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress && nullptr != m_pBlurItem) {
        QPointF pointMouse = event->scenePos();
        bool shiftKeyPress = CDrawParamSigleton::GetInstance()->getShiftKeyStatus();
        m_pBlurItem->updatePenPath(pointMouse, shiftKeyPress);
        m_pBlurItem->updateBlurPath();
    }
}

void CMasicoTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_sPointRelease = event->scenePos();

        //如果鼠标没有移动
        if ( m_pBlurItem != nullptr) {
            if ( event->scenePos() == m_sPointPress ) {
                scene->removeItem(m_pBlurItem);
                delete m_pBlurItem;
            } else {
                emit scene->itemAdded(m_pBlurItem);
                m_pBlurItem->setSelected(true);
            }
        }

        m_pBlurItem = nullptr;
        m_bMousePress = false;
    }
}
