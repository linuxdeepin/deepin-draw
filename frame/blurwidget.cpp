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
#include "blurwidget.h"
#include "widgets/toolbutton.h"
#include "widgets/ccheckbutton.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/globaldefine.h"

#include <DLabel>
#include <DSlider>
#include <QHBoxLayout>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

BlurWidget::BlurWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

BlurWidget::~BlurWidget()
{
}

void BlurWidget::updateBlurWidget()
{
    EBlurEffect effect = CDrawParamSigleton::GetInstance()->getBlurEffect();
    bool bEffect = (effect == BlurEffect);

    m_blurBtn->setChecked(bEffect);
    m_masicBtn->setChecked(!bEffect);

    m_pLineWidthSlider->setValue(CDrawParamSigleton::GetInstance()->getBlurWidth());
    m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
}

void BlurWidget::initUI()
{
    DLabel *penLabel = new DLabel(this);
    penLabel->setObjectName("WidthLabel");
    penLabel->setText(tr("类型"));


    QMap<CCheckButton::EButtonSattus, QString> pictureMap;

    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/attribute/fuzzy tool_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/attribute/fuzzy tool_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/attribute/fuzzy tool_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/attribute/fuzzy tool_checked.svg");
    m_blurBtn = new CCheckButton(pictureMap, this);
    m_actionButtons.append(m_blurBtn);


    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/attribute/smudge tool_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/attribute/smudge tool_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/attribute/smudge tool_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/attribute/smudge tool_checked.svg");
    m_masicBtn = new CCheckButton(pictureMap, this);
    m_actionButtons.append(m_masicBtn);

    EBlurEffect effect = CDrawParamSigleton::GetInstance()->getBlurEffect();
    bool bEffect = (effect == BlurEffect);

    m_blurBtn->setChecked(bEffect);
    m_masicBtn->setChecked(!bEffect);

    m_pLineWidthSlider = new DSlider(Qt::Horizontal, this);

    m_pLineWidthSlider->setMinimum(20);
    m_pLineWidthSlider->setMaximum(160);
    m_pLineWidthSlider->setFixedWidth(200);
    m_pLineWidthSlider->setMaximumHeight(24);


    m_pLineWidthLabel = new DLabel(this);
    m_pLineWidthLabel->setObjectName("WidthLabel");
    m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));

    connect(m_pLineWidthSlider, &DSlider::valueChanged, this, [ = ](int value) {
        m_pLineWidthLabel->setText(QString("%1px").arg(value));
        CDrawParamSigleton::GetInstance()->setBlurWidth(value);
        emit signalBlurAttributeChanged();
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(penLabel);
    layout->addWidget(m_blurBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_masicBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_pLineWidthSlider);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_pLineWidthLabel);
    layout->addStretch();
    setLayout(layout);
}

void BlurWidget::initConnection()
{
    connect(m_blurBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_blurBtn);
        CDrawParamSigleton::GetInstance()->setBlurEffect(BlurEffect);
        emit signalBlurAttributeChanged();

    });

    connect(m_masicBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_masicBtn);
        CDrawParamSigleton::GetInstance()->setBlurEffect(MasicoEffect);
        emit signalBlurAttributeChanged();

    });
}

void BlurWidget::clearOtherSelections(CCheckButton *clickedButton)
{
    foreach (CCheckButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}
