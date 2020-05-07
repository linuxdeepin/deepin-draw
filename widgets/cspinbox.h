#ifndef CSPINBOX_H
#define CSPINBOX_H

#include <DWidget>
#include <DSpinBox>

DWIDGET_USE_NAMESPACE

class QTimer;

class CSpinBox : public DSpinBox
{
    Q_OBJECT
public:
    CSpinBox(DWidget *parent = nullptr);

    bool isTimerRunning();

    bool isChangedByWheelEnd();

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
signals:
    void focusChanged(bool isFocus);

private:
    QTimer *getTimer();

private:
    QTimer *_wheelTimer = nullptr;
    bool    _wheelbegin = false;
    bool    _wheelEnd   = false;
};

#endif // CSPINBOX_H
