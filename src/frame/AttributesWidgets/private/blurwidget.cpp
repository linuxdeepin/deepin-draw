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
#include "cattributemanagerwgt.h"

#include <DSlider>
#include <DGuiApplicationHelper>
#include <QButtonGroup>

#include <QHBoxLayout>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 14;

const int blur_min_width = 5;
const int blur_max_width = 500;

BlurWidget::BlurWidget(DWidget *parent)
    : DrawAttribution::CAttriBaseOverallWgt(parent)
{
    setAttribution(DrawAttribution::EBlurAttri);
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
//        emit blurWidthChanged(width);
        emit blurEffectChanged(getEffect());
    }
}

void BlurWidget::setBlurType(const EBlurEffect &blurEffect, bool emitSig)
{
    auto btn = m_TypeButtons->button(blurEffect);
    if (btn != nullptr) {
        if (!emitSig) {
            m_TypeButtons->blockSignals(true);
        }

        btn->setChecked(true);

        if (!emitSig) {
            m_TypeButtons->blockSignals(false);
        }
    }
}

SBLurEffect BlurWidget::getEffect() const
{
    SBLurEffect ef;
    ef.type = m_TypeButtons->checkedId();
    ef.width = m_spinboxForLineWidth->value();

    return ef;
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

    auto m_blurBtn = new DToolButton(this);
    setWgtAccesibleName(m_blurBtn, "Blur type button");
    m_blurBtn->setMaximumSize(QSize(38, 38));
    m_blurBtn->setIconSize(QSize(38, 38));
    m_blurBtn->setToolTip(tr("Blur"));
    m_blurBtn->setCheckable(true);
    connect(m_blurBtn, &DToolButton::toggled, this, [ = ](bool checked) {
        QIcon icon       = QIcon::fromTheme("ddc_fuzzy tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_fuzzy tool_checked");
        m_blurBtn->setIcon(checked ? activeIcon : icon);
    });
    m_blurBtn->setIcon(QIcon::fromTheme("ddc_fuzzy tool_normal"));

    auto m_masicBtn = new DToolButton(this);
    setWgtAccesibleName(m_masicBtn, "Masic type button");
    m_masicBtn->setMaximumSize(QSize(38, 38));
    m_masicBtn->setIconSize(QSize(38, 38));
    m_masicBtn->setToolTip(tr("Mosaic"));
    m_masicBtn->setCheckable(true);
    connect(m_masicBtn, &DToolButton::toggled, this, [ = ](bool checked) {
        QIcon icon       = QIcon::fromTheme("ddc_smudge tool");
        QIcon activeIcon = QIcon::fromTheme("ddc_smudge tool_checked");
        m_masicBtn->setIcon(checked ? activeIcon : icon);
    });
    m_masicBtn->setIcon(QIcon::fromTheme("ddc_smudge tool"));

    m_TypeButtons = new QButtonGroup(this);
    m_TypeButtons->addButton(m_blurBtn, BlurEffect);
    m_TypeButtons->addButton(m_masicBtn, MasicoEffect);
    m_TypeButtons->setExclusive(true);

    connect(m_TypeButtons, QOverload<int, bool>::of(&QButtonGroup::buttonToggled), this, [ = ](int tp, bool checked) {
        if (checked) {
//            emit blurTypeChanged(EBlurEffect(tp));
            emit blurEffectChanged(getEffect());
        }
    });

    DLabel *penWidthLabel = new DLabel(this);
    penWidthLabel->setObjectName("Width");
    penWidthLabel->setText(tr("Width"));
    penWidthLabel->setFont(ft);

    m_spinboxForLineWidth = new CSpinBox(this);
    m_spinboxForLineWidth->setObjectName("BlurPenWidth");
    m_spinboxForLineWidth->setKeyboardTracking(false);

    m_spinboxForLineWidth->setSpinRange(5, 500);

    m_spinboxForLineWidth->setValue(20);
    m_spinboxForLineWidth->setProperty("preValue", 20);
    if (!Application::isTabletSystemEnvir())
        m_spinboxForLineWidth->setFixedWidth(90);
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

    QHBoxLayout *layout = static_cast<QHBoxLayout *>(centerLayout());

    layout->addWidget(penLabel);
    layout->addWidget(m_blurBtn);
    layout->addWidget(m_masicBtn);
    layout->addWidget(penWidthLabel);
    layout->addWidget(m_spinboxForLineWidth);
    setLayout(layout);

    setBlurType(EBlurEffect::MasicoEffect);

    for (int i = 0; i < centerLayout()->count(); ++i) {
        auto item = centerLayout()->itemAt(i);
        if (item->widget() != nullptr) {
            _allWgts.append(item->widget());
        }
    }
}

void BlurWidget::initConnection()
{
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
