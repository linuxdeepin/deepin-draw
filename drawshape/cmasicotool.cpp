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
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

CMasicoTool::CMasicoTool()
    : IDrawTool(blur)
    , m_pBlurItem(nullptr)
{

}

CMasicoTool::~CMasicoTool()
{

}

void CMasicoTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->drawView()->setPaintEnable(false);

        scene->clearSelection();
        m_sPointPress = event->scenePos();

        m_pBlurItem = new CGraphicsMasicoItem(m_sPointPress);
        QPen pen;
        pen.setWidth(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth());
        QColor color(255, 255, 255, 0);
        pen.setColor(color);
        m_pBlurItem->setPen(pen);
        m_pBlurItem->setBrush(Qt::NoBrush);
        m_pBlurItem->setZValue(scene->getMaxZValue() + 1);
        scene->setMaxZValue(scene->getMaxZValue() + 1);
        scene->addItem(m_pBlurItem);

        //m_pBlurItem->updateBlurPixmap(true);

        m_bMousePress = true;
    } else {
        scene->mouseEvent(event);
    }
}

void CMasicoTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    Q_UNUSED(scene)
    if (m_bMousePress && nullptr != m_pBlurItem) {
        QPointF pointMouse = event->scenePos();
        bool shiftKeyPress = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getShiftKeyStatus();
        m_pBlurItem->updatePenPath(pointMouse, shiftKeyPress);
        m_pBlurItem->updateBlurPath();
        updateRealTimePixmap(scene);
    }
}

void CMasicoTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        scene->drawView()->setPaintEnable(true);
        m_pBlurItem->setPixmap();
        m_sPointRelease = event->scenePos();

        //如果鼠标没有移动
        if (m_pBlurItem != nullptr) {
            if (event->scenePos() == m_sPointPress) {
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
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentDrawToolMode(selection);
    emit scene->signalChangeToSelect();
}

void CMasicoTool::drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene)
{
    Q_UNUSED(rect)
    Q_UNUSED(scene)
    painter->setClipPath(m_clippPath);
    painter->drawPixmap(QPoint(0, 0), m_tempBulrPix);
}

void CMasicoTool::updateRealTimePixmap(CDrawScene *scene)
{
    QPixmap tmpPixmap = scene->drawView()->cachPixMap();
    //计算交叉矩形的区域

    m_clippPath = scene->drawView()->mapFromScene(m_pBlurItem->mapToScene(m_pBlurItem->blurPath()));
    //painter->setClipPath(, Qt::IntersectClip);
    //判断和他交叉的元素，裁剪出下层的像素
    //下层有图元才显示
    int imgWidth = tmpPixmap.width();
    int imgHeigth = tmpPixmap.height();
    int radius = 10;
    if (!tmpPixmap.isNull()) {
        tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (m_pBlurItem->getBlurEffect() == BlurEffect) {
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth);
        }
    }
    m_tempBulrPix = tmpPixmap;
}
