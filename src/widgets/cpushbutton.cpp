#include "cpushbutton.h"

CPushButton::CPushButton(const QString &text, QWidget *parent)
    : DPushButton (text, parent)
{

}

void CPushButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
    this->setFlat(false);
}

void CPushButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
    this->setFlat(true);
}
