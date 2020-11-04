/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     JiangChangli
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
#include "progresslayout.h"
#include "frame/cviewmanagement.h"
#include <DPalette>
#include <DApplicationHelper>
#include <QObject>
DGUI_USE_NAMESPACE

ProgressLayout::ProgressLayout(QWidget *parent)
    : DBlurEffectWidget(parent)
{
    m_progressVBoxLayout = new QVBoxLayout();
    m_label = new DLabel();
    m_label->setFixedWidth(400);

    m_label->setText(QObject::tr("Importing pictures, please wait..."));

    QFont ft;
    ft.setPixelSize(16);
//    ft.setFamily(QString("SourceHanSansSC-Medium"));
    m_label->setFont(ft);

    m_progressLabel = new DLabel();
    m_progressLabel->setFixedWidth(400);
    m_progressbar = new DProgressBar();
    m_progressbar->setTextVisible(false);
    m_progressbar->setFixedSize(400, 6);
    m_progressVBoxLayout->addWidget(m_label, 40, Qt::AlignCenter);
    m_progressVBoxLayout->addSpacing(1);
    m_progressVBoxLayout->addWidget(m_progressLabel, 40, Qt::AlignCenter);
    m_progressVBoxLayout->addSpacing(1);
    m_progressVBoxLayout->addWidget(m_progressbar, 40, Qt::AlignCenter);
    this->setFixedSize(480, 100);

    this->setLayout(m_progressVBoxLayout);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setBlurEnabled(true);
    this->setMaskAlpha(int(255 * 0.8));
}

ProgressLayout::~ProgressLayout()
{

}
void ProgressLayout::showInCenter(DWidget *w)
{
    show();

    QPoint gp = w->mapToGlobal(QPoint(0, 0));
    move((w->width() - this->width()) / 2 + gp.x(),
         (w->height() - this->sizeHint().height()) / 2 + gp.y());
}

void ProgressLayout::setRange(int start, int end)
{
    m_start = start;
    m_end = end;
    m_progressbar->setMinimum(start);
    m_progressbar->setMaximum(end);
    m_progressbar->setValue(0);
}

void ProgressLayout::setProgressValue(int value)
{
    //设置字号
    QFont ft2;
    ft2.setPixelSize(14);
//    ft2.setFamily(QString("SourceHanSansSC-Bold"));
    m_progressLabel->setFont(ft2);

    //设置颜色
    DPalette textcolor;
    if (CManageViewSigleton::GetInstance()->getThemeType() == 1) {
        this->setMaskColor(QColor("#F7F7F7"));
        textcolor.setColor(DPalette::TextTitle, QColor(0, 0, 0));
        m_label->setPalette(textcolor);
        m_label->setForegroundRole(DPalette::TextTitle);
    } else {
        this->setMaskColor(QColor("#191919"));
        textcolor.setColor(DPalette::TextLively, QColor(255, 255, 255));
        m_label->setPalette(textcolor);
        m_label->setForegroundRole(DPalette::TextLively);
    }

    m_progressbar->setValue(value);
    //已导入%1/%2张
    m_progressLabel->setText(QObject::tr("%1/%2 pictures imported").arg(value).arg(m_end));

}


