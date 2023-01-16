// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
