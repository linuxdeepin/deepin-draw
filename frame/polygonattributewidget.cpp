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
#include "polygonattributewidget.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/cspinbox.h"
#include "utils/cvalidator.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"

#include <DLabel>
#include <DFontSizeManager>

#include <QAction>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 14;
//DWIDGET_USE_NAMESPACE

PolygonAttributeWidget::PolygonAttributeWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}



PolygonAttributeWidget::~PolygonAttributeWidget()
{

}

void PolygonAttributeWidget::changeButtonTheme()
{
    m_sideWidthWidget->changeButtonTheme();
    m_sepLine->updateTheme();
}

void PolygonAttributeWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys, bool write2Cache)
{
    m_fillBtn->setVisible(false);
    m_strokeBtn->setVisible(false);
    m_sepLine->setVisible(true);
//    m_lwLabel->setVisible(false);
    m_sideWidthWidget->setVisible(false);
    m_sideNumLabel->setVisible(false);
    m_sideNumSpinBox->setVisible(false);
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
        case FillColor:
            m_fillBtn->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fillBtn->setIsMultColorSame(false);
            } else {
                m_fillBtn->setColor(propertys[property].value<QBrush>().color());
            }
            m_fillBtn->update();
            break;
        case LineColor:
            m_strokeBtn->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_strokeBtn->setIsMultColorSame(false);
            } else {
                m_strokeBtn->setColor(propertys[property].value<QColor>());
            }
            m_strokeBtn->update();
            break;
        case LineWidth:
//            m_lwLabel->setVisible(true);
            m_sideWidthWidget->setVisible(true);
            m_sideWidthWidget->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideWidthWidget->setMenuNoSelected(true);
            } else {
                m_sideWidthWidget->setSideWidth(propertys[property].toInt());
            }
            m_sideWidthWidget->blockSignals(false);
            m_sideWidthWidget->update();
            break;
        case SideNumber:
            m_sideNumLabel->setVisible(true);
            m_sideNumSpinBox->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideNumSpinBox->blockSignals(true);
                m_sideNumSpinBox->setValue(0);
                m_sideNumSpinBox->blockSignals(false);
            } else {
                m_sideNumSpinBox->setValue(propertys[property].toInt());
            }
            m_sideNumSpinBox->setProperty("preValue", m_sideNumSpinBox->value());
            break;
        default:
            break;
        }
    }
    m_sepLine->setVisible(!m_sideNumSpinBox->isHidden());
}

void PolygonAttributeWidget::initUI()
{
    setAttribute(Qt::WA_NoMousePropagation, true);
    m_fillBtn = new BigColorButton(this);
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);

    m_strokeBtn = new BorderColorButton(this);

    m_sepLine = new SeperatorLine(this);
//    m_lwLabel = new DLabel(this);
//    m_lwLabel->setText(tr("Width"));
    QFont ft1;
    ft1.setPixelSize(TEXT_SIZE - 1);
//    m_lwLabel->setFont(ft1);

    m_sideWidthWidget = new CSideWidthWidget(this);
    m_sideWidthWidget->setFixedWidth(100);
    m_sideWidthWidget->setFont(ft);

    m_sideNumLabel = new DLabel(this);
    m_sideNumLabel->setText(tr("Sides"));
    m_sideNumLabel->setFont(ft1);

    m_sideNumSpinBox = new CSpinBox(this);
    m_sideNumSpinBox->setKeyboardTracking(false);
    m_sideNumSpinBox->setFixedWidth(70);
    m_sideNumSpinBox->setRange(0, 1000);
    //m_sideNumSlider->setRange(4, 10);//此注释不删，记录边数范围
    m_sideNumSpinBox->setFont(ft);
    m_sideNumSpinBox->setSpecialValueText("— —");
    m_sideNumSpinBox->setEnabledEmbedStyle(true);
    m_sideNumSpinBox->lineEdit()->setClearButtonEnabled(false);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
//    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
//    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_strokeBtn);
//    layout->addSpacing(SEPARATE_SPACING);
//    layout->addWidget(m_sepLine);
//    layout->addSpacing(SEPARATE_SPACING);
//    layout->addWidget(m_lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addWidget(m_sepLine);
    layout->addWidget(m_sideNumLabel);
    layout->addWidget(m_sideNumSpinBox);

    layout->addStretch();
    setLayout(layout);
}

void PolygonAttributeWidget::initConnection()
{
    connect(m_fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_strokeBtn->resetChecked();
        emit showColorPanel(DrawStatus::Fill, getBtnPosition(m_fillBtn), show);

    });
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke,  getBtnPosition(m_strokeBtn), show);
    });

    connect(this, &PolygonAttributeWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
        m_strokeBtn->resetChecked();
    });

    //描边粗细
    connect(m_sideWidthWidget, SIGNAL(signalSideWidthChoosed(int)), this, SLOT(slotSideWidthChoosed(int)));
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthMenuShow, this, [ = ] () {
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    ///多边形边数
    connect(m_sideNumSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotSideValueChanged(int)));
    //m_sideNumSlider->setValue(5);
    connect(m_sideNumSpinBox, &CSpinBox::focusChanged, this, [ = ] (bool isFocus) {
        emit signalSideValueIsfocus(isFocus);
    });
    connect(m_sideNumSpinBox, &DSpinBox::editingFinished, this, [ = ] () {
        //等于0时是特殊字符，不做处理
        qDebug() << "m_sideNumSpinBox->value() = " << m_sideNumSpinBox->value();
        if ( m_sideNumSpinBox->value() == 0) {
            return ;
        }
        m_sideNumSpinBox->blockSignals(true);
        if (m_sideNumSpinBox->value() < 4) {
            m_sideNumSpinBox->setValue(4);
        } else if (m_sideNumSpinBox->value() > 10) {
            m_sideNumSpinBox->setValue(10);
        }
        m_sideNumSpinBox->blockSignals(false);

        QVariant preValue = m_sideNumSpinBox->property("preValue");

        if (preValue.isValid()) {
            int preIntValue = preValue.toInt();
            int curValue    = m_sideNumSpinBox->value();
            if (preIntValue == curValue)
                return;
        }
        m_sideNumSpinBox->setProperty("preValue", m_sideNumSpinBox->value());

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSideNum(m_sideNumSpinBox->value());
        emit signalPolygonAttributeChanged();
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::SideNumber, m_sideNumSpinBox->value());


    });
    m_sideNumSpinBox->setProperty("preValue", 5);
}

void PolygonAttributeWidget::updatePolygonWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    int sideNum = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSideNum();

    if (sideNum != m_sideNumSpinBox->value()) {
        m_sideNumSpinBox->blockSignals(true);
        m_sideNumSpinBox->setValue(sideNum);
        m_sideNumSpinBox->blockSignals(false);
    }

    m_fillBtn->setVisible(true);
    m_strokeBtn->setVisible(true);
    m_sepLine->setVisible(true);
//    m_lwLabel->setVisible(true);
    m_sideWidthWidget->setVisible(true);
    m_sideNumLabel->setVisible(true);
    m_sideNumSpinBox->setVisible(true);
    //CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void PolygonAttributeWidget::slotSideValueChanged(int value)
{
    m_sideNumSpinBox->blockSignals(true);
    if (m_sideNumSpinBox->value() < 4) {
        m_sideNumSpinBox->setValue(4);
    } else if (m_sideNumSpinBox->value() > 10) {
        m_sideNumSpinBox->setValue(10);
    }
    m_sideNumSpinBox->blockSignals(false);

    if (!m_sideNumSpinBox->isChangedByWheelEnd()) {
        QVariant preValue = m_sideNumSpinBox->property("preValue");

        if (preValue.isValid()) {
            int preIntValue = preValue.toInt();
            int curValue    = m_sideNumSpinBox->value();
            if (preIntValue == curValue)
                return;
        }
        m_sideNumSpinBox->setProperty("preValue", m_sideNumSpinBox->value());
    }

    value = m_sideNumSpinBox->value();
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSideNum(value);
    emit signalPolygonAttributeChanged();
    //隐藏调色板
    showColorPanel(DrawStatus::Stroke, QPoint(), false);

    static QMap<CGraphicsItem *, QVariant> s_oldTempValues;
    bool pushToStack = !m_sideNumSpinBox->isTimerRunning();
    bool firstRecord = s_oldTempValues.isEmpty();
    QMap<CGraphicsItem *, QVariant> *inUndoValues = m_sideNumSpinBox->isChangedByWheelEnd() ? &s_oldTempValues : nullptr;
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::SideNumber, value, pushToStack, ((!pushToStack && firstRecord) ? &s_oldTempValues : nullptr), inUndoValues);
    m_sideNumSpinBox->setProperty("preValue", m_sideNumSpinBox->value());
    if (m_sideNumSpinBox->isChangedByWheelEnd()) {
        s_oldTempValues.clear();
    }

    //CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::SideNumber, m_sideNumSpinBox->value());
}

void PolygonAttributeWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
    this->setFocus();
}

QPoint PolygonAttributeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
