#include "cvalidator.h"

CIntValidator::CIntValidator(int bottom, int top, QObject *parent)
    : QIntValidator(bottom, top, parent)
{

}


CIntValidator::CIntValidator(QObject *parent)
    : QIntValidator(parent)
{

}

CIntValidator::~CIntValidator()
{

}

QValidator::State CIntValidator::validate(QString &s, int &n) const
{
    return QIntValidator::validate(s, n);
}


void CIntValidator::fixup(QString &str) const
{
    str = QString("%1").arg(bottom());
}
