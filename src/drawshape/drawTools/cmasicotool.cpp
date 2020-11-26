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
#include "cgraphicsitemselectedmgr.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QApplication>

CMasicoTool::CMasicoTool()
    : IDrawTool(blur)
{

}

CMasicoTool::~CMasicoTool()
{

}

void CMasicoTool::toolCreatItemUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsMasicoItem *pItem = dynamic_cast<CGraphicsMasicoItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            QPointF pointMouse = event->pos();

            if (pItem->parentItem() != nullptr) {
                pointMouse = pItem->parentItem()->mapFromScene(pointMouse);
            }

            //判断是否是鼠标禁用状态
            changeMouseShape(pInfo->startPosTopBzItem, event->pos());

            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
            pItem->updatePenPath(pointMouse, shiftKeyPress);
            pItem->updateBlurPath();
            pItem->updateMasicPixmap();

            // updateRealTimePixmap(event->scene());

            event->setAccepted(true);
        }
    }
}

void CMasicoTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsMasicoItem *pItem = dynamic_cast<CGraphicsMasicoItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            if (!pInfo->hasMoved()) {
                event->scene()->removeCItem(pItem);
                delete pItem;
                pInfo->businessItem = nullptr;
            } else {
                pItem->drawComplete();
                pItem->updateBlurPath();
                pItem->updateMasicPixmap();
                if (pItem->scene() == nullptr) {
                    pItem->drawScene()->addCItem(pItem);
                }
            }
        }
        event->view()->setCacheEnable(false);
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CMasicoTool::creatItem(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {

        //判断是否是鼠标禁用状态
        changeMouseShape(pInfo->startPosTopBzItem, event->pos());

        //模糊只对位图生效[Q4引入的需求]
        if (pInfo->startPosTopBzItem != nullptr && pInfo->startPosTopBzItem->type() == PictureType) {
            // event->view()->setCacheEnable(true);
            // updateRealTimePixmap(event->scene());

            CGraphicsMasicoItem *pItem = new CGraphicsMasicoItem(event->pos());

            CPictureItem *blurPicture =  dynamic_cast<CPictureItem *>(pInfo->startPosTopBzItem);


            CGraphicsView *pView = event->scene()->drawView();
            QPen pen;
            QColor color(255, 255, 255, 0);
            pen.setColor(color);
            pen.setWidth(pView->getDrawParam()->getBlurWidth());
            pItem->setPen(pen);
            pItem->setBrush(Qt::NoBrush);
            event->scene()->addCItem(pItem);
            pItem->setBlurPicture(blurPicture);
            return pItem;
        }
    }
    return nullptr;
}

void CMasicoTool::drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene)
{
    Q_UNUSED(rect)
    Q_UNUSED(scene)
    if (!_allITERecordInfo.isEmpty() && _allITERecordInfo.first().hasMoved()) {
        painter->setClipPath(m_clippPath);
        painter->drawPixmap(QPoint(0, 0), m_tempBulrPix);
    }
}

void CMasicoTool::changeMouseShape(CGraphicsItem *item, QPointF point)
{
    if (item == nullptr)
        return ;

    if (item->type() != PictureType || !item->sceneBoundingRect().contains(point)) {
        QApplication::setOverrideCursor(QCursor(Qt::ForbiddenCursor));
    } else {
        // 缩放系数公式： 目的系数 = （1-最大系数）/ （最大值 - 最小值）
        double scanleRate = 0.5 / (500 - 5);
        int blur_width = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth();
        scanleRate = scanleRate * blur_width + 1.0;

        QPixmap pix = QPixmap(":/cursorIcons/smudge_mouse.png");
        pix = pix.scaled(static_cast<int>(pix.width() * scanleRate), static_cast<int>(pix.height() * scanleRate));
        QApplication::setOverrideCursor(pix);
    }
}

void CMasicoTool::updateRealTimePixmap(CDrawScene *scene)
{
    QPixmap tmpPixmap = scene->drawView()->cachedPixmap();
    //计算交叉矩形的区域

    for (auto it = _allITERecordInfo.begin(); it != _allITERecordInfo.end(); ++it) {
        ITERecordInfo &rInfo = it.value();
        CGraphicsMasicoItem *pMasItem = dynamic_cast<CGraphicsMasicoItem *>(rInfo.businessItem);
        //qDebug() << "CMasicoTool::drawMore m_clippPath = " << pMasItem->blurPath();
        m_clippPath = scene->drawView()->mapFromScene(pMasItem->mapToScene(pMasItem->blurPath()));
        //判断和他交叉的元素，裁剪出下层的像素
        //下层有图元才显示
        int imgWidth = tmpPixmap.width();
        int imgHeigth = tmpPixmap.height();
        int radius = 10;
        if (!tmpPixmap.isNull()) {
            tmpPixmap = tmpPixmap.scaled(imgWidth / radius, imgHeigth / radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if (pMasItem->getBlurEffect() == BlurEffect) {
                tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            } else {
                tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth);
            }
        }
        m_tempBulrPix = tmpPixmap;

        break;

    }
}
