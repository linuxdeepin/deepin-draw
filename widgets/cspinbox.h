#ifndef CSPINBOX_H
#define CSPINBOX_H

#include <DWidget>
#include <DSpinBox>

DWIDGET_USE_NAMESPACE

class CSpinBox : public DSpinBox
{
    Q_OBJECT
public:
    CSpinBox(DWidget *parent = nullptr);
protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
signals:
    void focusChanged(bool isFocus);
};

#endif // CSPINBOX_H
