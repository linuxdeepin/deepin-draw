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

#include <DLabel>
#include <DFontSizeManager>

#include <QAction>
#include <QHBoxLayout>
#include <QButtonGroup>

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

void PolygonAttributeWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_fillBtn->setVisible(false);
    m_strokeBtn->setVisible(false);
    m_sepLine->setVisible(false);
//    m_lwLabel->setVisible(false);
    m_sideWidthWidget->setVisible(false);
    m_sideNumLabel->setVisible(false);
    m_sideNumSlider->setVisible(false);
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
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideWidthWidget->setMenuNoSelected();
            } else {
                m_sideWidthWidget->setSideWidth(propertys[property].toInt());
            }
            m_sideWidthWidget->update();
            break;
        case SideNumber:
            m_sideNumLabel->setVisible(true);
            m_sideNumSlider->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideNumSlider->blockSignals(true);
                m_sideNumSlider->setValue(0);
                m_sideNumSlider->blockSignals(false);
            } else {
                m_sideNumSlider->setValue(propertys[property].toInt());
            }
            break;
        default:
            break;
        }
    }
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

    m_sideNumSlider = new CSpinBox(this);
    m_sideNumSlider->setKeyboardTracking(false);
    m_sideNumSlider->setFixedWidth(70);
    m_sideNumSlider->setRange(0, 1000);
    //m_sideNumSlider->setRange(3, 10);//此注释不删，记录边数范围
    m_sideNumSlider->setFont(ft);
    m_sideNumSlider->setSpecialValueText("— —");

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
    layout->addWidget(m_sideNumSlider);

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

    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalPolygonAttributeChanged();
    });
    //描边粗细
    connect(m_sideWidthWidget, SIGNAL(signalSideWidthChoosed(int)), this, SLOT(slotSideWidthChoosed(int)));
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthMenuShow, this, [ = ] () {
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    ///多边形边数
    connect(m_sideNumSlider, SIGNAL(valueChanged(int)), this, SLOT(slotSideValueChanged(int)));
    m_sideNumSlider->setValue(5);
    connect(m_sideNumSlider, &CSpinBox::focusChanged, this, [ = ] (bool isFocus) {
        emit signalSideValueIsfocus(isFocus);
    });
    connect(m_sideNumSlider, &DSpinBox::editingFinished, this, [ = ] () {
        m_sideNumSlider->blockSignals(true);
        if (m_sideNumSlider->value() < 3) {
            m_sideNumSlider->setValue(3);
        } else if (m_sideNumSlider->value() > 10) {
            m_sideNumSlider->setValue(10);
        }
        m_sideNumSlider->blockSignals(false);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSideNum(m_sideNumSlider->value());
        emit signalPolygonAttributeChanged();
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::SideNumber, m_sideNumSlider->value());
    });
}

void PolygonAttributeWidget::updatePolygonWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    int sideNum = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSideNum();

    if (sideNum != m_sideNumSlider->value()) {
        m_sideNumSlider->setValue(sideNum);
    }

    m_fillBtn->setVisible(true);
    m_strokeBtn->setVisible(true);
    m_sepLine->setVisible(true);
//    m_lwLabel->setVisible(true);
    m_sideWidthWidget->setVisible(true);
    m_sideNumLabel->setVisible(true);
    m_sideNumSlider->setVisible(true);
    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void PolygonAttributeWidget::slotSideValueChanged(int value)
{
    m_sideNumSlider->blockSignals(true);
    if (m_sideNumSlider->value() < 3) {
        m_sideNumSlider->setValue(3);
    } else if (m_sideNumSlider->value() > 10) {
        m_sideNumSlider->setValue(10);
    }
    m_sideNumSlider->blockSignals(false);
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSideNum(m_sideNumSlider->value());
    emit signalPolygonAttributeChanged();
    //隐藏调色板
    showColorPanel(DrawStatus::Stroke, QPoint(), false);
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::SideNumber, m_sideNumSlider->value());
}

void PolygonAttributeWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
}

QPoint PolygonAttributeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
