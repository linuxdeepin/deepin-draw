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
#ifndef CPENTOOL_H
#define CPENTOOL_H
#include "drawitemtool.h"
#include "rasteritem.h"

#include <QUndoCommand>
#include <QPicture>
#include <QPointer>

class DRAWLIB_EXPORT PenTool : public DrawItemTool
{
    Q_OBJECT
public:
    enum EPenToolType {ENormalPen = 1, ECalligraphyPen, ECrayonPen};

    PenTool(QObject *parent = nullptr);

    int toolType() const override;

    SAttrisList attributions() override;

protected:
    PageItem *drawItemStart(ToolSceneEvent *event) override;

    void drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;

    void drawItemFinish(ToolSceneEvent *event, PageItem *pItem) override;

    int minMoveUpdateDistance() override;

    void onStatusChanged(EStatus oldStatus, EStatus nowStatus) override;

    PRIVATECLASS(PenTool)
};


#endif // CPENTOOL_H
