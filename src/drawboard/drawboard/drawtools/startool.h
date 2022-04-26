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
#ifndef CPOLYGONALSTARTOOL_H
#define CPOLYGONALSTARTOOL_H
#include "drawitemtool.h"

class DRAWLIB_EXPORT StarTool : public DrawItemTool
{
    Q_OBJECT
public:
    StarTool(QObject *parent = nullptr);

    int toolType() const override;

    ~StarTool() override;

    enum {EStartLineSep = EUserAttri + 777};
    SAttrisList attributions() override;

    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    void drawItemUpdate(ToolSceneEvent *event, PageItem *pItem) override;

    /**
     * @brief toolFinish 创建图元
     * @param event 事件
     * @param scene 场景
     */
    PageItem *drawItemStart(ToolSceneEvent *event) override;
};

#endif // CPOLYGONALSTARTOOL_H
