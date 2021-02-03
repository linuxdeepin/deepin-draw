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
    virtual ~CPenTool() override;

protected:

    CGraphicsItem *creatItem(CDrawToolEvent *event) override;

    /**
     * @brief toolCreatItemStart 工具创建Item开始
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemStart(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief painter　绘制更多的内容（用于绘制shift按住时直线线段）
     * @param painter  绘制指针
     * @param rect     矩形大小
     * @param scene    场景指针
     */
    void drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene) override;


    /**
     * @brief allowedMaxTouchPointCount 允许的最大支持实时绘制点数
     */
    int  allowedMaxTouchPointCount() override;

    /**
     * @brief returnToSelectTool 工具执行的结束
     * @param event 事件
     * @param pInfo 额外信息
     */
    bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;
};

#endif // CPENTOOL_H
