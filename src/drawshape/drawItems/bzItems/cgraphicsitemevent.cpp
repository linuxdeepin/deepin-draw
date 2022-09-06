// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cgraphicsitemevent.h"
#include <QDebug>

CGraphItemEvent::CGraphItemEvent(CGraphItemEvent::EItemType tp,
                                 const QPointF &oldPos,
                                 const QPointF &pos)
    : _tp(tp), _oldPos(oldPos), _pos(pos)
{

}

void CGraphItemEvent::setOldPos(const QPointF &pos)
{
    _oldPos = pos;
    _transDirty = true;
}

void CGraphItemEvent::setPos(const QPointF &pos)
{
    _pos = pos;
    _transDirty = true;
}

void CGraphItemEvent::setBeginPos(const QPointF &pos)
{
    _beginPos = pos;
    _transDirty = true;
}

void CGraphItemEvent::setCenterPos(const QPointF &pos)
{
    _centerPos = pos;
    _transDirty = true;
}

void CGraphItemEvent::setOrgSize(const QSizeF &sz)
{
    _orgSz = sz;
    _transDirty = true;
}

void CGraphItemEvent::setEventPhase(EChangedPhase ph)
{
    _phase = ph;
}

// Cppcheck检测函数没有使用到
//void CGraphItemEvent::setAccept(bool b)
//{
//    _accept = b;
//}

//void CGraphItemEvent::setPosXAccept(bool b)
//{
//    _acceptPosX = b;
//}

//void CGraphItemEvent::setPosYAccept(bool b)
//{
//    _acceptPosY = b;
//}

//CGraphItemEvent *CGraphItemEvent::driverEvent()
//{
//    return _driverEvent;
//}

void CGraphItemEvent::setDriverEvent(CGraphItemEvent *event)
{
//    if (event != nullptr) {
//        qWarning() << "event======================== " << event->type();
//    }
    _driverEvent = event;
}

void CGraphItemEvent::setItem(CGraphicsItem *pItem)
{
    _pItem = pItem;
}

CGraphicsItem *CGraphItemEvent::item()
{
    return _pItem;
}

void CGraphItemEvent::updateTrans()
{
    QTransform trans;
    if (reCalTransform(trans)) {
        _trans = trans;
    }
    _transDirty = false;
}

CGraphItemEvent *CGraphItemEvent::creatTransDuplicate(const QTransform &tran, const QSizeF &newOrgSz)
{
    auto p = newInstace();
    p->setOrgSize(newOrgSz);
    p->transAllPosTo(tran);
    return p;
}

CGraphItemEvent *CGraphItemEvent::newInstace()
{
    auto p = new CGraphItemEvent;
    *p = *this;
    return p;
}

QTransform CGraphItemEvent::trans()
{
    if (_transDirty) {
        updateTrans();
    }
    return _trans;
}

void CGraphItemEvent::setTrans(const QTransform &trans)
{
    _trans = trans;
    _transDirty = false;
}

//CGraphItemEvent *CGraphItemEvent::transToEvent(const QTransform &tran, const QSizeF &newOrgSz)
//{
//    CGraphItemEvent event = (this->type());
//    event.setEventPhase(this->eventPhase());

//    event.setPos(tran.map(this->pos()));
//    event.setOldPos(tran.map(this->oldPos()));
//    event.setBeginPos(tran.map(this->beginPos()));
//    event.setCenterPos(tran.map(this->centerPos()));
//    event.setOrgSize(newOrgSz);


//    event._blockXTrans = this->_blockXTrans;
//    event._blockYTrans = this->_blockYTrans;
//    event._isXNegtiveOffset = this->_isXNegtiveOffset;
//    event._isYNegtiveOffset = this->_isYNegtiveOffset;

//    event._pressedDirection = this->_pressedDirection;
//    event._orgToolEventTp = this->_orgToolEventTp;

//    event._oldScenePos = this->_oldScenePos;
//    event._scenePos = this->_scenePos;
//    event._sceneBeginPos = this->_sceneBeginPos;
//    event._sceneCenterPos = this->_sceneCenterPos;

//    event.updateTrans();

//    return event;
//}

void CGraphItemEvent::transAllPosTo(const QTransform &tran)
{
    this->setPos(tran.map(this->pos()));
    this->setOldPos(tran.map(this->oldPos()));
    this->setBeginPos(tran.map(this->beginPos()));
    this->setCenterPos(tran.map(this->centerPos()));
    this->updateTrans();
}

bool CGraphItemEvent::reCalTransform(QTransform &outTrans)
{
    Q_UNUSED(outTrans);
    return false;
}


CGraphItemEvent *CGraphItemMoveEvent::newInstace()
{
    auto p = new CGraphItemMoveEvent;
    *p = *this;
    return p;
}

bool CGraphItemMoveEvent::reCalTransform(QTransform &outTrans)
{
    //仅仅是一个平移
    QPointF move = this->pos() - this->oldPos();
    outTrans = QTransform::fromTranslate(move.x(), move.y());
    return true;
}

// Cppcheck检测函数没有使用到
//bool CGraphItemScalEvent::isXTransBlocked() const
//{
//    return _blockXTrans;
//}

void CGraphItemScalEvent::setXTransBlocked(bool b)
{
    _blockXTrans = b;
    _transDirty = true;
}

// Cppcheck检测函数没有使用到
//bool CGraphItemScalEvent::isYTransBlocked() const
//{
//    return _blockYTrans;
//}

void CGraphItemScalEvent::setYTransBlocked(bool b)
{
    _blockYTrans = b;
    _transDirty = true;
}

// Cppcheck检测函数没有使用到
//bool CGraphItemScalEvent::isXNegtiveOffset() const
//{
//    return _isXNegtiveOffset;
//}

void CGraphItemScalEvent::setXNegtiveOffset(bool b)
{
    _isXNegtiveOffset = b;
    _transDirty = true;
}

// Cppcheck检测函数没有使用到
//bool CGraphItemScalEvent::isYNegtiveOffset() const
//{
//    return _isYNegtiveOffset;
//}

void CGraphItemScalEvent::setYNegtiveOffset(bool b)
{
    _isYNegtiveOffset = b;
    _transDirty = true;
}

// Cppcheck检测函数没有使用到
//bool CGraphItemScalEvent::isKeepOrgRadio() const
//{
//    return _isKeepOrgRadio;
//}

void CGraphItemScalEvent::setKeepOrgRadio(bool b)
{
    _isKeepOrgRadio = b;
    _transDirty = true;
}

void CGraphItemScalEvent::setMayResultPolygon(const QPolygonF rect)
{
    _driverMayResultRect = rect;
}

//QPolygonF CGraphItemScalEvent::mayResultPolygon()
//{
//    return _driverMayResultRect;
//}

CGraphItemEvent *CGraphItemScalEvent::newInstace()
{
    auto p = new CGraphItemScalEvent;
    *p = *this;
    return p;
}

bool CGraphItemScalEvent::reCalTransform(QTransform &outTrans)
{
    QTransform trans;

    if (!_orgSz.isValid()) {
        return false;
    }

    const QPointF centerPos = this->centerPos();

    //qWarning() << "current pos = " << this->pos() << "old pos = " << this->oldPos();
    const qreal offsetX = (_isXNegtiveOffset ? -1 : 1) * this->offset().x();
    const qreal offsetY = (_isYNegtiveOffset ? -1 : 1) * this->offset().y();

    qreal sX = _blockXTrans ? 1.0 : (offsetX + _orgSz.width()) / _orgSz.width();
    qreal sY = _blockYTrans ? 1.0 : (offsetY + _orgSz.height()) / _orgSz.height();

    const qreal minW = 1.0;
    if (_orgSz.width() * sX < minW) {
        sX = minW / _orgSz.width();
        _acceptPosX = false;
    }
    const qreal minH = 1.0;
    if (_orgSz.height() * sY < minH) {
        sY = minH / _orgSz.height();
        _acceptPosY = false;
    }

    if (_isKeepOrgRadio) {
        if (qAbs(offsetY) > qAbs(offsetX)) {
            sX = sY;
        } else {
            sY = sX;
        }
    }

    trans.translate(centerPos.x(), centerPos.y());
    trans.scale(sX, sY);
    trans.translate(-centerPos.x(), -centerPos.y());

    outTrans = trans;

    return true;
}

CGraphItemEvent *CGraphItemRotEvent::newInstace()
{
    auto p = new CGraphItemRotEvent;
    *p = *this;
    return p;
}

bool CGraphItemRotEvent::reCalTransform(QTransform &outTrans)
{
    auto prePos = oldPos();
    auto pos    = this->pos();
    auto center = this->centerPos();

    QLineF l1 = QLineF(center, prePos);
    QLineF l2 = QLineF(center, pos);
    qreal angle = l2.angle() - l1.angle();
    QTransform trans;
    trans.translate(center.x(), center.y());
    trans.rotate(-angle);
    trans.translate(-center.x(), -center.y());
    outTrans = trans;
    return true;
}
