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
#ifndef IDRAWTOOLEVENT_H
#define IDRAWTOOLEVENT_H

#include "../utils/globaldefine.h"

#include <QPointF>
#include <QTouchEvent>

class PageScene;
class PageView;
class PageItem;
class QGraphicsItem;

class DRAWLIB_EXPORT ToolSceneEvent
{
public:
    enum EEventLifeTo {EDoNotthing, EDoQtCoversion, ENormal};
//    enum EToolOperate { EToolCreatItemMove = -6542,
//                        EToolDoNothing = 0
//                      };

    enum EPosType {EScenePos, EViewportPos, EGlobelPos, EInCurrentLayer, PosTypeCount};

    enum EEventTp { EMouseEvent,
                    ETouchEvent,
                    EEventSimulated,
                    EEventCount
                  };
    ToolSceneEvent(const QPointF &vPos      = QPointF(),
                   const QPointF &scenePos  = QPointF(),
                   const QPointF &globelPos = QPointF(),
                   PageScene *pScene = nullptr);

    typedef  QMap<int, ToolSceneEvent> CDrawToolEvents;

    static CDrawToolEvents fromQEvent(QEvent *event, PageScene *scene);
    static ToolSceneEvent  fromQEvent_single(QEvent *event, PageScene *scene);
    static ToolSceneEvent  fromTouchPoint(const QTouchEvent::TouchPoint &tPos,
                                          PageScene *scene, QTouchEvent *eOrg = nullptr);

    QPointF                scenePos()const;
    QPointF                viewPos()const;
    QPointF                globelPos()const;
    QPointF                currentLayerPos()const;
    QPointF                toItemPos(PageItem *item)const;

    QPointF                pos(EPosType tp = EScenePos) const;
    void                   setPos(const QPointF &pos, EPosType tp = EScenePos);
    Qt::MouseButtons       mouseButtons() const;
    Qt::KeyboardModifiers  keyboardModifiers() const;
    int                    uuid() const;
    EEventTp               eventType()const;
    QEvent                *orgQtEvent() const;
    PageScene             *scene() const;
    PageView *view() const;
    bool isPressed()const;
    bool isNormalPressed() const;

    QList<QGraphicsItem *> itemsUnderPressedPos();
    PageItem *topPageItemUnderPressedPos();
    QGraphicsItem *topItemUnderPressedPos();

    ToolSceneEvent *firstEvent() const;
    ToolSceneEvent *lastEvent() const;

    int  activedEventCount() const;
    //int  movedDistance() const;

    bool haveMoved() const;

    bool isFirstOne() const {return activedEventCount() == 1;}
    bool isFinalOne() const {return activedEventCount() == 1;}

    bool isAccepted() const;
    void setAccepted(bool b);

    //bool isPosXAccepted() const;
    void setPosXAccepted(bool b);

    //bool isPosYAccepted() const;
    void setPosYAccepted(bool b);

private:
    PRIVATECLASS(ToolSceneEvent)

    friend class IDrawTool;
    friend class DrawToolBase;
};

#endif // IDRAWTOOLEVENT_H
