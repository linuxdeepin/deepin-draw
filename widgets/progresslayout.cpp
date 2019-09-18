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
#include <DPalette>
//#include <DApplicationHelper>
DGUI_USE_NAMESPACE

ProgressLayout::ProgressLayout(DBlurEffectWidget *parent)
    : DBlurEffectWidget (parent)
{
    m_progressVBoxLayout = new QVBoxLayout();
    m_label = new DLabel();
    m_label->setFixedWidth(400);
    m_label->setText(QString("正在导入图片，请稍候"));
    QFont ft;
    ft.setPixelSize(16);
    ft.setFamily(QString("SourceHanSansSC-Medium"));
    m_label->setFont(ft);
    //设置颜色
    DPalette pa1;
    pa1.setColor(DPalette::Text, QColor("#001a2e"));
    m_label->setPalette(pa1);

    m_progressLabel = new DLabel();
    m_progressLabel->setFixedWidth(400);
    m_progressbar = new DProgressBar();
    m_progressbar->setTextVisible(false);
    m_progressbar->setFixedSize(400, 6);
    //m_progressbar->setTextVisiable(false);
    m_progressVBoxLayout->addWidget(m_label, 40, Qt::AlignCenter);
    m_progressVBoxLayout->addSpacing(1);
    m_progressVBoxLayout->addWidget(m_progressLabel, 40, Qt::AlignCenter);
    m_progressVBoxLayout->addSpacing(1);
    m_progressVBoxLayout->addWidget(m_progressbar, 40, Qt::AlignCenter);
    this->setFixedSize(480, 110);

    this->setLayout(m_progressVBoxLayout);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //this->setAttribute(Qt::WA_TranslucentBackground);
    //setWindowOpacity(0.5);
    this->setBlurEnabled(true);

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
    m_progressbar->setValue(value);
    m_progressLabel->setText(QString::fromLocal8Bit("已导入%1/%2张").arg(value).arg(m_end));
    //设置字号
    QFont ft2;
    ft2.setPixelSize(14);
    ft2.setFamily(QString("SourceHanSansSC-Bold"));
    m_progressLabel->setFont(ft2);

    //设置颜色
    DPalette pa;
    pa.setColor(DPalette::WindowText, QColor("#6a829f"));
    m_progressLabel->setPalette(pa);


}


