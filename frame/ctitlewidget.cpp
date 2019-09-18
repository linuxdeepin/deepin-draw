/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "ctitlewidget.h"
#include "drawshape/cdrawparamsigleton.h"

#include <QHBoxLayout>


CTitleWidget::CTitleWidget(DWidget *parent)
    : DWidget(parent)
{
    m_title = new DLabel(this);

    m_title->setText(tr("未命名画板"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_title);
    layout->addStretch();

    setLayout(layout);
}

CTitleWidget::~CTitleWidget()
{

}

void CTitleWidget::updateTitleWidget()
{
    QString path = CDrawParamSigleton::GetInstance()->getDdfSavePath();
    if (path.isEmpty()) {
        m_title->setText(tr("未命名画板"));
    } else {

    }
}

