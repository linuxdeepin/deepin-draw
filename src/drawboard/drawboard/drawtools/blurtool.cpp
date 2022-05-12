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

#include "blurtool.h"
#include "pagescene.h"
#include "pagecontext.h"
#include "pageview.h"
#include "itemgroup.h"
#include "rasteritem.h"
#include "global.h"

#include <QGraphicsView>
#include <QToolButton>
#include <QDebug>

class BlurTool::BlurTool_private
{
public:
    explicit BlurTool_private(BlurTool *qc): q(qc) {}

    BlurTool *q;

    int blurTp = 0;
    int blurWidth = 30;
    int blurRadius = 10;

    QMap<PageScene *, RasterItem *> _layers;

    QMap<RasterItem *, QImage> tempImages;

    QMap<int, QPainterPath> blurActivedOrgPath;
    QMap<int, QPainterPath> blurActivedStrokerPath;


    PageScene *currentScene = nullptr;
};

BlurTool::BlurTool(QObject *parent): DrawItemTool(parent), BlurTool_d(new BlurTool_private(this))
{
    setClearSelectionOnActived(false);
    QPixmap s_cur = QPixmap(":/cursorIcons/smudge_mouse.svg");
    setCursor(QCursor(s_cur));

    auto m_blurBtn = toolButton();
    m_blurBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_B)));
    setWgtAccesibleName(m_blurBtn, "Blur tool button");
    m_blurBtn->setToolTip(tr("Blur (B)"));
    m_blurBtn->setIconSize(QSize(20, 20));
    m_blurBtn->setFixedSize(QSize(37, 37));
    m_blurBtn->setCheckable(true);
    m_blurBtn->setIcon(QIcon::fromTheme("vague_normal"));
}

int BlurTool::toolType() const
{
    return blur;
}

SAttrisList BlurTool::attributions()
{
    SAttrisList result;
    result << defaultAttriVar(EBlurAttri);
    return result;
}

int BlurTool::blurType() const
{
    return d_BlurTool()->blurTp;
}

void BlurTool::setBlurType(int tp)
{
    d_BlurTool()->blurTp = tp;
}

PageItem *BlurTool::drawItemStart(ToolSceneEvent *event)
{
    RasterItem *pItem = getRasterItem(event);

    QPointF  pos = pItem->mapFromScene((event->pos()));
    d_BlurTool()->blurActivedOrgPath[event->uuid()].moveTo(pos);
    return pItem;
}

void BlurTool::drawItemUpdate(ToolSceneEvent *event, PageItem *pItem)
{
    auto rasterItem = dynamic_cast<RasterItem *>(pItem);
    creatBlurSrokerPaths(event, rasterItem);

    // 获取到当前顶层图元
    auto currenItem = event->scene()->topPageItem(event->pos(), true,
                                                  event->eventType() == ToolSceneEvent::ETouchEvent ? drawBoard()->touchFeelingEnhanceValue() : 0);

    // 判断顶层图元是否发生变化
    bool eraserActive = (rasterItem == currenItem) ? true : false;

    // 更新鼠标光标
    event->scene()->page()->setDrawCursor(eraserActive ? cursor() : QCursor(Qt::ForbiddenCursor));

    event->view()->viewport()->update();
}

void BlurTool::drawItemFinish(ToolSceneEvent *event, PageItem *pItem)
{
    d_BlurTool()->blurActivedOrgPath.remove(event->uuid());
    auto blurStrokerPath = d_BlurTool()->blurActivedStrokerPath.take(event->uuid());


    RasterItem *rst = static_cast<RasterItem *>(pItem);
    blurStrokerPath = rst->imgTrans().map(blurStrokerPath);
    RasterBlurCmd *cmd = new RasterBlurCmd(blurStrokerPath, d_BlurTool()->blurTp);
    rst->appendComand(cmd, true, false);

    if (event->isFinalOne()) {
        d_BlurTool()->_layers.remove(event->scene());
        d_BlurTool()->tempImages.remove(rst);
        setCursor(cursor());
    }
    event->view()->viewport()->update();
}

void BlurTool::onStatusChanged(EStatus oldStatus, EStatus nowStatus)
{

}

void BlurTool::onCurrentPageChanged(Page *newPage)
{
    auto newScene = (newPage == nullptr ? nullptr : newPage->scene());

    if (newScene != d_BlurTool()->currentScene) {
        if (d_BlurTool()->currentScene != nullptr) {
            disconnect(d_BlurTool()->currentScene, &PageScene::selectionChanged, this, &BlurTool::_onSceneSelectionChanged);
        }

        d_BlurTool()->currentScene = newScene;

        if (newScene != nullptr) {
            connect(d_BlurTool()->currentScene, &PageScene::selectionChanged, this, &BlurTool::_onSceneSelectionChanged);
            _onSceneSelectionChanged(newScene->selectedPageItems());
        }
    }
}

int BlurTool::minMoveUpdateDistance()
{
    return 0;
}

void BlurTool::drawMore(QPainter *painter, const QRectF &rect, PageScene *scene)
{
    DrawItemTool::drawMore(painter, rect, scene);
    if (d_BlurTool()->_layers.contains(scene)) {
        auto rst = d_BlurTool()->_layers[scene];
        if (d_BlurTool()->tempImages.contains(rst)) {
            auto bluredImag = d_BlurTool()->tempImages[rst];
            painter->setTransform(rst->sceneTransform(), true);

            foreach (auto clipPath, d_BlurTool()->blurActivedStrokerPath) {
                painter->setClipPath(clipPath);
                painter->drawImage(rst->itemRect(), bluredImag, bluredImag.rect());
            }
        }
    }
}

void BlurTool::clearPointRecording()
{
    d_BlurTool()->blurActivedOrgPath.clear();
    d_BlurTool()->blurActivedStrokerPath.clear();
    DrawItemTool::clearPointRecording();
}
RasterItem *BlurTool::getRasterItem(ToolSceneEvent *event)
{
    RasterItem *pItem = nullptr;
    auto findIt = d_BlurTool()->_layers.find(event->scene());
    if (d_BlurTool()->_layers.find(event->scene()) == d_BlurTool()->_layers.end()) {
        auto scene = event->scene();
        RasterItem *layer = nullptr;
        if (scene->selectedItemCount() == 1) {
            auto pSelected = dynamic_cast<RasterItem *>(scene->selectedPageItems().first());
            if (pSelected != nullptr) {
                layer = pSelected;
            }
        }

        if (layer != nullptr) {
            scene->selectPageItem(layer);
            d_BlurTool()->_layers.insert(scene, layer);
        }

        pItem = layer;
    } else {
        pItem = findIt.value();
    }

    return pItem;
}

QPainterPath &BlurTool::getActivedOrgPath(ToolSceneEvent *event, RasterItem *rstItem)
{
    if (!d_BlurTool()->blurActivedOrgPath.contains(event->uuid())) {
        QPainterPath path; path.moveTo(event->pos());
        d_BlurTool()->blurActivedOrgPath.insert(event->uuid(), path);
    }
    return d_BlurTool()->blurActivedOrgPath[event->uuid()];
}

QPainterPath &BlurTool::getActivedStrokerPath(ToolSceneEvent *event, RasterItem *rstItem)
{
    return d_BlurTool()->blurActivedStrokerPath[event->uuid()];
}

void BlurTool::creatBlurSrokerPaths(ToolSceneEvent *event, RasterItem *rstItem)
{
    if (d_BlurTool()->tempImages.find(rstItem) == d_BlurTool()->tempImages.end()) {
        QImage img = NSBlur::blurImage(rstItem->rImage(), d_BlurTool()->blurRadius, d_BlurTool()->blurTp);
        d_BlurTool()->tempImages[rstItem] = img;
    }

    //QPointF  prePos = rstItem->mapFromScene(event->lastEvent()->pos());
    QPointF  pos = rstItem->mapFromScene((event->pos()));

    auto &path = d_BlurTool()->blurActivedOrgPath[event->uuid()];
    QPointF lastPos = path.elementAt(path.elementCount() - 1);
    path.lineTo(pos == lastPos ? pos + QPointF(0.000001, 0.000001) : pos);
    QPen pen;
    pen.setWidthF(d_BlurTool()->blurWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    d_BlurTool()->blurActivedStrokerPath[event->uuid()] = VectorItem::getPenStrokerPath(path,
                                                                                        pen, true, 0, false);
    event->scene()->update();
}

void BlurTool::_onSceneSelectionChanged(const QList<PageItem *> &selectedItems)
{
    if (selectedItems.count() == 1) {
        auto first = selectedItems.first();
        setEnable(first->type() == RasterItemType);
    } else {
        setEnable(false);
    }
}
