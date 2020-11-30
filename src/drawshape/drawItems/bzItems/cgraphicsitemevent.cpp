#include "cgraphicsitemevent.h"

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

bool CGraphItemEvent::isXTransBlocked()
{
    return _blockXTrans;
}

void CGraphItemEvent::setXTransBlocked(bool b)
{
    _blockXTrans = b;
    _transDirty = true;
}

bool CGraphItemEvent::isYTransBlocked()
{
    return _blockYTrans;
}

void CGraphItemEvent::setYTransBlocked(bool b)
{
    _blockYTrans = b;
    _transDirty = true;
}

bool CGraphItemEvent::isXNegtiveOffset()
{
    return _isXNegtiveOffset;
}

void CGraphItemEvent::setXNegtiveOffset(bool b)
{
    _isXNegtiveOffset = b;
    _transDirty = true;
}

bool CGraphItemEvent::isYNegtiveOffset()
{
    return _isYNegtiveOffset;
}

void CGraphItemEvent::setYNegtiveOffset(bool b)
{
    _isYNegtiveOffset = b;
    _transDirty = true;
}
#include <QDebug>
void CGraphItemEvent::updateTrans()
{
    if (!_orgSz.isValid() /*|| _centerPos.isNull()*/) {
        qDebug() << "_orgSz = " << _orgSz << "_centerPos = " << _centerPos;
        return;
    }

    QTransform trans;

    const QPointF centerPos = this->centerPos();

    const qreal offsetX = (_isXNegtiveOffset ? -1 : 1) * this->offset().x();
    const qreal offsetY = (_isYNegtiveOffset ? -1 : 1) * this->offset().y();

    qreal sX = _blockXTrans ? 1.0 : (offsetX + _orgSz.width()) / _orgSz.width();
    qreal sY = _blockYTrans ? 1.0 : (offsetY + _orgSz.height()) / _orgSz.height();

    if (qFuzzyIsNull(sX) || sX < 0) {
        sX  = 1;
    }
    if (qFuzzyIsNull(sY) || sY < 0) {
        sY  = 1;
    }

    trans.translate(centerPos.x(), centerPos.y());
    //trans.scale(qFuzzyIsNull(sX) ? 1.0 : sX, qFuzzyIsNull(sY) ? 1.0 : sY);
    trans.scale(sX, sY);
    trans.translate(-centerPos.x(), -centerPos.y());

    _trans = trans;

    _transDirty = false;
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

CGraphItemEvent CGraphItemEvent::transToEvent(const QTransform &tran, const QSizeF &newOrgSz)
{
    CGraphItemEvent event(this->type());
    event.setEventPhase(this->eventPhase());

    event.setPos(tran.map(this->pos()));
    event.setOldPos(tran.map(this->oldPos()));
    event.setBeginPos(tran.map(this->beginPos()));
    event.setCenterPos(tran.map(this->centerPos()));
    event.setOrgSize(newOrgSz);

    event._blockXTrans = this->_blockXTrans;
    event._blockYTrans = this->_blockYTrans;
    event._isXNegtiveOffset = this->_isXNegtiveOffset;
    event._isYNegtiveOffset = this->_isYNegtiveOffset;

    event.updateTrans();

    return event;
}
