#ifndef IDRAWTOOLEVENT_H
#define IDRAWTOOLEVENT_H
#include <QPointF>
#include <QTouchEvent>

class CDrawScene;
class CGraphicsView;

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
                                         CDrawScene *scene, QTouchEvent *eOrg = nullptr);

    QPointF                pos(EPosType tp = EScenePos) const;
    Qt::MouseButtons       mouseButtons() const;
    Qt::KeyboardModifiers  keyboardModifiers() const;
    int                    uuid() const;
    EEventTp               eventType();
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

#endif // IDRAWTOOLEVENT_H
