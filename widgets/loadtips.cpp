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
#include "loadtips.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>

#include "utils/global.h"

const QSize TIPS_SIZE = QSize(300, 300);

LoadTips::LoadTips(DWidget *parent)
    : DDialog(parent),
      m_counts(0)
{
    //DRAW_THEME_INIT_WIDGET("LoadTips");
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);
    DLabel *w = new DLabel(this);
    w->setObjectName("TipsLabel");
    w->setFixedSize(TIPS_SIZE);
    m_waterProg = new DWaterProgress(this);
    m_waterProg->setFixedSize(QSize(200, 200));

    m_messageLabel = new DLabel(this);
    m_messageLabel->setObjectName("MessageLabel");
    m_messageLabel->setText(tr("Importing pictures, please wait..."));

    QVBoxLayout *vLayout = new QVBoxLayout(w);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addStretch();
    vLayout->addWidget(m_waterProg, 0, Qt::AlignCenter);
    vLayout->addSpacing(20);
    vLayout->addWidget(m_messageLabel, 0, Qt::AlignCenter);
    vLayout->addStretch();

    connect(this, &LoadTips::progressValueChanged, this, [ = ](int value) {
        m_counts = value;
        m_waterProg->setValue(m_counts);
    });
    connect(this, &LoadTips::finishedPainting, this, [ = ] {
        this->hide();
    });
}

void LoadTips::showTips()
{
    this->show();
    QPoint gp = this->mapToGlobal(QPoint(0, 0));
    move((window()->width() - this->width()) / 2 + gp.x(),
         (window()->height() - this->height()) / 2 + gp.y());


    m_waterProg->setValue(m_counts);
    m_waterProg->start();

}

LoadTips::~LoadTips()
{
}
