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
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <DSlider>
#include <QHBoxLayout>
#include <DGuiApplicationHelper>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 12;

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
    EBlurEffect effect = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurEffect();
    bool bEffect = (effect == BlurEffect);

    m_blurBtn->setChecked(bEffect);
    m_masicBtn->setChecked(!bEffect);

    m_pLineWidthSlider->blockSignals(true);
    m_pLineWidthSlider->setValue(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth());
    m_pLineWidthSlider->blockSignals(false);
    m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void BlurWidget::changeButtonTheme()
{
    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
    m_blurBtn->setCurrentTheme(themeType);
    m_masicBtn->setCurrentTheme(themeType);
}

void BlurWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
        case Blurtype: {
            m_blurBtn->setChecked(!propertys[property].toBool());
            m_masicBtn->setChecked(propertys[property].toBool());
            break;
        }
        case BlurWith: {
            m_pLineWidthSlider->blockSignals(true);
            m_pLineWidthSlider->setValue(propertys[property].toInt());
            m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
            m_pLineWidthSlider->blockSignals(false);
            break;
        }
        default:
            break;
        }
    }

}

void BlurWidget::initUI()
{
    DLabel *penLabel = new DLabel(this);
    penLabel->setObjectName("TypeLabel");
    //penLabel->setText(tr("类型"));

    penLabel->setText(tr("Type"));

    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
    penLabel->setFont(ft);


    QMap<int, QMap<CCheckButton::EButtonSattus, QString> > pictureMap;

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/attribute/fuzzy tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/attribute/fuzzy tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/attribute/fuzzy tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/attribute/fuzzy tool_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/attribute/fuzzy tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/attribute/fuzzy tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/attribute/fuzzy tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/attribute/fuzzy tool_checked.svg");

    m_blurBtn = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_blurBtn->setToolTip(tr("Blur"));
    m_actionButtons.append(m_blurBtn);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/attribute/smudge tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/attribute/smudge tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/attribute/smudge tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/attribute/smudge tool_checked.svg");


    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/attribute/smudge tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/attribute/smudge tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/attribute/smudge tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/attribute/smudge tool_checked.svg");

    m_masicBtn = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_masicBtn->setToolTip(tr("Mosaic"));
    m_actionButtons.append(m_masicBtn);

    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr) {
        EBlurEffect effect = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurEffect();
        bool bEffect = (effect == BlurEffect);

        m_blurBtn->setChecked(bEffect);
        m_masicBtn->setChecked(!bEffect);
    }


    DLabel *penWidthLabel = new DLabel(this);
    penWidthLabel->setObjectName("Width");
    //penWidthLabel->setText(tr("粗细"));

    penWidthLabel->setText(tr("Width"));

    penWidthLabel->setFont(ft);

    m_pLineWidthSlider = new DSlider(Qt::Horizontal, this);

    m_pLineWidthSlider->setMinimum(20);
    m_pLineWidthSlider->setMaximum(160);
    m_pLineWidthSlider->setFixedWidth(160);
    m_pLineWidthSlider->setMaximumHeight(24);


    m_pLineWidthLabel = new DLabel(this);
    m_pLineWidthLabel->setObjectName("Width Label");
    m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
    m_pLineWidthLabel->setFont(ft);
    m_pLineWidthLabel->setFixedWidth(60);

    connect(m_pLineWidthSlider, &DSlider::valueChanged, this, [ = ](int value) {
        m_pLineWidthLabel->setText(QString("%1px").arg(value));
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurWidth(value);
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
    layout->addSpacing(SEPARATE_SPACING + 8);
    layout->addWidget(penWidthLabel);
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
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurEffect(BlurEffect);
        emit signalBlurAttributeChanged();

    });

    connect(m_masicBtn, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_masicBtn);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurEffect(MasicoEffect);
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
