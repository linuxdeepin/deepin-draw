#include "cspinbox.h"

#include <QLineEdit>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>

#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

CSpinBox::CSpinBox(DWidget *parent)
    : DSpinBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

bool CSpinBox::isTimerRunning()
{
    if (_wheelTimer != nullptr) {
        return _wheelTimer->isActive();
    }
    return false;
}

bool CSpinBox::isChangedByWheelEnd()
{
    return _wheelEnd;
}

void CSpinBox::focusInEvent(QFocusEvent *event)
{
    //emit focusChanged(true);
    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr)
        CManageViewSigleton::GetInstance()->getCurView()->disableCutShortcut(true);
    DSpinBox::focusInEvent(event);
}

void CSpinBox::focusOutEvent(QFocusEvent *event)
{
    //emit focusChanged(false);
    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr)
        CManageViewSigleton::GetInstance()->getCurView()->disableCutShortcut(false);
    DSpinBox::focusOutEvent(event);
}

void CSpinBox::wheelEvent(QWheelEvent *event)
{
    //启动定时器
    timerStart();

    DSpinBox::wheelEvent(event);
}

void CSpinBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up ) {
        //启动定时器
        timerStart();
    }
    DSpinBox::keyPressEvent(event);
    event->accept();
    setFocus();
}

void CSpinBox::keyReleaseEvent(QKeyEvent *event)
{
    //timerEnd();
    DSpinBox::keyReleaseEvent(event);
    event->accept();
    setFocus();
}

void CSpinBox::timerEnd()
{
    //先结束timer(标志着isTimerRunning返回false)
    if (_wheelTimer != nullptr) {
        _wheelTimer->deleteLater();
        _wheelTimer = nullptr;
    }

    //wheel结束时的值变化标记
    _wheelEnd = true;
    emit this->valueChanged(value());
    emit this->valueChanged(text());
    _wheelEnd = false;
    setFocus();
    if (lineEdit() != nullptr) {
        lineEdit()->selectAll();
    }
}

QTimer *CSpinBox::getTimer()
{
    if (_wheelTimer == nullptr) {
        _wheelTimer = new QTimer(this);
        _wheelTimer->setSingleShot(true);
        connect(_wheelTimer, &QTimer::timeout, this, &CSpinBox::timerEnd);
    }
    return _wheelTimer;
}

void CSpinBox::timerStart()
{
    getTimer()->start(300);
}
