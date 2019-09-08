#include "tipslabel.h"

TipsLabel::TipsLabel(DWidget *parent)
    : DLabel(parent)
{
    setObjectName("TipsLabel");
//    setStyleSheet("QLabel#TipsLabel{"
//                  "background-color: rgba(255, 255, 255, 153);"
//                  "border-radius: 3px;"
//                  "border: 1px solid rgba(0, 0, 0, 13);}");
    setAlignment(Qt::AlignCenter);

}

TipsLabel::~TipsLabel()
{
}
