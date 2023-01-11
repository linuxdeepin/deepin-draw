/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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

CAlphaControlWidget::CAlphaControlWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnection();
}

void CAlphaControlWidget::setAlpha(int alpha, bool internalChanged)
{
    if (!internalChanged) {
#ifdef USE_DTK
        m_alphaSlider->slider()->blockSignals(true);
#endif
        m_alphaSlider->blockSignals(true);
        this->blockSignals(true);
    }

    m_alphaSlider->setValue(alpha);

    if (!internalChanged) {
#ifdef USE_DTK
        m_alphaSlider->slider()->blockSignals(false);
#endif
        m_alphaSlider->blockSignals(false);
        this->blockSignals(false);
    }

    m_alphaLabel->setText(QString("%1%").arg(int(alpha * 100 / 255)));
}

int CAlphaControlWidget::alpha()
{
    return m_alphaSlider->value();
}

void CAlphaControlWidget::initUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    QLabel *nameLabel = new QLabel(this);

    nameLabel->setText(tr("Alpha"));

    QFont nameLabelFont = nameLabel->font();
    nameLabelFont.setPixelSize(13);
    nameLabel->setFixedSize(QSize(52, 36));
    nameLabel->setFont(nameLabelFont);

    m_alphaLabel = new DLineEdit(this);
    m_alphaLabel->setFixedSize(QSize(65, 36));
    m_alphaLabel->setClearButtonEnabled(false);
    QLineEdit *lineedit = nullptr;
#ifdef USE_DTK
    lineedit = m_alphaLabel->lineEdit();
#else
    lineedit = m_alphaLabel;
#endif
    lineedit->setReadOnly(true);
    m_alphaLabel->setText("0%");
    QFont alphaLabelFont = m_alphaLabel->font();
    lineedit->setTextMargins(0, 0, 0, 0);
    alphaLabelFont.setPixelSize(14);
    m_alphaLabel->setFont(alphaLabelFont);

    m_alphaSlider = new DSlider(Qt::Horizontal, this);
    setWgtAccesibleName(m_alphaSlider, "Color Alpha slider");
#ifdef USE_DTK
    m_alphaSlider->slider()->setFocusPolicy(Qt::NoFocus);
#endif
    m_alphaSlider->setMinimum(0);
    m_alphaSlider->setMaximum(255);

    //m_alphaSlider->setFixedWidth(157);

    layout->addWidget(nameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_alphaSlider);
    layout->addSpacing(10);
    layout->addWidget(m_alphaLabel);

    setLayout(layout);
}

void CAlphaControlWidget::initConnection()
{
    connect(m_alphaSlider, &DSlider::sliderPressed, this, [ = ]() {
        emit alphaChanged(m_alphaSlider->value(), EChangedBegin);
    });

    connect(m_alphaSlider, &DSlider::valueChanged, this, [ = ](int value) {
        value = m_alphaSlider->value();
        m_alphaLabel->setText(QString("%1%").arg(value * 100 / 255));
        emit alphaChanged(value, EChangedUpdate);
    });

    connect(m_alphaSlider, &DSlider::sliderReleased, this, [ = ]() {
        emit alphaChanged(m_alphaSlider->value(), EChangedFinished);
    });
}
