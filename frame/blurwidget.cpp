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
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/globaldefine.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"
#include "widgets/cspinbox.h"

#include <DSlider>
#include <DGuiApplicationHelper>

#include <QHBoxLayout>
#include <QLineEdit>

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
    m_pLineWidthSlider->blockSignals(true);
    m_pLineWidthSlider->setValue(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth());
    m_pLineWidthSlider->blockSignals(false);

    //m_spinboxForLineWidth
    m_spinboxForLineWidth->blockSignals(true);
    m_spinboxForLineWidth->setValue(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getBlurWidth());
    m_spinboxForLineWidth->blockSignals(false);

    m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
    //CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void BlurWidget::changeButtonTheme()
{

}

void BlurWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys, bool write2Cache)
{
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
        case Blurtype: {
            bool macio = propertys[property].toBool();
            updateIcon(macio);
            break;
        }
        case BlurWidth: {
            m_pLineWidthSlider->blockSignals(true);
            m_pLineWidthSlider->setValue(propertys[property].toInt());
            m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
            m_pLineWidthSlider->blockSignals(false);

            m_spinboxForLineWidth->blockSignals(true);
            m_spinboxForLineWidth->setValue(propertys[property].toInt());
            m_spinboxForLineWidth->blockSignals(false);
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

    m_blurBtn = new DToolButton(this);
    m_blurBtn->setMaximumSize(QSize(38, 38));
    m_blurBtn->setIconSize(QSize(38, 38));
    m_blurBtn->setToolTip(tr("Blur"));
    m_blurBtn->setCheckable(true);
    m_actionButtons.append(m_blurBtn);

    m_masicBtn = new DToolButton(this);
    m_masicBtn->setMaximumSize(QSize(38, 38));
    m_masicBtn->setIconSize(QSize(38, 38));
    m_masicBtn->setToolTip(tr("Mosaic"));
    m_masicBtn->setCheckable(true);
    m_actionButtons.append(m_masicBtn);

    updateIcon(true);


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
    m_pLineWidthSlider->hide();

    m_spinboxForLineWidth = new CSpinBox(this);
    m_spinboxForLineWidth->setKeyboardTracking(false);
    m_spinboxForLineWidth->setEnabledEmbedStyle(true);
    m_spinboxForLineWidth->setMinimum(INT_MIN + 1); //允许输入任何值在槽响应中限制范围(20-160)
    m_spinboxForLineWidth->setMaximum(INT_MAX - 1); //允许输入任何值在槽响应中限制范围(20-160)
//    m_spinboxForLineWidth->setMinimum(0); //允许输入任何值在槽响应中限制范围(20-160)
//    m_spinboxForLineWidth->setMaximum(10000); //允许输入任何值在槽响应中限制范围(20-160)
    m_spinboxForLineWidth->setValue(20);
    m_spinboxForLineWidth->setProperty("preValue", 20);
    m_spinboxForLineWidth->setFixedWidth(90);
    m_spinboxForLineWidth->setMaximumHeight(36);
    m_spinboxForLineWidth->setSuffix("px");
    m_spinboxForLineWidth->lineEdit()->setClearButtonEnabled(false);


    m_pLineWidthLabel = new DLabel(this);
    m_pLineWidthLabel->setObjectName("Width Label");
    m_pLineWidthLabel->setText(QString("%1px").arg(m_pLineWidthSlider->value()));
    m_pLineWidthLabel->setFont(ft);
    m_pLineWidthLabel->setFixedWidth(60);
    m_pLineWidthLabel->hide();

    connect(m_spinboxForLineWidth, QOverload<int>::of(&DSpinBox::valueChanged), this, [ = ](int value) {
//        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurWidth(value);
//        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::BlurWidth, value);
        //1.值检测是否合法符合需求(检测最大值和最小值)
        m_spinboxForLineWidth->blockSignals(true);
        if (m_spinboxForLineWidth->value() < 20) {
            m_spinboxForLineWidth->setValue(20);
        } else if (m_spinboxForLineWidth->value() > 160) {
            m_spinboxForLineWidth->setValue(160);
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
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurWidth(value);

        static QMap<CGraphicsItem *, QVariant> s_oldTempValues;
        bool pushToStack = !m_spinboxForLineWidth->isTimerRunning();
        bool firstRecord = s_oldTempValues.isEmpty();
        QMap<CGraphicsItem *, QVariant> *inUndoValues = m_spinboxForLineWidth->isChangedByWheelEnd() ? &s_oldTempValues : nullptr;
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(BlurWidth, value, pushToStack, ((!pushToStack && firstRecord) ? &s_oldTempValues : nullptr), inUndoValues);
        m_spinboxForLineWidth->setProperty("preValue", m_spinboxForLineWidth->value());

        if (m_spinboxForLineWidth->isChangedByWheelEnd()) {
            s_oldTempValues.clear();
        }
    });
    connect(m_spinboxForLineWidth, &DSpinBox::editingFinished, this, [ = ] () {
        //等于0时是特殊字符，不做处理
        qDebug() << "m_spinboxForLineWidth->value() = " << m_spinboxForLineWidth->value();
        if ( m_spinboxForLineWidth->value() < 0) {
            return ;
        }
        m_spinboxForLineWidth->blockSignals(true);
        if (m_spinboxForLineWidth->value() < 20) {
            m_spinboxForLineWidth->setValue(20);
        } else if (m_spinboxForLineWidth->value() > 160) {
            m_spinboxForLineWidth->setValue(160);
        }
        m_spinboxForLineWidth->blockSignals(false);

        int preIntValue = m_spinboxForLineWidth->property("preValue").toInt();
        if (preIntValue == m_spinboxForLineWidth->value()) {
            return;
        }
        m_spinboxForLineWidth->setProperty("preValue", m_spinboxForLineWidth->value());

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurWidth(m_spinboxForLineWidth->value());
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(BlurWidth, m_spinboxForLineWidth->value(), !m_spinboxForLineWidth->isTimerRunning());
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
    layout->addWidget(m_spinboxForLineWidth);
    layout->addStretch();
    setLayout(layout);
}

void BlurWidget::initConnection()
{
    connect(m_blurBtn, &DPushButton::released, [ = ]() {
        updateIcon(false);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurEffect(BlurEffect);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::Blurtype, BlurEffect);
    });

    connect(m_masicBtn, &DPushButton::released, [ = ]() {
        updateIcon(true);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setBlurEffect(MasicoEffect);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::Blurtype, MasicoEffect);
    });
}

void BlurWidget::updateIcon(bool masic)
{
    m_blurBtn->setChecked(!masic);
    m_masicBtn->setChecked(masic);

    if (masic) {
        m_blurBtn->setIcon(QIcon::fromTheme("ddc_fuzzy tool_normal"));
        m_masicBtn->setIcon(QIcon::fromTheme("ddc_smudge tool_checked"));
    } else {
        m_blurBtn->setIcon(QIcon::fromTheme("ddc_fuzzy tool_checked"));
        m_masicBtn->setIcon(QIcon::fromTheme("ddc_smudge tool"));
    }
}
