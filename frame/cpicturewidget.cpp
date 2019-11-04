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
#include "cpicturewidget.h"
#include "widgets/cclickbutton.h"
#include "drawshape/cdrawparamsigleton.h"

#include <QMap>
#include <QHBoxLayout>
#include <QDebug>

#include <DPalette>
#include <DApplicationHelper>
#include <DGuiApplicationHelper>




DGUI_USE_NAMESPACE


const int BTN_SPACING = 13;

CPictureWidget::CPictureWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CPictureWidget::~CPictureWidget()
{
}

void CPictureWidget::changeButtonTheme()
{
    int themeType = CDrawParamSigleton::GetInstance()->getThemeType();

    m_leftRotateBtn->setCurrentTheme(themeType);
    m_rightRotateBtn->setCurrentTheme(themeType);
    m_flipHBtn->setCurrentTheme(themeType);
    m_flipVBtn->setCurrentTheme(themeType);

}

void CPictureWidget::initUI()
{
    QMap<int, QMap<CClickButton::EClickBtnSatus, QString> > pictureMapClick;

    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Normal] = QString(":/theme/light/images/attribute/contrarotate_normal.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Hover] = QString(":/theme/light/images/attribute/contrarotate_hover.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Press] = QString(":/theme/light/images/attribute/contrarotate_press.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Disable] = QString(":/theme/light/images/attribute/contrarotate_disable.svg");

    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Normal] = QString(":/theme/dark/images/attribute/contrarotate_normal.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Hover] = QString(":/theme/dark/images/attribute/contrarotate_hover.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Press] = QString(":/theme/dark/images/attribute/contrarotate_press.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Disable] = QString(":/theme/dark/images/attribute/contrarotate_disable.svg");


    m_leftRotateBtn = new CClickButton(pictureMapClick, QSize(40, 36), this);


    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Normal] = QString(":/theme/light/images/attribute/clockwise rotation_normal.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Hover] = QString(":/theme/light/images/attribute/clockwise rotation_hover.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Press] = QString(":/theme/light/images/attribute/clockwise rotation_press.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Disable] = QString(":/theme/light/images/attribute/clockwise rotation_disable.svg");

    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Normal] = QString(":/theme/dark/images/attribute/clockwise rotation_normal.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Hover] = QString(":/theme/dark/images/attribute/clockwise rotation_hover.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Press] = QString(":/theme/dark/images/attribute/clockwise rotation_press.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Disable] = QString(":/theme/dark/images/attribute/clockwise rotation_disable.svg");



    m_rightRotateBtn = new CClickButton(pictureMapClick, QSize(40, 36), this);


    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Normal] = QString(":/theme/light/images/attribute/flip horizontal_normal.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Hover] = QString(":/theme/light/images/attribute/flip horizontal_hover.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Press] = QString(":/theme/light/images/attribute/flip horizontal_press.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Disable] = QString(":/theme/light/images/attribute/flip horizontal_disable.svg");

    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Normal] = QString(":/theme/dark/images/attribute/flip horizontal_normal.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Hover] = QString(":/theme/dark/images/attribute/flip horizontal_hover.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Press] = QString(":/theme/dark/images/attribute/flip horizontal_press.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Disable] = QString(":/theme/dark/images/attribute/flip horizontal_disable.svg");

    m_flipHBtn = new CClickButton(pictureMapClick, QSize(40, 36), this);


    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Normal] = QString(":/theme/light/images/attribute/flip vertical_normal.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Hover] = QString(":/theme/light/images/attribute/flip vertical_hover.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Press] = QString(":/theme/light/images/attribute/flip vertical_press.svg");
    pictureMapClick[DGuiApplicationHelper::LightType][CClickButton::Disable] = QString(":/theme/light/images/attribute/flip vertical_disable.svg");

    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Normal] = QString(":/theme/dark/images/attribute/flip vertical_normal.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Hover] = QString(":/theme/dark/images/attribute/flip vertical_hover.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Press] = QString(":/theme/dark/images/attribute/flip vertical_press.svg");
    pictureMapClick[DGuiApplicationHelper::DarkType][CClickButton::Disable] = QString(":/theme/dark/images/attribute/flip vertical_disable.svg");

    m_flipVBtn = new CClickButton(pictureMapClick, QSize(40, 36), this);


    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_leftRotateBtn);
    layout->addWidget(m_rightRotateBtn);
    layout->addWidget(m_flipHBtn);
    layout->addWidget(m_flipVBtn);
    layout->addStretch();
    setLayout(layout);
}

void CPictureWidget::initConnection()
{
    connect(m_leftRotateBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(LeftRotate);
    });

    connect(m_rightRotateBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(RightRotate);
    });

    connect(m_flipHBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(FlipHorizontal);
    });

    connect(m_flipVBtn, &CClickButton::buttonClick, this, [ = ]() {
        emit signalBtnClick(FlipVertical);
    });
}
