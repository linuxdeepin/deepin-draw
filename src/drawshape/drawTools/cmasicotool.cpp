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
#include "cundoredocommand.h"

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
}

void CMasicoTool::toolCreatItemFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
}

CGraphicsItem *CMasicoTool::creatItem(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    return nullptr;
}

void CMasicoTool::toolStart(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)

    //光标入栈
    drawApp->saveCursor();

    //缓存光标
    _blurCursor           = *dApp->overrideCursor();

    //获取到这次能进行模糊的图元
    auto selectItems      = event->scene()->selectGroup()->items();
    auto pCurSelectItem   = selectItems.count() == 1 ? selectItems.first() : nullptr;
    _blurEnableItems      = getBlurEnableItems(pCurSelectItem);

    if (_blurEnableItems.count() > 0) {
        _pressedPosBlurEnable = true;

        //置顶前,收集到可能会被模糊的图片图元数据(置顶后再获取到图元z值的话就不是最原始的z值了)
        event->scene()->recordItemsInfoToCmd(_blurEnableItems,
                                             UndoVar, true);

        //置顶功能:实现进行模糊时,被模糊的图元置顶

        //置顶功能Step1.保存场景当前图元的z值
        saveZ(event->scene());

        //置顶功能Step2.置顶
        event->scene()->moveBzItemsLayer(selectItems, EUpLayer, -1);

    } else {
        _pressedPosBlurEnable = false;
    }

    drawApp->setApplicationCursor(isBlurEnable(pInfo->startPosTopBzItem) ? _blurCursor : QCursor(Qt::ForbiddenCursor));
}

int CMasicoTool::decideUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)

    if (_pressedPosBlurEnable) {
        enum {EDoBLur = 1};
        return EDoBLur;
    }
    return 0;
}

void CMasicoTool::toolUpdate(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)

    if (!_pressedPosBlurEnable)
        return;

    //0.获取到当前最上层的图元是
    auto currenItem = event->scene()->topBzItem(event->pos(), true,
                                                event->eventType() == CDrawToolEvent::ETouchEvent ? drawApp->touchFeelingEnhanceValue() : 0);

    bool blurActive = isBlurEnable(currenItem);

    //1.顶层图元发生改变或没变化的情况处理
    if (_pLastTopItem != currenItem) {
        //1.1:那么原来的顶层图元_pLastTopItem如果可以模糊那么需要进行模糊路径的结束
        if (isBlurEnable(_pLastTopItem)) {
            _pLastTopItem->blurEnd();
        }
        //1.2:新的顶层图元currenItem如果可以模糊那么进行模糊开始
        if (blurActive) {
            currenItem->blurBegin(currenItem->mapFromScene(event->pos()));
            _bluringItemsSet.insert(currenItem);
        }
        _pLastTopItem = currenItem;
    } else {
        //1.3:顶层图元未发生改变:那么直接进行模糊路径刷新即可
        if (blurActive) {
            currenItem->blurUpdate(currenItem->mapFromScene(event->pos()), true);
        }
    }

    //2.更新鼠标样式
    drawApp->setApplicationCursor(blurActive ? _blurCursor : QCursor(Qt::ForbiddenCursor));
}

void CMasicoTool::toolFinish(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)
    if (_pressedPosBlurEnable) {

        //置顶功能Step3.还原z值
        restoreZ();

        _pressedPosBlurEnable = false;

        enum {EDoBLur = 1};
        if (pInfo->_opeTpUpdate == EDoBLur) {
            for (auto pItem : _bluringItemsSet) {

                if (pItem->isBlurActived()) {
                    pItem->blurEnd();
                }
                pItem->resetCachePixmap();

                event->scene()->recordItemsInfoToCmd(QList<CGraphicsItem *>() << pItem,
                                                     RedoVar, false);
            }
            event->scene()->finishRecord(false);

            _bluringItemsSet.clear();
        } else {
            //没有移动那么就直接清理命令
            CUndoRedoCommand::clearCommand();
        }
    }

    _pLastTopItem = nullptr;

    //恢复光标
    drawApp->restoreCursor();
}

void CMasicoTool::drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene)
{
    Q_UNUSED(rect)
    Q_UNUSED(scene)
    Q_UNUSED(painter);
}

bool CMasicoTool::returnToSelectTool(IDrawTool::CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    return !pInfo->hasMoved();
}

bool CMasicoTool::isBlurEnable(const CGraphicsItem *pItem)
{
    return _blurEnableItems.contains(const_cast<CGraphicsItem *>(pItem));
}

QList<CGraphicsItem *> CMasicoTool::getBlurEnableItems(const CGraphicsItem *pItem)
{
    QList<CGraphicsItem *> resultItems;
    if (pItem == nullptr)
        return QList<CGraphicsItem *>();

    if (pItem->isBzGroup()) {
        auto items = static_cast<const CGraphicsItemGroup *>(pItem)->items(true);
        for (auto p : items) {
            if (p->type() == PictureType) {
                resultItems.append(const_cast<CGraphicsItem *>(p));
            }
        }
    } else {
        if (pItem->type() == PictureType) {
            resultItems.append(const_cast<CGraphicsItem *>(pItem));
        }
    }
    return resultItems;
}

void CMasicoTool::saveZ(CDrawScene *scene)
{
    _tempZs.clear();
    auto invokedItems = scene->getRootItems(CDrawScene::EAesSort);
    for (int i = 0; i < invokedItems.size(); ++i) {
        auto pItem = invokedItems.at(i);
        saveItemZValue(pItem);
    }
}

void CMasicoTool::restoreZ()
{
    for (auto it = _tempZs.begin(); it != _tempZs.end(); ++it) {
        it.key()->setZValue(it.value());
    }
    _tempZs.clear();
}

void CMasicoTool::saveItemZValue(CGraphicsItem *pItem)
{
    if (pItem->isBzItem()) {
        _tempZs.insert(pItem, pItem->drawZValue());
    } else if (pItem->isBzGroup()) {
        auto items = static_cast<CGraphicsItemGroup *>(pItem)->items();
        for (auto p : items) {
            saveItemZValue(p);
            _tempZs.insert(pItem, pItem->drawZValue());
        }
    }
}
