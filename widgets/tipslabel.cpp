/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
