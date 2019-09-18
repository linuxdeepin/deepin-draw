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

const QSize LINEEDIT_SIZE = QSize(37, 24);

EditLabel::EditLabel(DWidget *parent)
    : DLabel(parent)
    , m_titleSpacing(1)
{
    m_titleLabel = new DLabel(this);

    m_edit = new DLineEdit(this);
    m_edit->setFixedSize(LINEEDIT_SIZE);
    m_edit->setClearButtonEnabled(false);

    QFont font = m_edit->font();
    font.setPixelSize(8);
    m_edit->setFont(font);


    QHBoxLayout *mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addWidget(m_titleLabel);
    mLayout->addWidget(m_edit);

    connect(m_edit, &DLineEdit::editingFinished, this, [ = ] {
        emit editTextChanged(m_edit->text());
    });

    setLayout(mLayout);
}

void EditLabel::setTitle(QString title)
{
    m_titleLabel->setText(title);
    QFont font = m_titleLabel->font();
    QFontMetrics fm(font);
    m_titleLabel->setFixedWidth(fm.boundingRect(m_titleLabel->text()).width());
}

void EditLabel::setEditText(QString text)
{
    m_edit->setText(text);
}

void EditLabel::setEditWidth(int width)
{
    m_edit->setFixedWidth(width);
    this->updateGeometry();
}

void EditLabel::setTitleSpacing(int spacing)
{
    m_titleSpacing = spacing;
    this->updateGeometry();
}

QString EditLabel::editText()
{
    return m_edit->text();
}

EditLabel::~EditLabel() {}
