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
#ifndef CMASICOTOOL_H
#define CMASICOTOOL_H
#include <drawitemtool.h>

class PageScene;
class RasterItem;
class DRAWLIB_EXPORT BlurTool: public DrawItemTool
{
    Q_OBJECT
public:
    explicit BlurTool(QObject *parent = nullptr);

    int toolType() const override;

    SAttrisList attributions() override;

    int  blurType() const;
    void setBlurType(int tp);

protected:
    PageItem *drawItemStart(ToolSceneEvent *event) override;
    void      drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;
    void      drawItemFinish(ToolSceneEvent *event, PageItem *pItem) override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;
    void onCurrentPageChanged(Page *newPage) override;
    int  minMoveUpdateDistance() override;

    void drawMore(QPainter *painter, const QRectF &rect, PageScene *scene) override;

    void clearPointRecording() override;

    RasterItem *getRasterItem(ToolSceneEvent *event);
    QPainterPath &getActivedOrgPath(ToolSceneEvent *event, RasterItem *rstItem);
    QPainterPath &getActivedStrokerPath(ToolSceneEvent *event, RasterItem *rstItem);

    void creatBlurSrokerPaths(ToolSceneEvent *event, RasterItem *rstItem);

    Q_SLOT void _onSceneSelectionChanged(const QList<PageItem * > &selectedItems);

    PRIVATECLASS(BlurTool)
};

#endif // CMASICOTOOL_H
