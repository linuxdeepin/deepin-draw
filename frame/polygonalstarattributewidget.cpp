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
#include "polygonalstarattributewidget.h"

#include <DLabel>
//#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QIntValidator>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QAction>

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/cspinbox.h"
#include "utils/cvalidator.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"

#include <QLineEdit>

const int BTN_SPACING = 5;
const int SEPARATE_SPACING = 4;
const int TEXT_SIZE = 14;
PolygonalStarAttributeWidget::PolygonalStarAttributeWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

PolygonalStarAttributeWidget::~PolygonalStarAttributeWidget()
{

}

void PolygonalStarAttributeWidget::changeButtonTheme()
{
    m_sideWidthWidget->changeButtonTheme();
    m_sepLine->updateTheme();
}

void PolygonalStarAttributeWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_fillBtn->setVisible(false);
    m_strokeBtn->setVisible(false);
    m_sepLine->setVisible(false);
//    m_lwLabel->setVisible(false);
    m_sideWidthWidget->setVisible(false);
    m_anchorNumLabel->setVisible(false);
    m_anchorNumber->setVisible(false);
    m_radiusLabel->setVisible(false);
    m_radiusNumber->setVisible(false);
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
        case Anchors:
            m_sepLine->setVisible(true);
            m_anchorNumLabel->setVisible(true);
            m_anchorNumber->setVisible(true);
            m_anchorNumber->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_anchorNumber->setValue(0);
            } else {
                m_anchorNumber->setValue(propertys[property].toInt());
            }
            m_anchorNumber->setProperty("preValue", m_anchorNumber->value());
            m_anchorNumber->blockSignals(false);
            break;
        case StarRadius:
            m_sepLine->setVisible(true);
            m_radiusLabel->setVisible(true);
            m_radiusNumber->setVisible(true);
            m_radiusNumber->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_radiusNumber->setValue(-1);
            } else {
                m_radiusNumber->setValue(propertys[property].toInt());
            }
            m_radiusNumber->setProperty("preValue", m_radiusNumber->value());
            m_radiusNumber->blockSignals(false);
            break;
        default:
            break;
        }
    }
}

void PolygonalStarAttributeWidget::initUI()
{
    setAttribute(Qt::WA_NoMousePropagation, true);
    m_fillBtn = new BigColorButton( this);

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

    m_anchorNumLabel = new DLabel(this);
    m_anchorNumLabel->setText(tr("Points"));
    m_anchorNumLabel->setFont(ft1);
    m_anchorNumber = new CSpinBox(this);
    m_anchorNumber->setKeyboardTracking(false);
    m_anchorNumber->setFixedSize(QSize(120, 36));
    m_anchorNumber->setRange(0, 1000);
    m_anchorNumber->setFont(ft);
    m_anchorNumber->setSpecialValueText("— —");
    m_anchorNumber->setEnabledEmbedStyle(true);


    m_radiusLabel = new DLabel(this);
    m_radiusLabel->setText(tr("Diameter"));
    m_radiusLabel->setFont(ft1);
    m_radiusNumber = new CSpinBox(this);
    m_radiusNumber->setKeyboardTracking(false);
    m_radiusNumber->setRange(-1, 1000);
    m_radiusNumber->setFixedSize(QSize(140, 36));
    m_radiusNumber->setSuffix("%");
    m_radiusNumber->setFont(ft);
    m_radiusNumber->setSpecialValueText("— —");
    m_radiusNumber->setEnabledEmbedStyle(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_strokeBtn);
    layout->addSpacing(SEPARATE_SPACING);
//    layout->addWidget(m_lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addWidget(m_sepLine);
    layout->addWidget(m_anchorNumLabel);
    layout->addWidget(m_anchorNumber);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_radiusLabel);
    layout->addWidget(m_radiusNumber);

    layout->addStretch();
    setLayout(layout);
}

void PolygonalStarAttributeWidget::initConnection()
{
    connect(m_fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_strokeBtn->resetChecked();
        emit showColorPanel(DrawStatus::Fill, getBtnPosition(m_fillBtn), show);

    });
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke,  getBtnPosition(m_strokeBtn), show);
    });

    connect(this, &PolygonalStarAttributeWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
        m_strokeBtn->resetChecked();
    });

    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthMenuShow, this, [ = ] () {
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });
    //描边粗细
    connect(m_sideWidthWidget, SIGNAL(signalSideWidthChoosed(int)), this, SLOT(slotSideWidthChoosed(int)));

    //锚点数
    connect(m_anchorNumber, SIGNAL(valueChanged(int)), this, SLOT(slotAnchorvalueChanged(int)));
    connect(m_anchorNumber, &CSpinBox::focusChanged, this, [ = ] (bool isFocus) {
        emit signalAnchorvalueIsfocus(isFocus);
    });
    connect(m_anchorNumber, &DSpinBox::editingFinished, this, [ = ] () {
        //等于0时是特殊字符，不做处理
        qDebug() << "m_anchorNumber->value() = " << m_anchorNumber->value();
        if ( m_anchorNumber->value() == 0) {
            return ;
        }
        m_anchorNumber->blockSignals(true);
        if (m_anchorNumber->value() < 3) {
            m_anchorNumber->setValue(3);
        } else if (m_anchorNumber->value() > 50) {
            m_anchorNumber->setValue(50);
        }
        m_anchorNumber->blockSignals(false);

        QVariant preValue = m_anchorNumber->property("preValue");

        if (preValue.isValid()) {
            int preIntValue = preValue.toInt();
            int curValue    = m_anchorNumber->value();
            if (preIntValue == curValue)
                return;
        }
        m_anchorNumber->setProperty("preValue", m_anchorNumber->value());

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAnchorNum(m_anchorNumber->value());
        emit signalPolygonalStarAttributeChanged();
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
        //设置多选图元属性
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::Anchors, m_anchorNumber->value());


    });

    //半径
    connect(m_radiusNumber, SIGNAL(valueChanged(int)), this, SLOT(slotRadiusvalueChanged(int)));
    connect(m_radiusNumber, &CSpinBox::focusChanged, this, [ = ] (bool isFocus) {
        emit signalRadiusvalueIsfocus(isFocus);
    });
    connect(m_radiusNumber, &DSpinBox::editingFinished, this, [ = ] () {
        m_radiusNumber->blockSignals(true);
        if (m_radiusNumber->value() < 0) {
            m_radiusNumber->setValue(0);
        } else if (m_radiusNumber->value() > 100) {
            m_radiusNumber->setValue(100);
        }
        m_radiusNumber->blockSignals(false);

        QVariant preValue = m_radiusNumber->property("preValue");

        if (preValue.isValid()) {
            int preIntValue = preValue.toInt();
            int curValue    = m_radiusNumber->value();
            if (preIntValue == curValue)
                return;
        }
        m_radiusNumber->setProperty("preValue", m_radiusNumber->value());


        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setRadiusNum(m_radiusNumber->value());
        emit signalPolygonalStarAttributeChanged();
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
        //设置多选图元属性
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::StarRadius, m_radiusNumber->value());
    });


    // 锚点数
    m_anchorNumber->setValue(5);
    m_radiusNumber->setValue(50);
    m_anchorNumber->setProperty("preValue", 5);
    m_radiusNumber->setProperty("preValue", 50);
}

void PolygonalStarAttributeWidget::updatePolygonalStarWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    int anchorNum = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getAnchorNum();

    if (anchorNum != m_anchorNumber->value()) {
        m_anchorNumber->blockSignals(true);
        m_anchorNumber->setValue(anchorNum);
        m_anchorNumber->blockSignals(false);
    }

    int radiusNum = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getRadiusNum();

    if (radiusNum != m_radiusNumber->value()) {
        m_radiusNumber->blockSignals(true);
        m_radiusNumber->setValue(radiusNum);
        m_radiusNumber->blockSignals(false);
    }

    m_fillBtn->setVisible(true);
    m_strokeBtn->setVisible(true);
    m_sepLine->setVisible(true);
//    m_lwLabel->setVisible(true);
    m_sideWidthWidget->setVisible(true);
    m_anchorNumLabel->setVisible(true);
    m_anchorNumber->setVisible(true);
    m_radiusLabel->setVisible(true);
    m_radiusNumber->setVisible(true);
    //CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void PolygonalStarAttributeWidget::slotAnchorvalueChanged(int value)
{
    m_anchorNumber->blockSignals(true);
    if (m_anchorNumber->value() < 3) {
        m_anchorNumber->setValue(3);
    } else if (m_anchorNumber->value() > 50) {
        m_anchorNumber->setValue(50);
    }
    m_anchorNumber->blockSignals(false);

    if (!m_anchorNumber->isChangedByWheelEnd()) {
        QVariant preValue = m_anchorNumber->property("preValue");

        if (preValue.isValid()) {
            int preIntValue = preValue.toInt();
            int curValue    = m_anchorNumber->value();
            if (preIntValue == curValue)
                return;
        }
        m_anchorNumber->setProperty("preValue", m_anchorNumber->value());
    }

    value = m_anchorNumber->value();
    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr)
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAnchorNum(value);
    emit signalPolygonalStarAttributeChanged();
    //隐藏调色板
    showColorPanel(DrawStatus::Stroke, QPoint(), false);


    //设置多选图元属性
    static QMap<CGraphicsItem *, QVariant> s_oldTempValues;
    bool pushToStack = !m_anchorNumber->isTimerRunning();
    bool firstRecord = s_oldTempValues.isEmpty();
    QMap<CGraphicsItem *, QVariant> *inUndoValues = m_anchorNumber->isChangedByWheelEnd() ? &s_oldTempValues : nullptr;
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::Anchors, value, pushToStack, ((!pushToStack && firstRecord) ? &s_oldTempValues : nullptr), inUndoValues);
    m_anchorNumber->setProperty("preValue", m_anchorNumber->value());

    if (m_anchorNumber->isChangedByWheelEnd()) {
        s_oldTempValues.clear();
    }

    //设置多选图元属性
    //CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::Anchors, value);
}

void PolygonalStarAttributeWidget::slotRadiusvalueChanged(int value)
{
    m_radiusNumber->blockSignals(true);
    if (m_radiusNumber->value() < 0) {
        m_radiusNumber->setValue(0);
    } else if (m_radiusNumber->value() > 100) {
        m_radiusNumber->setValue(100);
    }
    m_radiusNumber->blockSignals(false);

    if (!m_radiusNumber->isChangedByWheelEnd()) {
        QVariant preValue = m_radiusNumber->property("preValue");

        if (preValue.isValid()) {
            int preIntValue = preValue.toInt();
            int curValue    = m_radiusNumber->value();
            if (preIntValue == curValue)
                return;
        }
        m_radiusNumber->setProperty("preValue", m_radiusNumber->value());
    }

    value = m_radiusNumber->value();
    if (CManageViewSigleton::GetInstance()->getCurView() != nullptr) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setRadiusNum(value);
        emit signalPolygonalStarAttributeChanged();
    }

    //隐藏调色板
    showColorPanel(DrawStatus::Stroke, QPoint(), false);
    //设置多选图元属性
    //CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::StarRadius, value);
    static QMap<CGraphicsItem *, QVariant> s_oldTempValues;
    bool pushToStack = !m_radiusNumber->isTimerRunning();
    bool firstRecord = s_oldTempValues.isEmpty();
    QMap<CGraphicsItem *, QVariant> *inUndoValues = m_radiusNumber->isChangedByWheelEnd() ? &s_oldTempValues : nullptr;
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::StarRadius, value, pushToStack, ((!pushToStack && firstRecord) ? &s_oldTempValues : nullptr), inUndoValues);
    m_radiusNumber->setProperty("preValue", m_radiusNumber->value());

    if (m_radiusNumber->isChangedByWheelEnd()) {
        s_oldTempValues.clear();
    }

    m_radiusNumber->setFocus(Qt::MouseFocusReason);

    //m_radiusNumber->setProperty("preValue", m_radiusNumber->value());
}

void PolygonalStarAttributeWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
    this->setFocus();
}

QPoint PolygonalStarAttributeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
