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
#include <QPointF>
#include <QTouchEvent>

class PageScene;
class PageView;

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
                   PageScene *pScene = nullptr);

    typedef  QMap<int, CDrawToolEvent> CDrawToolEvents;

    static CDrawToolEvents fromQEvent(QEvent *event, PageScene *scene);
    static CDrawToolEvent fromQEvent_single(QEvent *event, PageScene *scene);
    static CDrawToolEvent fromTouchPoint(const QTouchEvent::TouchPoint &tPos,
                                         PageScene *scene, QTouchEvent *eOrg = nullptr);

    QPointF                pos(EPosType tp = EScenePos) const;
    Qt::MouseButtons       mouseButtons() const;
    Qt::KeyboardModifiers  keyboardModifiers() const;
    int                    uuid() const;
    EEventTp               eventType();
    QEvent                *orgQtEvent();
    PageScene            *scene();
    PageView *view() const;

    bool isAccepted() const;
    void setAccepted(bool b);

    bool isPosXAccepted() const;
    void setPosXAccepted(bool b);

    bool isPosYAccepted() const;
    void setPosYAccepted(bool b);

private:
    QPointF _pos[PosTypeCount] = {QPointF(0, 0)};

    Qt::MouseButtons       _msBtns = Qt::NoButton;
    Qt::KeyboardModifiers  _kbMods = Qt::NoModifier;
    PageScene            *_scene  = nullptr;
    int                    _uuid   = 0;
    QEvent                *_orgEvent = nullptr;
    bool _accept = true;
    bool _acceptPosX = true;
    bool _acceptPosY = true;

    friend class IDrawTool;
};

#endif // IDRAWTOOLEVENT_H
