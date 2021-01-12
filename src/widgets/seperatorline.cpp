#include "seperatorline.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"

SeperatorLine::SeperatorLine(DWidget *parent)
    : DLabel(parent)
{

    setFixedSize(12, 24);
    QPixmap seprtatorLine;

    updateTheme();
}

SeperatorLine::~SeperatorLine()
{

}

void SeperatorLine::updateTheme()
{
    QPixmap seprtatorLine;
    if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
        seprtatorLine = QPixmap(QString(":/theme/common/line.svg"));
    } else {
        //seprtatorLine = QPixmap(QString(":/theme/common/linedark.svg"));
        seprtatorLine = QPixmap(QString(":/theme/common/line.svg"));
    }
    this->setPixmap(seprtatorLine);

}
