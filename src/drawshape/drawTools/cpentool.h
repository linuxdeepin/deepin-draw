/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "idrawtool.h"

class CPenTool : public IDrawTool
{
public:
    CPenTool();
    virtual ~CPenTool() Q_DECL_OVERRIDE;

protected:

    CGraphicsItem *creatItem(CDrawToolEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) Q_DECL_OVERRIDE;

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) Q_DECL_OVERRIDE;

    /**
     * @brief painter　绘制更多的内容（用于绘制shift按住时直线线段）
     * @param painter  绘制指针
     * @param rect     矩形大小
     * @param scene    场景指针
     */
    void drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene) override;


    int  allowedMaxTouchPointCount() override;
};

#endif // CPENTOOL_H
