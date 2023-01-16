// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "calphacontrolwidget.h"
#include "application.h"

#include <QHBoxLayout>

CAlphaControlWidget::CAlphaControlWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

void CAlphaControlWidget::setAlpha(int alpha, bool internalChanged)
{
    if (!internalChanged) {
        m_alphaSlider->slider()->blockSignals(true);
        m_alphaSlider->blockSignals(true);
        this->blockSignals(true);
    }

    m_alphaSlider->setValue(alpha);

    if (!internalChanged) {
        m_alphaSlider->slider()->blockSignals(false);
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

    DLabel *nameLabel = new DLabel(this);

    nameLabel->setText(tr("Alpha"));

    QFont nameLabelFont = nameLabel->font();
    nameLabelFont.setPixelSize(13);
    nameLabel->setFixedSize(QSize(52, 36));
    nameLabel->setFont(nameLabelFont);

    m_alphaLabel = new DLineEdit(this);
    m_alphaLabel->setFixedSize(QSize(65, 36));
    m_alphaLabel->setClearButtonEnabled(false);
    m_alphaLabel->lineEdit()->setReadOnly(true);
    m_alphaLabel->setText("0%");
    QFont alphaLabelFont = m_alphaLabel->font();
    m_alphaLabel->lineEdit()->setTextMargins(0, 0, 0, 0);
    alphaLabelFont.setPixelSize(14);
    m_alphaLabel->setFont(alphaLabelFont);
//    m_alphaLabel->lineEdit()->setFocusPolicy(Qt::NoFocus);

    m_alphaSlider = new DSlider(Qt::Horizontal, this);
    setWgtAccesibleName(m_alphaSlider, "Color Alpha slider");
    //m_alphaSlider->setObjectName("AlphaSlider");
    m_alphaSlider->slider()->setFocusPolicy(Qt::NoFocus);
    m_alphaSlider->setMinimum(0);
    m_alphaSlider->setMaximum(255);

    m_alphaSlider->setFixedWidth(157);

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
