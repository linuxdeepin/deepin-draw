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
#include "calphacontrolwidget.h"

#include <QHBoxLayout>

CAlphaControlWidget::CAlphaControlWidget(DWidget *parent)
    : DWidget(parent)
    , m_isUserOperation(false)
{
    initUI();
    initConnection();
}

void CAlphaControlWidget::updateAlphaControlWidget(int alpha)
{
    int percentValue = (float)alpha / 255 * 100;
    m_alphaSlider->setValue(percentValue);
}

void CAlphaControlWidget::initUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);



    DLabel *nameLabel = new DLabel(this);
    nameLabel->setText(tr("Alpha"));


    m_alphaLabel = new DLabel(this);
    m_alphaLabel->setFixedWidth(38);
    m_alphaLabel->setText("0%");


    m_alphaSlider = new DSlider(Qt::Horizontal, this);
    m_alphaSlider->setMinimum(0);
    m_alphaSlider->setMaximum(100);


    m_alphaSlider->setFixedWidth(130);

//    m_alphaSlider->setStyleSheet("background-color: rgb(0, 255, 0);");

    layout->addWidget(nameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_alphaSlider);
    layout->addSpacing(10);
    layout->addWidget(m_alphaLabel);

    setLayout(layout);

}

void CAlphaControlWidget::initConnection()
{
    connect(m_alphaSlider, &DSlider::valueChanged, this, [ = ](int value) {
        int trueValue =  (float)value * 255 / 100;
        m_alphaLabel->setText(QString("%1%").arg(value));
        if (m_isUserOperation) {
            emit signalAlphaChanged(trueValue);
        }
    });

    connect(m_alphaSlider, &DSlider::sliderPressed, this, [ = ]() {
        m_isUserOperation = true;
    });

    connect(m_alphaSlider, &DSlider::sliderReleased, this, [ = ]() {
        m_isUserOperation = false;
    });

    connect(m_alphaSlider, &DSlider::sliderReleased, this, &CAlphaControlWidget::signalFinishChanged);
}
