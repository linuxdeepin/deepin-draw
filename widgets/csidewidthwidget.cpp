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
#include "ccheckbutton.h"

CSideWidthWidget::CSideWidthWidget(DWidget *parent)
    : DFrame(parent)
{
    initUI();
    initConnection();
}


void CSideWidthWidget::initUI()
{
    m_layout = new QHBoxLayout (this);
    m_layout->setMargin(0);

    QMap<CCheckButton::EButtonSattus, QString> pictureMap;

    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness01_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness01_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/draw/tickness01_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/draw/tickness01_checked.svg");

//    pictureMap[CPushButton::Normal] = QString(":/theme/light/images/draw/line_border_1_normal.svg");
//    pictureMap[CPushButton::Hover]  = QString(":/theme/light/images/draw/line_border_1_hover.svg");
//    pictureMap[CPushButton::Press]  = QString(":/theme/light/images/draw/line_border_1_active.svg");
//    pictureMap[CPushButton::Active] = QString(":/theme/light/images/draw/line_border_1_active.svg");

    m_finerButton = new CCheckButton(pictureMap, this);
    m_buttonMap.insert(m_finerButton, Finer);

    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness02_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness02_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/draw/tickness02_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/draw/tickness02_checked.svg");
    m_fineButton = new CCheckButton(pictureMap, this);

    m_fineButton->minimumSize();

    m_buttonMap.insert(m_fineButton, Fine);

    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness03_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness03_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/draw/tickness03_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/draw/tickness03_checked.svg");
    m_mediumButton = new CCheckButton(pictureMap, this);
    m_buttonMap.insert(m_mediumButton, Medium);

    pictureMap[CCheckButton::Normal] = QString(":/theme/light/images/draw/tickness04_normal.svg");
    pictureMap[CCheckButton::Hover] = QString(":/theme/light/images/draw/tickness04_hover.svg");
    pictureMap[CCheckButton::Press] = QString(":/theme/light/images/draw/tickness04_press.svg");
    pictureMap[CCheckButton::Active] = QString(":/theme/light/images/draw/tickness04_checked.svg");
    m_boldButton = new CCheckButton(pictureMap, this);
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
        CDrawParamSigleton::GetInstance()->setLineWidth(m_buttonMap.value(m_finerButton));
        emit signalSideWidthChange();
    });

    connect(m_fineButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_fineButton);
        CDrawParamSigleton::GetInstance()->setLineWidth(m_buttonMap.value(m_fineButton));
        emit signalSideWidthChange();
    });

    connect(m_mediumButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_mediumButton);
        CDrawParamSigleton::GetInstance()->setLineWidth(m_buttonMap.value(m_mediumButton));
        emit signalSideWidthChange();
    });

    connect(m_boldButton, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_boldButton);
        CDrawParamSigleton::GetInstance()->setLineWidth(m_buttonMap.value(m_boldButton));
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
    int sideWidth = CDrawParamSigleton::GetInstance()->getLineWidth();
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


