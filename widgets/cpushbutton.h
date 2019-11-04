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
    virtual void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
};

#endif // CPUSHBUTTON_H
