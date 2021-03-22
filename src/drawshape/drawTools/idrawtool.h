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
#ifndef CDRAWTOOL_H
#define CDRAWTOOL_H

#include "globaldefine.h"
#include "csizehandlerect.h"
#include <QList>
#include <QCursor>
#include <QTouchEvent>
#include <QTime>

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
    explicit IDrawTool(EDrawToolMode mode);

    /**
     * @brief ~IDrawTool 析构函数
     */
    virtual ~IDrawTool() = 0;

    /**
     * @brief mousePressEvent 鼠标按下事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);

    /**
     * @brief mouseMoveEvent 鼠标移动事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);

    /**
     * @brief mouseReleaseEvent 鼠标放开事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);

    /**
     * @brief mouseDoubleClickEvent 鼠标双击事件
     * @param event 事件
     * @param scene 场景
     */
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene);

    /**
     * @brief isActived 是否是激活正在使用的(有触控或者鼠标左键点住中)
     */
    bool isActived();

    /**
     * @brief interrupt 停止并打断actived状态
     */
    void interrupt();

    class CDrawToolEvent
    {
    public:
        enum EPosType {EScenePos, EViewportPos, EGlobelPos, PosTypeCount};

        enum EEventTp { EMouseEvent,
                        ETouchEvent,
                        EEventSimulated,
                        EEventCount
                      };
        CDrawToolEvent(const QPointF &vPos      = QPointF(),
                       const QPointF &scenePos  = QPointF(),
                       const QPointF &globelPos = QPointF(),
                       CDrawScene *pScene = nullptr);

        typedef  QMap<int, CDrawToolEvent> CDrawToolEvents;

        static CDrawToolEvents fromQEvent(QEvent *event, CDrawScene *scene);
        static CDrawToolEvent fromTouchPoint(const QTouchEvent::TouchPoint &tPos,
                                             CDrawScene *scene, QEvent *eOrg = nullptr);

        QPointF                pos(EPosType tp = EScenePos) const;
        Qt::MouseButtons       mouseButtons() const;
        Qt::KeyboardModifiers  keyboardModifiers() const;
        int                    uuid() const;
        EEventTp eventType();
        QEvent                *orgQtEvent();
        CDrawScene            *scene();
        CGraphicsView *view() const;

        bool isAccepted() const;
        void setAccepted(bool b);

        bool isPosXAccepted() const;
//        void setPosXAccepted(bool b);

        bool isPosYAccepted() const;
//        void setPosYAccepted(bool b);

    private:
        QPointF _pos[PosTypeCount] = {QPointF(0, 0)};

        Qt::MouseButtons       _msBtns = Qt::NoButton;
        Qt::KeyboardModifiers  _kbMods = Qt::NoModifier;
        CDrawScene            *_scene  = nullptr;
        int                    _uuid   = 0;
        QEvent                *_orgEvent = nullptr;
        bool _accept = true;
        bool _acceptPosX = true;
        bool _acceptPosY = true;

        friend class IDrawTool;
    };

    /**
     * @brief toolDoStart 工具执行的开始
     * @param scene 场景
     */
    void toolDoStart(CDrawToolEvent *event);

    /**
     * @brief toolDoUpdate 工具执行的刷新
     * @param event 事件
     */
    void toolDoUpdate(CDrawToolEvent *event);

    /**
     * @brief toolDoFinish 工具执行的结束
     * @param event 事件
     */
    void toolDoFinish(CDrawToolEvent *event);

    /**
     * @brief isEnable 工具当前是否可用
     */
    virtual bool isEnable(CGraphicsView *pView);

protected:
    struct ITERecordInfo;

    /**
     * @brief dueTouchDoubleClickedStart　判定工具开始事件是否是一次双击事件，如果是就会转成双击
     * @param event      当次事件信息
     */
    bool dueTouchDoubleClickedStart(CDrawToolEvent *event);

    /**
     * @brief toolStart　工具开始事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    virtual void toolStart(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolStart　 工具刷新事件
     * @param event       当次事件信息
     * @param pInfo       记录信息
     */
    virtual void toolUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolStart　工具结束事件
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    virtual void toolFinish(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolStart　判断工具活跃类型
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    virtual int decideUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolStart　鼠标hover事件（处理高亮，鼠标样式变化等）
     * @param event      当次事件信息
     */
    virtual void mouseHoverEvent(IDrawTool::CDrawToolEvent *event);

    /**
     * @brief toolDoubleClikedEvent　双击事件
     * @param event      当次事件信息
     */
    virtual void toolDoubleClikedEvent(IDrawTool::CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolCreatItemStart　工具创造业务图元的开始
     * @param event               当次事件信息
     * @param pInfo               记录信息
     */
    virtual void toolCreatItemStart(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolCreatItemUpdate　工具创造业务图元的刷新
     * @param event                当次事件信息
     * @param pInfo                记录信息
     */
    virtual void toolCreatItemUpdate(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief toolCreatItemFinish　工具创造业务图元的结束
     * @param event                当次事件信息
     * @param pInfo                记录信息
     */
    virtual void toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief creatItem　工具创造业务图元指针
     * @param event      当次事件信息
     * @param pInfo      记录信息
     */
    virtual CGraphicsItem *creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief minMoveUpdateDistance　工具创造业务图元指针
     */

    virtual int minMoveUpdateDistance();

public:
    /**
     * @brief painter　绘制更多的内容（用于绘制框选矩形和高亮路径）
     * @param painter  绘制指针
     * @param rect     矩形大小
     * @param scene    场景指针
     */
    virtual void drawMore(QPainter *painter, const QRectF &rect, CDrawScene *scene);

    /**
     * @brief clearITE 清理ITE事件记录
     */
    void clearITE();

    /**
     * @brief setViewToolTo 设置当前页的工具
     * @param pView  当前页view
     * @param mode   工具类型
     */
    static void setViewToSelectionTool(CGraphicsView *pView = nullptr);

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
    static QCursor getCursor(CSizeHandleRect::EDirection dir, bool bMouseLeftPress = false, char toolType = 0);


//    /**
//     * @brief getCursorRotation 获取当前旋转角度
//     */
//    static qreal getCursorRotation();

protected:
    /**
     * @brief allowedMaxTouchPointCount 允许的最大触控点数
     */
    virtual int  allowedMaxTouchPointCount();

    /**
     * @brief isPressEventHandledByQt 是否将当前的事件交给qt框架去处理(一般应用于原生的qt控件及代理类，这样才能最大化让qt工作)
     */
    virtual bool isPressEventHandledByQt(CDrawToolEvent *event, ITERecordInfo *pInfo);

    /**
     * @brief returnToSelectTool 是否在一次完整的系列事件（点下->移动（可选）->释放）返回到select工具
     */
    virtual bool returnToSelectTool(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo);


    /**
     * @brief autoSupUndoForCreatItem 是否支持在创建生成图元后自动添加到undo栈(默认是true)
     */
    virtual bool autoSupUndoForCreatItem();


    /**
     * @brief sendToolEventToItem 将工具事件发送给图元，事件将会根据各工具的类型生成对应的图元事件
     */
    virtual void sendToolEventToItem(CDrawToolEvent *event,
                                     ITERecordInfo *info,
                                     EChangedPhase phase);

private:

    /**
     * @brief getCurVaildActivedPointCount 得到当前有效的激活的触控数(用于和allowedMaxTouchPointCount对比判断是否已经达到允许的最大触控点数)
     */
    int getCurVaildActivedPointCount();

protected:
    bool m_bMousePress;
    QPointF m_sPointPress;
    QPointF m_sLastPress;
    QPointF m_sPointRelease;

    bool m_bShiftKeyPress;
    bool m_bAltKeyPress;


    enum EEventLifeTo {EDoNotthing, EDoQtCoversion, ENormal};
    enum EToolOperate { EToolCreatItemMove = -6542,
                        EToolDoNothing = 0
                      };
    struct ITERecordInfo {
        EEventLifeTo eventLife = ENormal;
        QPointF _startPos;
        QPointF _prePos;
        QPointF m_sPointRelease;
        CGraphicsItem *businessItem = nullptr;
        QList<QGraphicsItem *> startPosItems;
        CGraphicsItem *startPosTopBzItem = nullptr;
        bool haveDecidedOperateType = false;
        bool _isvaild = false;
        int _opeTpUpdate = EToolDoNothing;
        int _etcopeTpUpdate = -1;
        int _elapsedToUpdate = -1;
        QList<QGraphicsItem *> etcItems;
        CDrawScene *_scene = nullptr;
        CDrawToolEvent _startEvent;
        CDrawToolEvent _preEvent;
        CDrawToolEvent _curEvent;
        bool _moved = false;

        bool isVaild() const;
        bool hasMoved() const;
        int elapsedFromStartToUpdate() const { return _elapsedToUpdate; }
        inline QTime *getTimeHandle();

    private:
        QTime _elapsedToUpdateTimeHandle;
    };

//    ITERecordInfo *getEventIteInfo(int uuid);

    QMap<int, ITERecordInfo> _allITERecordInfo;

public:
    QGraphicsItem *m_noShiftSelectItem;
private:
    EDrawToolMode m_mode;
    QCursor m_RotateCursor;
    QCursor m_LeftTopCursor;
    QCursor m_RightTopCursor;
    QCursor m_LeftRightCursor;
    QCursor m_UpDownCursor;

    QTimer *getTimerForDoubleCliked();
    static QTimer *s_timerForDoubleClike;
};

#endif // CDRAWTOOL_H
