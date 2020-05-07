#include "cspinbox.h"

#include <QLineEdit>
#include <QTimer>

CSpinBox::CSpinBox(DWidget *parent)
    : DSpinBox(parent)
{
    QRegExp regExp("^[1-9][0-9]{1,8}$"); //^[1-9][0-9]*$ 任意位数正整数
    this->lineEdit()->setValidator(new QRegExpValidator(regExp, this));
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
    emit focusChanged(true);
    DSpinBox::focusInEvent(event);
}

void CSpinBox::focusOutEvent(QFocusEvent *event)
{
    emit focusChanged(false);
    DSpinBox::focusOutEvent(event);
}

void CSpinBox::wheelEvent(QWheelEvent *event)
{
    //启动定时器
    getTimer()->start(300);

    DSpinBox::wheelEvent(event);
}

QTimer *CSpinBox::getTimer()
{
    if (_wheelTimer == nullptr) {
        _wheelTimer = new QTimer(this);
        _wheelTimer->setSingleShot(true);
        connect(_wheelTimer, &QTimer::timeout, this, [ = ]() {

            //先结束timer(标志着isTimerRunning返回false)
            _wheelTimer->deleteLater();
            _wheelTimer = nullptr;

            //wheel结束时的值变化标记
            _wheelEnd = true;
            emit this->valueChanged(value());
            emit this->valueChanged(text());
            _wheelEnd = false;
        });
    }
    return _wheelTimer;
}
