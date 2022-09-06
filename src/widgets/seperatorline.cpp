// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "seperatorline.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include <DGuiApplicationHelper>

SeperatorLine::SeperatorLine(DWidget *parent)
    : DLabel(parent)
{
    setFixedSize(12, 24);

    updateTheme();

    //绑定主题发生变化的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
            this, &SeperatorLine::updateTheme);

    setAlignment(Qt::AlignCenter);

    this->setProperty(WidgetShowInVerWindow, false);
}

SeperatorLine::~SeperatorLine()
{

}

void SeperatorLine::updateTheme()
{
    QPixmap seprtatorLine;
    if (DGuiApplicationHelper::instance()->themeType() == 1) {
        seprtatorLine = QPixmap(QString(":/theme/common/line.svg"));
    } else {
        seprtatorLine = QPixmap(QString(":/theme/common/linedark.svg"));
    }
    this->setPixmap(seprtatorLine);
}
