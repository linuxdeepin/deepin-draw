#include "seperatorline.h"

SeperatorLine::SeperatorLine(DWidget *parent)
    : DLabel(parent)
{

    setFixedSize(6, 24);
    QPixmap seprtatorLine;

    updateTheme();
}

SeperatorLine::~SeperatorLine()
{

}

void SeperatorLine::updateTheme()
{
    QPixmap seprtatorLine;
    if (CDrawParamSigleton::GetInstance()->getThemeType() == 1) {
        seprtatorLine = QPixmap(QString(":/theme/common/line.svg"));
    } else {
        seprtatorLine = QPixmap(QString(":/theme/common/linedark.svg"));
    }
    this->setPixmap(seprtatorLine);

}
