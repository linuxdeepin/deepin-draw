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
#ifndef CSELECTTOOL_H
#define CSELECTTOOL_H
#include "idrawtool.h"
#include "globaldefine.h"
#include "csizehandlerect.h"


#include <QCursor>

class CGraphicsItem;
class CGraphicsRotateAngleItem;
class CGraphicsItemSelectedMgr;
class CGraphicsRectItem;
class GraphicsEllipseItem;
class QUndoCommand;

class CSelectTool : public IDrawTool
{
public:
    CSelectTool();
    virtual ~CSelectTool();
    /**
     * @brief mousePressEvent　鼠标点击事件
     * @param event　场景事件
     * @param scene　场景句柄
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param event 场景事件
     * @param scene 场景句柄
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
    /**
     * @brief mouseReleaseEvent　鼠标弹起事件
     * @param event 场景事件
     * @param scene 场景句柄
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);
private:
    /**
     * @brief pintToRect　任意两点确定一个矩形
     * @param point1 任意一点
     * @param point2 任意一点
     */
    QRectF pointToRect(QPointF point1, QPointF point2);
private:
    QGraphicsItem *m_currentSelectItem;
    QGraphicsRectItem *m_frameSelectItem;
    CSizeHandleRect::EDirection m_dragHandle; //选中的方块方向
    bool m_bRotateAng;
    qreal m_rotateAng;
    QPointF m_initRotateItemPos;
    CGraphicsRotateAngleItem *m_RotateItem;
    QCursor m_textEditCursor;
    bool m_doCopy;
    bool m_doMove;
    bool m_doResize;

};

#endif // CSELECTTOOL_H
