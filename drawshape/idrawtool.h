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
#include <QTouchEvent>

class QGraphicsSceneMouseEvent;
class CDrawScene;
class CGraphicsItem;

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
     * @brief mouseDoubleClickEvent 鼠标双击事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);


    class CDrawToolEvent
    {
    public:
        enum EPosType {EScenePos, EViewportPos, EGlobelPos, PosTypeCount};

        CDrawToolEvent(const QPointF &vPos      = QPointF(),
                       const QPointF &scenePos  = QPointF(),
                       const QPointF &globelPos = QPointF(),
                       CDrawScene *pScene = nullptr);

        typedef  QMap<int, CDrawToolEvent> CDrawToolEvents;

        static CDrawToolEvents fromQEvent(QEvent *event, CDrawScene *scene);
        static CDrawToolEvent  fromTouchPoint(const QTouchEvent::TouchPoint &tPos, CDrawScene *scene);

        QPointF                pos(EPosType tp = EScenePos);
        Qt::MouseButtons       mouseButtons();
        Qt::KeyboardModifiers  keyboardModifiers();
        int                    uuid();
        QEvent                *orgQtEvent();
        CDrawScene            *scene();
    private:
        QPointF                _pos[PosTypeCount] = {QPointF(0, 0)};

        Qt::MouseButtons       _msBtns = Qt::NoButton;
        Qt::KeyboardModifiers  _kbMods = Qt::NoModifier;
        CDrawScene            *_scene  = nullptr;
        int                    _uuid   = 0;
        QEvent                *_orgEvent = nullptr;
    };

    /**
     * @brief toolStart 工具执行的开始
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolStart(CDrawToolEvent *event);

    /**
     * @brief toolUpdate 工具执行的刷新
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolUpdate(CDrawToolEvent *event);

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    virtual void toolFinish(CDrawToolEvent *event);


    virtual CGraphicsItem *creatItem();

    /**
     * @brief getDrawToolMode 获取当前工具类型
     * @return 工具类型
     */
    EDrawToolMode getDrawToolMode() const;

    /**
     * @brief getCursor 获取鼠标显示的样式
     * @param dir 方向
     * @param bMouseLeftPress true: 鼠标按下 false:鼠标没按下
     * @param toolType 0: 公共 1:selectTool
     * @return
     */
    QCursor getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress = false, char toolType = 0);

    qreal getCursorRotation();
protected:
    bool m_bMousePress;
    QPointF m_sPointPress;
    QPointF m_sLastPress;
    QPointF m_sPointRelease;

    bool m_bShiftKeyPress;
    bool m_bAltKeyPress;

    struct SRecordedStartInfo {
        QPointF m_sPointPress;
        QPointF m_sLastPress;
        QPointF m_sPointRelease;
        CGraphicsItem *tempItem = nullptr;
    };

    QMap<int, SRecordedStartInfo> allStartInfo;

public:
    QGraphicsItem *m_noShiftSelectItem;
private:
    EDrawToolMode m_mode;
    QCursor m_RotateCursor;
    QCursor m_LeftTopCursor;
    QCursor m_RightTopCursor;
    QCursor m_LeftRightCursor;
    QCursor m_UpDownCursor;
};

#endif // CDRAWTOOL_H
