/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: ZhangYong <zhangyong@uniontech.com>
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
#ifndef CFILLTOOL_H
#define CFILLTOOL_H
#include "drawfunctiontool.h"
class RasterItem;
class Q_DECL_EXPORT FillTool: public DrawFunctionTool
{
    Q_OBJECT
public:
    FillTool(QObject *parent = nullptr);

    int toolType() const override;

    SAttrisList attributions() override;

    int  minMoveUpdateDistance() override;

    void funcStart(ToolSceneEvent *event)  override;
    void funcUpdate(ToolSceneEvent *event, int decided) override;
    void funcFinished(ToolSceneEvent *event, int decided) override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    PageItem *currentLayer(ToolSceneEvent *event);
};

#endif // CFILLTOOL_H
