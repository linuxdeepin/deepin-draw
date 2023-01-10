/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Wang Yicun <wangyicun@uniontech.com>
 *
 * Maintainer: Ji Xianglong <jixianglong@uniontech.com>
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
#ifndef CERASERTOOL_H
#define CERASERTOOL_H
#include "drawitemtool.h"
#include "rasteritem.h"
#include "globaldefine.h"

class DRAWLIB_EXPORT EraserTool : public DrawItemTool
{
    Q_OBJECT
public:
    EraserTool(QObject *parent = nullptr);

    ~EraserTool() override;

    int toolType() const override;

    SAttrisList attributions() override;

protected:

    PageItem *drawItemStart(ToolSceneEvent *event) override;
    void      drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;
    void      drawItemFinish(ToolSceneEvent *event, PageItem *pItem) override;
    void      drawItemHover(ToolSceneEvent *event) override;

    int  minMoveUpdateDistance() override;

    void onCurrentPageChanged(Page *newPage) override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    void enterSceneEvent(ToolSceneEvent *event) override;

    void leaveSceneEvent(ToolSceneEvent *event) override;

    void drawMore(QPainter *painter, const QRectF &rect, PageScene *scene) override;

protected:
    RasterPaint paintTempErasePen(ToolSceneEvent *event, RasterItem *pItem);

    RasterItem *getRasterItem(ToolSceneEvent *event);

    Q_SLOT void _onSceneSelectionChanged(const QList<PageItem * > &selectedItems);

    PRIVATECLASS(EraserTool)
};

#endif // CERASERTOOL_H
