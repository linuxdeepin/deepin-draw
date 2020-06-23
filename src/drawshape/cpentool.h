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
#include "cgraphicspenitem.h"

class CPenTool : public IDrawTool
{
public:
    CPenTool();
    virtual ~CPenTool() Q_DECL_OVERRIDE;

public:
    /**
     * @brief mousePressEvent　鼠标点击事件
     * @param event　场景事件
     * @param scene　场景句柄
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param event 场景事件
     * @param scene 场景句柄
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) Q_DECL_OVERRIDE;
    /**
     * @brief mouseReleaseEvent　鼠标弹起事件
     * @param event 场景事件
     * @param scene 场景句柄
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)Q_DECL_OVERRIDE;

    /**
     * @brief toolStart 工具执行的开始
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolStart(CDrawToolEvent *event)Q_DECL_OVERRIDE;

    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolUpdate(CDrawToolEvent *event)Q_DECL_OVERRIDE;

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolFinish(CDrawToolEvent *event)Q_DECL_OVERRIDE;

private:
    CGraphicsPenItem *m_pPenItem;
};

#endif // CPENTOOL_H
