#include "seperatorline.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include <DGuiApplicationHelper>

SeperatorLine::SeperatorLine(DWidget *parent)
    : DLabel(parent)
{
    setFixedSize(12, 24);
    QPixmap seprtatorLine;

    updateTheme();

    //绑定主题发生变化的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
            this, &SeperatorLine::updateTheme);
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
        seprtatorLine = QPixmap(QString(":/theme/common/linedark.svg"));
    }
    this->setPixmap(seprtatorLine);
}
