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
#include "pickcolorwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QDBusInterface>
#include <QDebug>

#include "utils/global.h"
#include "service/colorpickerinterface.h"
#include "ccheckbutton.h"

#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

const QSize PICKCOLOR_WIDGET_SIZE = QSize(226, 230);

PickColorWidget::PickColorWidget(DWidget *parent)
    : DWidget(parent)
{
    setFixedSize(PICKCOLOR_WIDGET_SIZE);
    DLabel *titleLabel = new DLabel(this);
    titleLabel->setText("RGB");
    titleLabel->setFixedWidth(38);


    m_redEditLabel = new EditLabel(this);
    m_redEditLabel->setTitle("R");
    m_redEditLabel->setEditText("255");
    connect(m_redEditLabel, &EditLabel::editTextChanged,
            this, &PickColorWidget::updateColor);

    m_greenEditLabel = new EditLabel(this);
    m_greenEditLabel->setTitle("G");
    m_greenEditLabel->setEditText("255");
    connect(m_greenEditLabel, &EditLabel::editTextChanged,
            this, &PickColorWidget::updateColor);

    m_blueEditLabel = new EditLabel(this);
    m_blueEditLabel->setTitle("B");
    m_blueEditLabel->setEditText("255");
    connect(m_blueEditLabel, &EditLabel::editTextChanged,
            this, &PickColorWidget::updateColor);


    QMap<int, QMap<CCheckButton::EButtonSattus, QString> > pictureMap;
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/draw/color_draw_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/draw/color_draw_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/draw/color_draw_active.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/draw/color_draw_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/light/images/draw/color_draw_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/light/images/draw/color_draw_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/light/images/draw/color_draw_active.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/light/images/draw/color_draw_active.svg");

    m_picker = new CCheckButton(pictureMap, QSize(24, 24), this, false);
    m_picker->setFixedSize(24, 24);
    QHBoxLayout *rgbLayout = new QHBoxLayout;
    rgbLayout->setMargin(0);
    rgbLayout->setSpacing(0);
    rgbLayout->addWidget(titleLabel);
    rgbLayout->addSpacing(10);
    rgbLayout->addWidget(m_redEditLabel);
    rgbLayout->addWidget(m_greenEditLabel);
    rgbLayout->addWidget(m_blueEditLabel);
    rgbLayout->addSpacing(4);
    rgbLayout->addWidget(m_picker);
    m_colorSlider = new ColorSlider(this);
    m_colorSlider->setFixedSize(226, 25);

    m_colorLabel = new ColorLabel(this);
    m_colorLabel->setFixedSize(226, 136);
    connect(m_colorSlider, &ColorSlider::valueChanged, m_colorLabel, [ = ](int val) {
        m_colorLabel->setHue(val);
    });
    connect(m_colorLabel, &ColorLabel::pickedColor, this,  [ = ](QColor color) {
        setRgbValue(color, true);
    });
    connect(m_picker, &CCheckButton::buttonClick, this, [ = ] {
        ColorPickerInterface *cp = new ColorPickerInterface("com.deepin.Picker",
                                                            "/com/deepin/Picker", QDBusConnection::sessionBus(), this);

        cp->StartPick(QString("%1").arg(qApp->applicationPid()));
        connect(cp, &ColorPickerInterface::colorPicked, this, [ = ](QString uuid,
                                                                    QString colorName)
        {
            if (uuid == QString("%1").arg(qApp->applicationPid())) {
                setRgbValue(QColor(colorName), true);
            }
            m_picker->setChecked(false);
            cp->deleteLater();
        });
    });

    QVBoxLayout *mLayout = new QVBoxLayout;
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addSpacing(16);
    mLayout->addLayout(rgbLayout);
    mLayout->addSpacing(10);
    mLayout->addWidget(m_colorLabel, 0, Qt::AlignCenter);
    mLayout->addWidget(m_colorSlider, 0, Qt::AlignCenter);
    setLayout(mLayout);
}

void PickColorWidget::setRgbValue(QColor color, bool isPicked)
{
    m_redEditLabel->setEditText(QString("%1").arg(color.red()));
    m_greenEditLabel->setEditText(QString("%1").arg(color.green()));
    m_blueEditLabel->setEditText(QString("%1").arg(color.blue()));

    if (isPicked)
        emit pickedColor(color);
}

void PickColorWidget::updateColor()
{
    int r = m_redEditLabel->editText().toInt();
    int g = m_greenEditLabel->editText().toInt();
    int b = m_blueEditLabel->editText().toInt();

    if (QColor(r, g, b).isValid()) {
        emit pickedColor(QColor(r, g, b));
    }
}

void PickColorWidget::setPickedColor(bool picked)
{
    m_colorLabel->setPickColor(picked);
}

PickColorWidget::~PickColorWidget()
{
}
