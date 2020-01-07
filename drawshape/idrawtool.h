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
#ifndef CDRAWTOOL_H
#define CDRAWTOOL_H

#include "globaldefine.h"
#include "csizehandlerect.h"
#include <QList>
#include <QCursor>

class QGraphicsSceneMouseEvent;
class CDrawScene;

class IDrawTool
{
public:
    /**
     * @brief IDrawTool 构造函数
     * @param mode 工具类型
     */
    IDrawTool(EDrawToolMode mode);

    /**
     * @brief ~IDrawTool 析构函数
     */
    virtual ~IDrawTool() = 0;

    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;

    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;

    /**
     * @brief mouseReleaseEvent 鼠标放开事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene) = 0;

    /**
     * @brief getDrawToolMode 获取当前工具类型
     * @return 工具类型
     */
    EDrawToolMode getDrawToolMode() const;

    /**
     * @brief getCursor 获取鼠标显示的样式
     * @param dir 方向
     * @param bMouseLeftPress true: 鼠标按下 false:鼠标没按下
     * @return
     */
    QCursor getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress = false);

protected:
    bool m_bMousePress;
    QPointF m_sPointPress;
    QPointF m_sLastPress;
    QPointF m_sPointRelease;

    bool m_bShiftKeyPress;
    bool m_bAltKeyPress;


private:
    EDrawToolMode m_mode;
    QCursor m_RotateCursor;

};

#endif // CDRAWTOOL_H
