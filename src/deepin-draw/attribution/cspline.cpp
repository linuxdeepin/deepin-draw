#include "cspline.h"

CSpline::CSpline(QWidget *parent): QFrame(parent)
{
    this->setGeometry(QRect(300, 200, 118, 3));
    this->setFrameShape(QFrame::HLine);
    this->setFrameShadow(QFrame::Sunken);
}
