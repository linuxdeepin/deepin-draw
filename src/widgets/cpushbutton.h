#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <DPushButton>
#include <QString>

DWIDGET_USE_NAMESPACE

class CPushButton : public DPushButton
{
public:
    explicit CPushButton(const QString &text, QWidget *parent = nullptr);

protected:
    virtual void enterEvent(QEvent *e) override;
    virtual void leaveEvent(QEvent *e) override;
};

#endif // CPUSHBUTTON_H
