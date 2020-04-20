#include "cspinbox.h"

#include <QLineEdit>

CSpinBox::CSpinBox(DWidget *parent)
    : DSpinBox(parent)
{
    QRegExp regExp("^[1-9][0-9]{1,8}$"); //^[1-9][0-9]*$ 任意位数正整数
    this->lineEdit()->setValidator(new QRegExpValidator(regExp, this));
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
