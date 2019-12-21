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
#include "ciconbutton.h"

#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

const QSize PICKCOLOR_WIDGET_SIZE = QSize(294, 215);

PickColorWidget::PickColorWidget(DWidget *parent)
    : DWidget(parent)
{
    setFixedSize(PICKCOLOR_WIDGET_SIZE);
    DLabel *titleLabel = new DLabel(this);
    QFont  titleLabelFont = titleLabel->font();
    titleLabelFont.setPixelSize(13);
    titleLabel->setText("RGB");
    titleLabel->setFixedWidth(27);
    titleLabel->setFont(titleLabelFont);

    m_cp = new ColorPickerInterface("com.deepin.Picker",
                                    "/com/deepin/Picker", QDBusConnection::sessionBus(), this);

    connect(m_cp, &ColorPickerInterface::colorPicked, this, [ = ](QString uuid, QString colorName) {
        if (uuid == QString("%1").arg(qApp->applicationPid())) {
            setRgbValue(QColor(colorName), true);
        }
        m_picker->setChecked(false);
    });


    m_redEditLabel = new EditLabel(this);

    m_greenEditLabel = new EditLabel(this);

    m_blueEditLabel = new EditLabel(this);



    QMap<int, QMap<CIconButton::EIconButtonSattus, QString> > pictureMap;
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Normal] = QString(":/theme/light/images/draw/dorpper_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Hover] = QString(":/theme/light/images/draw/dorpper_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Press] = QString(":/theme/light/images/draw/dorpper_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Active] = QString(":/theme/light/images/draw/dorpper_active.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Normal] = QString(":/theme/dark/images/draw/dorpper_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Hover] = QString(":/theme/dark/images/draw/dorpper_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Press] = QString(":/theme/dark/images/draw/dorpper_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Active] = QString(":/theme/dark/images/draw/dorpper_active.svg");

    m_picker = new CIconButton(pictureMap, QSize(55, 36), this, false);


    QHBoxLayout *rgbLayout = new QHBoxLayout;
    rgbLayout->setMargin(0);
    rgbLayout->setSpacing(0);
    rgbLayout->addWidget(titleLabel);
    rgbLayout->addSpacing(22);
    rgbLayout->addWidget(m_redEditLabel);
    rgbLayout->addSpacing(7);
    rgbLayout->addWidget(m_greenEditLabel);
    rgbLayout->addSpacing(8);
    rgbLayout->addWidget(m_blueEditLabel);
    rgbLayout->addSpacing(10);
    rgbLayout->addWidget(m_picker);
    m_colorSlider = new ColorSlider(this);
//    m_colorSlider->setFixedSize(PICKCOLOR_WIDGET_SIZE.width(), 25);

    m_colorLabel = new ColorLabel(this);
    m_colorLabel->setFixedSize(PICKCOLOR_WIDGET_SIZE.width(), 136);

    connect(m_colorSlider, &ColorSlider::valueChanged, m_colorLabel, [ = ](int val) {
        m_colorLabel->setHue(val);
    });
    connect(m_colorLabel, &ColorLabel::pickedColor, this,  [ = ](QColor color) {
        setRgbValue(color, true);
    });

    connect(m_colorLabel, &ColorLabel::signalPreViewColor, this,  [ = ](QColor color) {
        preSetRgbValue(color);
    });

    connect(m_picker, &CIconButton::mouseRelease, this, [ = ] {
        m_cp->StartPick(QString("%1").arg(qApp->applicationPid()));
//        QDBusPendingReply<> reply =  m_cp->StartPick(QString("%1").arg(qApp->applicationPid()));
//        reply.waitForFinished();
//        if (reply.isError())
//        {
//            QDBusError error = reply.error();
//            qDebug() << "faild@@@@@@@@@@@" << error;
//        } else
//        {
//            qDebug() << "success!!!!!!!!!";
//            connect(m_cp, &ColorPickerInterface::colorPicked, this, [ = ](QString uuid,
//            QString colorName) {
//                if (uuid == QString("%1").arg(qApp->applicationPid())) {
//                    setRgbValue(QColor(colorName), true);
//                }
//                m_picker->setChecked(false);
//            });
//        }

//        ColorPickerInterface *cp = new ColorPickerInterface("com.deepin.Picker",
//                                                            "/com/deepin/Picker", QDBusConnection::sessionBus(), this);

//        cp->StartPick(QString("%1").arg(qApp->applicationPid()));
//        connect(cp, &ColorPickerInterface::colorPicked, this, [ = ](QString uuid,
//                                                                    QString colorName)
//        {
//            if (uuid == QString("%1").arg(qApp->applicationPid())) {
//                setRgbValue(QColor(colorName), true);
//            }
//            m_picker->setChecked(false);
//            cp->deleteLater();
//        });

    });

    QVBoxLayout *mLayout = new QVBoxLayout;
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
//    mLayout->addSpacing(2);
    mLayout->addLayout(rgbLayout);
    mLayout->addSpacing(10);
    mLayout->addWidget(m_colorLabel, 0, Qt::AlignCenter);
    mLayout->addSpacing(11);
    mLayout->addWidget(m_colorSlider, 0, Qt::AlignCenter);
    setLayout(mLayout);
}

void PickColorWidget::setRgbValue(QColor color, bool isPicked)
{
    m_redEditLabel->setText(QString("%1").arg(color.red()));
    m_greenEditLabel->setText(QString("%1").arg(color.green()));
    m_blueEditLabel->setText(QString("%1").arg(color.blue()));

    if (isPicked)
        emit pickedColor(color);
}

void PickColorWidget::preSetRgbValue(QColor color)
{
    m_redEditLabel->setText(QString("%1").arg(color.red()));
    m_greenEditLabel->setText(QString("%1").arg(color.green()));
    m_blueEditLabel->setText(QString("%1").arg(color.blue()));

    emit signalPreSetColorName(color);
}

void PickColorWidget::updateColor()
{
    int r = m_redEditLabel->text().toInt();
    int g = m_greenEditLabel->text().toInt();
    int b = m_blueEditLabel->text().toInt();

    if (QColor(r, g, b).isValid()) {
        emit pickedColor(QColor(r, g, b));
    }
}

void PickColorWidget::setPickedColor(bool picked)
{
    m_colorLabel->setPickColor(picked);
}

void PickColorWidget::updateButtonTheme(int theme)
{
    m_picker->setCurrentTheme(theme);
}

PickColorWidget::~PickColorWidget()
{
}
