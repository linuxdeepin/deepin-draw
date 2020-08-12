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

    connect(this, QOverload<int>::of(&DSpinBox::valueChanged), this, [ = ](int value) {
        setSpinPhaseValue(value, isTimerRunning() ? EChangedUpdate : EChanged);
    });

    connect(this, QOverload<int>::of(&DSpinBox::valueChanged), this, [ = ](int value) {
        Q_UNUSED(value);
        if (_keepFocus)
            this->setFocus();
    },
    Qt::QueuedConnection);

    setValueChangedKeepFocus(true);

    setKeyboardTracking(false);

    setRange(-INT_MAX, INT_MAX);
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

void CSpinBox::setValueChangedKeepFocus(bool b)
{
    _keepFocus = b;
}

void CSpinBox::setSpinRange(int min, int max)
{
    m_min = min;
    m_max = max;
}

void CSpinBox::setSpecialText(QString sp)
{
    this->blockSignals(true);
    this->lineEdit()->setText(sp);
    this->blockSignals(false);
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
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up) {
        //启动定时器
        timerStart();
    } else if (event->key() == Qt::Key_Minus) {
        //不允许负数值设置
        return;
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

void CSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
    // [41555] spinbox 触摸长按会弹出菜单导致焦点丢失，此处截获弹出菜单事件
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
    //emit this->valueChanged(value(), EChangedFinished);
    setSpinPhaseValue(value(), EChangedFinished);
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

void CSpinBox::setSpinPhaseValue(int value, EChangedPhase phase)
{
    if (_s_value != value || _s_phase != phase) {
        _s_value = value;
        _s_phase = phase;

        if (_s_value < m_min) {
            _s_value = m_min;
            this->blockSignals(true);
            setValue(_s_value);
            this->blockSignals(false);
        } else if (value > m_max) {
            _s_value = m_max;
            this->blockSignals(true);
            setValue(_s_value);
            this->blockSignals(false);
        }
        emit valueChanged(_s_value, EChangedPhase(_s_phase));
    }
}

void CSpinBox::timerStart()
{
    if (!isTimerRunning()) {
        //emit this->valueChanged(value(), EChangedBegin);
        setSpinPhaseValue(value(), EChangedBegin);
    }
    getTimer()->start(300);
}
