/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#include "editlabel.h"

#include <QHBoxLayout>
#include <QFontMetrics>
#include <QDebug>

#include "utils/global.h"

const QSize LINEEDIT_SIZE = QSize(55, 36);

EditLabel::EditLabel(DWidget *parent)
    : DLineEdit(parent)
{
    this->setFixedSize(LINEEDIT_SIZE);
    this->setClearButtonEnabled(false);
    this->lineEdit()->setReadOnly(true);

    QFont font = this->font();
    font.setPixelSize(14);
    this->setFont(font);
}


EditLabel::~EditLabel() {}
