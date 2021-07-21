/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
