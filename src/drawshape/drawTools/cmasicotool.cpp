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
#include "cpictureitem.h"
#include "global.h"
#include "application.h"

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
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
//#if 0
//    if (pInfo != nullptr) {
//        CGraphicsMasicoItem *pItem = dynamic_cast<CGraphicsMasicoItem *>(pInfo->businessItem);
//        if (nullptr != pItem) {
//            QPointF pointMouse = event->pos();

//            //判断是否是鼠标禁用状态
//            changeMouseShape(pInfo->startPosTopBzItem, event->pos());

//            bool shiftKeyPress = event->keyboardModifiers() & Qt::ShiftModifier;
//            pItem->updatePenPath(pointMouse, shiftKeyPress);
//            pItem->updateBlurPath();
//            //pItem->updateMasicPixmap();

//            updateRealTimePixmap(event->scene());

//            event->setAccepted(true);
//        }
//    }
//#endif
}

void CMasicoTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
//#if 0
//    if (pInfo != nullptr) {
//        CGraphicsMasicoItem *pItem = dynamic_cast<CGraphicsMasicoItem *>(pInfo->businessItem);
//        if (nullptr != pItem) {
//            if (!pInfo->hasMoved()) {
//                event->scene()->removeCItem(pItem);
//                delete pItem;
//                pInfo->businessItem = nullptr;
//            } else {
//                pItem->drawComplete();
//                pItem->updateBlurPath();
//                pItem->updateMasicPixmap();
//                if (pItem->scene() == nullptr) {
//                    pItem->drawScene()->addCItem(pItem);
//                }
//            }
//        }
//        event->view()->setCacheEnable(false);
//    }

//    IDrawTool::toolCreatItemFinish(event, pInfo);
//#endif
}

CGraphicsItem *CMasicoTool::creatItem(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
//#if 0
//    if (!creatBlurItem)
//        return nullptr;

//    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
//            || event->eventType() == CDrawToolEvent::ETouchEvent) {

//        //判断是否是鼠标禁用状态
//        changeMouseShape(pInfo->startPosTopBzItem, event->pos());

//        if (pInfo->startPosTopBzItem != nullptr) {
//            event->view()->setCacheEnable(true);
//            updateRealTimePixmap(event->scene());

//            CGraphicsMasicoItem *pItem = new CGraphicsMasicoItem(event->pos());
//            CGraphicsView *pView = event->scene()->drawView();
//            QPen pen;
//            QColor color(255, 255, 255, 0);
//            pen.setColor(color);
//            pen.setWidth(pView->getDrawParam()->getBlurWidth());
//            pItem->setPen(pen);
//            pItem->setBrush(Qt::NoBrush);
//            event->scene()->addCItem(pItem);
//            return pItem;
//        }
//    }
//#endif
    return nullptr;
}

void CMasicoTool::toolStart(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    //光标入栈
    drawApp->saveCursor();
    _blurCursor = *dApp->overrideCursor();

    //判断是否是鼠标禁用状态
    changeMouseShape(pInfo->startPosTopBzItem, event->pos());

    if (pInfo->startPosTopBzItem != nullptr && pInfo->startPosTopBzItem->type() == PictureType) {

        event->scene()->recordItemsInfoToCmd(QList<CGraphicsItem *>() << pInfo->startPosTopBzItem,
                                             UndoVar, true);
        //针对某一个图元进行模糊
        _blurBegin = true;
        pInfo->startPosTopBzItem->blurBegin(pInfo->startPosTopBzItem->mapFromScene(event->pos()));
        //图片模糊置顶显示
        _zTemp = pInfo->startPosTopBzItem->drawZValue();
        pInfo->startPosTopBzItem->setZValue(INT_MAX - 1);
    }
}

int CMasicoTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    enum {EDoBLur = 1};
    return EDoBLur;
}

void CMasicoTool::toolUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    //判断是否是鼠标禁用状态
    changeMouseShape(pInfo->startPosTopBzItem, event->pos());

    //qDebug() << "_blurBegin =========== " << _blurBegin;
    if (_blurBegin) {
        pInfo->startPosTopBzItem->blurUpdate(pInfo->startPosTopBzItem->mapFromScene(event->pos()));
    }
}

void CMasicoTool::toolFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    if (_blurBegin) {
        pInfo->startPosTopBzItem->blurEnd();

        event->scene()->recordItemsInfoToCmd(QList<CGraphicsItem *>() << pInfo->startPosTopBzItem,
                                             RedoVar, false);
        event->scene()->finishRecord(false);
        _blurBegin = false;
        //还原图片图元Z值
        pInfo->startPosTopBzItem->setZValue(_zTemp);

    }
    //恢复光标
    drawApp->restoreCursor();
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

bool CMasicoTool::returnToSelectTool(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    return !pInfo->hasMoved();
}

void CMasicoTool::changeMouseShape(CGraphicsItem *item, QPointF point)
{
    if (item == nullptr || item->type() != PictureType || !item->sceneBoundingRect().contains(point)) {
        drawApp->setApplicationCursor(QCursor(Qt::ForbiddenCursor));
    } else {
        drawApp->setApplicationCursor(_blurCursor);
    }
}

//void CMasicoTool::updateRealTimePixmap(CDrawScene *scene)
//{
//    QPixmap tmpPixmap = scene->drawView()->cachedPixmap();
//    //计算交叉矩形的区域

//    for (auto it = _allITERecordInfo.begin(); it != _allITERecordInfo.end(); ++it) {
//        ITERecordInfo &rInfo = it.value();
//        CGraphicsMasicoItem *pMasItem = dynamic_cast<CGraphicsMasicoItem *>(rInfo.businessItem);
//        //qDebug() << "CMasicoTool::drawMore m_clippPath = " << pMasItem->blurPath();
//        m_clippPath = scene->drawView()->mapFromScene(pMasItem->mapToScene(pMasItem->blurPath()));
//        //判断和他交叉的元素，裁剪出下层的像素
//        //下层有图元才显示
//        int imgWidth = tmpPixmap.width();
//        int imgHeigth = tmpPixmap.height();

//        if (!tmpPixmap.isNull()) {
//            tmpPixmap = tmpPixmap.scaled(imgWidth / 10, imgHeigth / 10, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//            if (pMasItem->getBlurEffect() == BlurEffect) {
//                tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//            } else {
//                tmpPixmap = tmpPixmap.scaled(imgWidth, imgHeigth);
//            }
//        }
//        m_tempBulrPix = tmpPixmap;

//        break;

//    }
//}
