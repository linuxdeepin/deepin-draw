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
#include "csidewidthwidget.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "ccheckbutton.h"

#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

CSideWidthWidget::CSideWidthWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}


void CSideWidthWidget::initUI()
{
    m_layout = new QHBoxLayout (this);
    m_layout->setMargin(0);
    m_layout->setSpacing(8);

    QMap<int, QMap<CCheckButton::EButtonSattus, QString> > pictureMap;

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness01_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness01_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/draw/tickness01_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/draw/tickness01_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/draw/tickness01_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/draw/tickness01_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/draw/tickness01_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/draw/tickness01_checked.svg");

    m_finerButton = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_buttonMap.insert(m_finerButton, Finer);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness02_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness02_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/draw/tickness02_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/draw/tickness02_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/draw/tickness02_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/draw/tickness02_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/draw/tickness02_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/draw/tickness02_checked.svg");

    m_fineButton = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_buttonMap.insert(m_fineButton, Fine);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness03_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness03_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/draw/tickness03_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/draw/tickness03_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/draw/tickness03_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/draw/tickness03_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/draw/tickness03_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/draw/tickness03_checked.svg");

    m_mediumButton = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_buttonMap.insert(m_mediumButton, Medium);

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness04_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness04_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/draw/tickness04_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/draw/tickness04_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/draw/tickness04_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/draw/tickness04_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/draw/tickness04_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/draw/tickness04_checked.svg");

    m_boldButton = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_buttonMap.insert(m_boldButton, Bold);

    m_layout->addWidget(m_finerButton);
    m_layout->addWidget(m_fineButton);
    m_layout->addWidget(m_mediumButton);
    m_layout->addWidget(m_boldButton);

    setLayout(m_layout);
}

void CSideWidthWidget::initConnection()
{
    connect(m_finerButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_finerButton);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineWidth(m_buttonMap.value(m_finerButton));
        emit signalSideWidthChange();
    });

    connect(m_fineButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_fineButton);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineWidth(m_buttonMap.value(m_fineButton));
        emit signalSideWidthChange();
    });

    connect(m_mediumButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_mediumButton);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineWidth(m_buttonMap.value(m_mediumButton));
        emit signalSideWidthChange();
    });

    connect(m_boldButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_boldButton);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineWidth(m_buttonMap.value(m_boldButton));
        emit signalSideWidthChange();
    });

}


void CSideWidthWidget::clearOtherSelections(CCheckButton *clickedButton)
{
    foreach (CCheckButton *button, m_buttonMap.keys()) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}

void CSideWidthWidget::updateSideWidth()
{
    int sideWidth = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineWidth();
    QMapIterator<CCheckButton *, CLineWidth> i(m_buttonMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == sideWidth) {
            if (!i.key()->isChecked()) {
                i.key()->setChecked(true);
            }
        } else {
            i.key()->setChecked(false);
        }
    }
}

void CSideWidthWidget::changeButtonTheme()
{
    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
    foreach (CCheckButton *button, m_buttonMap.keys()) {
        button->setCurrentTheme(themeType);
    }
}


