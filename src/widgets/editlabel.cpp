// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "editlabel.h"

#include "drawshape/globaldefine.h"

#include <QHBoxLayout>
#include <QFontMetrics>
#include <QDebug>

#include "utils/global.h"

const QSize LINEEDIT_SIZE = QSize(55, 36);

EditLabel::EditLabel(DWidget *parent)
    : DLineEdit(parent)
{
    setWgtAccesibleName(this, "EditLabel");
    this->setFixedSize(LINEEDIT_SIZE);
    this->setClearButtonEnabled(false);
    this->lineEdit()->setReadOnly(true);

    QFont font = this->font();
    font.setPixelSize(14);
    this->setFont(font);
}


EditLabel::~EditLabel() {}
