#ifndef CSPINBOX_H
#define CSPINBOX_H

#include <DWidget>
#include <DSpinBox>
#include "globaldefine.h"

DWIDGET_USE_NAMESPACE

class QTimer;

class CSpinBox : public DSpinBox
{
    Q_OBJECT
public:
    CSpinBox(DWidget *parent = nullptr);

    bool isTimerRunning();

    bool isChangedByWheelEnd();

    void setValueChangedKeepFocus(bool b);

    void setSpinRange(int min, int max);

    void setSpecialText(QString sp = "— —");

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event)override;
    void keyReleaseEvent(QKeyEvent *event)override;
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

    Q_SLOT void timerStart();
    Q_SLOT void timerEnd();

signals:
    void focusChanged(bool isFocus);

    void valueChanged(int value, EChangedPhase phase);

private:
    QTimer *getTimer();

    void setSpinPhaseValue(int value, EChangedPhase phase);

private:
    QTimer *_wheelTimer = nullptr;
    bool    _wheelbegin = false;
    bool    _wheelEnd   = false;
    bool    _keepFocus = false;

    int _s_value = -100;
    int _s_phase = -100;

    int m_min = -1;
    int m_max = INT_MAX;
};

#endif // CSPINBOX_H
