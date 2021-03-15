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
#include "blurwidget.h"
#include "widgets/toolbutton.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/globaldefine.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "widgets/cspinbox.h"
#include "application.h"

#include <DSlider>
#include <DGuiApplicationHelper>

#include <QHBoxLayout>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 14;

const int blur_min_width = 5;
const int blur_max_width = 500;

BlurWidget::BlurWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

BlurWidget::~BlurWidget()
{
}

void BlurWidget::setBlurWidth(const int &width, bool emitSig)
{
    m_spinboxForLineWidth->blockSignals(true);
    m_spinboxForLineWidth->setValue(width);
    m_spinboxForLineWidth->blockSignals(false);

    if (emitSig) {
        blurWidthChanged(width);
    }
}

void BlurWidget::setBlurType(const EBlurEffect &blurEffect, bool emitSig)
{
    bool masic = false;
    if (blurEffect == EBlurEffect::MasicoEffect) {
        masic = true;
    } else if (blurEffect == EBlurEffect::BlurEffect) {
        masic = false;
    }

    m_blurBtn->setChecked(!masic);
    m_masicBtn->setChecked(masic);

    if (masic) {
        m_blurBtn->setIcon(QIcon::fromTheme("ddc_fuzzy tool_normal"));
        m_masicBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_checked"));
    } else {
        m_blurBtn->setIcon(QIcon::fromTheme("ddc_fuzzy tool_checked"));
        m_masicBtn->setIcon(QIcon::fromTheme("ddc_smudge tool"));
    }

    if (emitSig) {
        if (masic) {
            blurTypeChanged(EBlurEffect::MasicoEffect);
        } else {
            blurTypeChanged(EBlurEffect::BlurEffect);
        }
    }
}

void BlurWidget::setSameProperty(bool sameType, bool sameWidth)
{
    if (!sameType) {
        setBlurType(EBlurEffect::BlurEffect, false);
    }
    if (!sameWidth) {
        m_spinboxForLineWidth->blockSignals(true);
        m_spinboxForLineWidth->setSpecialText();
        m_spinboxForLineWidth->blockSignals(false);
    } else {
        m_spinboxForLineWidth->blockSignals(true);
        m_spinboxForLineWidth->setPrefix("");
        m_spinboxForLineWidth->blockSignals(false);
    }
}

void BlurWidget::initUI()
{
    this->setObjectName("BlurWidget");
    setAttribute(Qt::WA_NoMousePropagation, true);
    DLabel *penLabel = new DLabel(this);
    penLabel->setObjectName("TypeLabel");

    penLabel->setText(tr("Type"));

    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
    penLabel->setFont(ft);

    m_blurBtn = new DToolButton(this);
    drawApp->setWidgetAccesibleName(m_blurBtn, "Blur type button");
    //m_blurBtn->setObjectName("BlurBlurBtn");
    m_blurBtn->setMaximumSize(QSize(38, 38));
    m_blurBtn->setIconSize(QSize(38, 38));
    m_blurBtn->setToolTip(tr("Blur"));
    m_blurBtn->setCheckable(true);

    m_masicBtn = new DToolButton(this);
    drawApp->setWidgetAccesibleName(m_masicBtn, "Masic type button");
    //m_masicBtn->setObjectName("BlurMasicBtn");
    m_masicBtn->setMaximumSize(QSize(38, 38));
    m_masicBtn->setIconSize(QSize(38, 38));
    m_masicBtn->setToolTip(tr("Mosaic"));
    m_masicBtn->setCheckable(true);

    DLabel *penWidthLabel = new DLabel(this);
    penWidthLabel->setObjectName("Width");
    penWidthLabel->setText(tr("Width"));
    penWidthLabel->setFont(ft);

    m_spinboxForLineWidth = new CSpinBox(this);
    m_spinboxForLineWidth->setObjectName("BlurPenWidth");
    m_spinboxForLineWidth->setKeyboardTracking(false);
    //m_spinboxForLineWidth->setEnabledEmbedStyle(true);

    m_spinboxForLineWidth->setSpinRange(5, 500);

    m_spinboxForLineWidth->setValue(20);
    m_spinboxForLineWidth->setProperty("preValue", 20);
    //m_spinboxForLineWidth->setFixedWidth(90);
    m_spinboxForLineWidth->setMaximumHeight(36);
    m_spinboxForLineWidth->setSuffix("px");
    m_spinboxForLineWidth->lineEdit()->setClearButtonEnabled(false);
    m_spinboxForLineWidth->setFont(ft);

    m_pLineWidthLabel = new DLabel(this);
    m_pLineWidthLabel->setObjectName("Width Label");
    m_pLineWidthLabel->setText(QString("%1px").arg(m_spinboxForLineWidth->value()));
    m_pLineWidthLabel->setFont(ft);
    m_pLineWidthLabel->setFixedWidth(60);
    m_pLineWidthLabel->hide();

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
    layout->addWidget(m_spinboxForLineWidth);
    layout->addStretch();
    setLayout(layout);

    setBlurType(EBlurEffect::MasicoEffect);
}

void BlurWidget::initConnection()
{
    connect(m_blurBtn, &DPushButton::released, [ = ]() {
        setBlurType(EBlurEffect::BlurEffect);
    });

    connect(m_masicBtn, &DPushButton::released, [ = ]() {
        setBlurType(EBlurEffect::MasicoEffect);
    });

    connect(m_spinboxForLineWidth, QOverload<int>::of(&DSpinBox::valueChanged), this, [ = ](int value) {
        //1.值检测是否合法符合需求(检测最大值和最小值)
        m_spinboxForLineWidth->blockSignals(true);
        if (m_spinboxForLineWidth->value() < blur_min_width) {
            m_spinboxForLineWidth->setValue(blur_min_width);
        } else if (m_spinboxForLineWidth->value() > blur_max_width) {
            m_spinboxForLineWidth->setValue(blur_max_width);
        }
        m_spinboxForLineWidth->blockSignals(false);

        value = m_spinboxForLineWidth->value();

        //2.非滚轮结束时发送的值变化信号要进行重复值检测
        //  a.实际为了避免重复值入栈;
        //  b.如果是滚轮结束时发送的值变化信号，那么强行入栈!
        //    因为滚轮滚动时在setItemsCommonPropertyValue传入的是不入栈的标记，在滚轮结束后强行入栈一次,(这个时候不应该进行重复值检测了，因为值肯定是重复相等的)
        //    从而实现了滚动事件一次只入栈一次
        if (!m_spinboxForLineWidth->isChangedByWheelEnd()) {
            int preIntValue = m_spinboxForLineWidth->property("preValue").toInt();
            if (preIntValue == m_spinboxForLineWidth->value()) {
                return;
            }
            m_spinboxForLineWidth->setProperty("preValue", m_spinboxForLineWidth->value());
        }

        setBlurWidth(value);
    });

    connect(m_spinboxForLineWidth, &DSpinBox::editingFinished, this, [ = ]() {
        //等于0时是特殊字符，不做处理
        qDebug() << "m_spinboxForLineWidth->value() = " << m_spinboxForLineWidth->value();
        if (m_spinboxForLineWidth->value() < 0) {
            return ;
        }
        m_spinboxForLineWidth->blockSignals(true);
        if (m_spinboxForLineWidth->value() < blur_min_width) {
            m_spinboxForLineWidth->setValue(blur_min_width);
        } else if (m_spinboxForLineWidth->value() > blur_max_width) {
            m_spinboxForLineWidth->setValue(blur_max_width);
        }
        m_spinboxForLineWidth->blockSignals(false);

        int preIntValue = m_spinboxForLineWidth->property("preValue").toInt();
        if (preIntValue == m_spinboxForLineWidth->value()) {
            return;
        }
        m_spinboxForLineWidth->setProperty("preValue", m_spinboxForLineWidth->value());
    });
}
