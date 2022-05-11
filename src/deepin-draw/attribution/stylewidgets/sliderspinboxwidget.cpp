/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     RenBin <renbin@uniontech.com>
 *
 * Maintainer: TanLang <tanlang@uniontech.com>
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

#include "sliderspinboxwidget.h"
#include "boxlayoutwidget.h"

#include <DSpinBox>

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

/**
 * @class SliderSpinboxwidget
 * @brief 滑动条和数值输入框的组合控件，用于属性值展示及设置
 */

/**
 * @brief 构造函数，根据 \a style 初始化不同的控件，\a attri 用于基类标识当前属性控件类型
 */
SliderSpinBoxWidget::SliderSpinBoxWidget(int attri, BoxStyle style, QWidget *parent)
    : AttributeWgt(attri, parent)
{
    initUi(style);
}

/*!
 * \brief 接受外部设置更新的属性值 \a var , 数值一般来源drawboard
 */
void SliderSpinBoxWidget::setVar(const QVariant &var)
{
    int value = var.toInt();

    // 来自drawborad的更新，不向外发送更新信号
    QSignalBlocker sliderLocker(m_slider);
    m_slider->setValue(value);
    QSignalBlocker bokLocker(m_spinBox);
    m_spinBox->setValue(value);
}

/*!
 * \brief 设置滑动条和输入框允许的数值变化范围为 \a min ~ \a max
 */
void SliderSpinBoxWidget::setRange(int min, int max)
{
    m_slider->slider()->setRange(min, max);
    m_spinBox->setRange(min, max);
}

/**
 * @brief 设置属性标题 \a title
 */
void SliderSpinBoxWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

/**
 * @brief 根据不同的输入框显示风格 \a style 初始化界面布局
 * @param style 输入框显示风格，包括整数和百分数
 */
void SliderSpinBoxWidget::initUi(SliderSpinBoxWidget::BoxStyle style)
{
    m_title = new QLabel(this);
    m_slider = new DSlider(Qt::Horizontal, this);

    // 根据不同风格切换数值显示框
    switch (style) {
    case EPercentStyle: {
        m_spinBox = new QSpinBox(this);
        m_spinBox->setButtonSymbols(QSpinBox::NoButtons);
        m_spinBox->setSuffix(tr(" %"));
        m_spinBox->setAlignment(Qt::AlignCenter);
        break;
    }
    // 默认使用整数输入框
    default: {
        DSpinBox *dBox = new DSpinBox(this);
        dBox->setEnabledEmbedStyle(true);
        m_spinBox = dBox;
        break;
    }
    }
    m_spinBox->setMaximumWidth(100);

    BoxLayoutWidget *contextWid = new BoxLayoutWidget(this);
    contextWid->addWidget(m_slider, 3);
    contextWid->addWidget(m_spinBox, 1);

    QVBoxLayout *lay = new QVBoxLayout;
    lay->setContentsMargins(0, 10, 10, 0);
    lay->addWidget(m_title);
    lay->addWidget(contextWid);
    setLayout(lay);

    connect(m_slider->slider(), SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

/**
 * @brief 根据输入的属性值 \a value 更新控件，若数值变更，
 *      则向外发送 sigValueChanged() 信号
 */
void SliderSpinBoxWidget::onValueChanged(int value)
{
    bool isValueChanged = false;

    if (value != m_spinBox->value()) {
        QSignalBlocker locker(m_spinBox);
        m_spinBox->setValue(value);

        isValueChanged = true;
    }

    if (value != m_slider->value()) {
        QSignalBlocker locker(m_slider);
        m_slider->setValue(value);

        isValueChanged = true;
    }

    if (isValueChanged)
        Q_EMIT sigValueChanged(value);
}
