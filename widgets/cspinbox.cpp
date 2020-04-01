#include "cspinbox.h"

CSpinBox::CSpinBox(DWidget *parent)
    : DSpinBox(parent)
{

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
