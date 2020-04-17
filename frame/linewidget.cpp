/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renranv
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
#include "linewidget.h"
#include "drawshape/cdrawparamsigleton.h"
#include "drawshape/cdrawscene.h"

#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/toolbutton.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/ccheckbutton.h"
#include "widgets/dmenucombobox.h"

#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "service/cmanagerattributeservice.h"

#include <QHBoxLayout>
#include <QDebug>

#include <DComboBox>

const int BTN_SPACNT = 10;
const int TEXT_SIZE = 14;

LineWidget::LineWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

LineWidget::~LineWidget()
{
}

void LineWidget::changeButtonTheme()
{
    m_sideWidthWidget->changeButtonTheme();
    m_sep1Line->updateTheme();
}

void LineWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_strokeBtn->setVisible(false);
    m_sideWidthWidget->setVisible(false);
    m_sep1Line->setVisible(false);

    m_startLabel->setVisible(false);
    m_endLabel->setVisible(false);
    m_lineStartComboBox->setVisible(false);
    m_lineEndComboBox->setVisible(false);
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
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
        case LineAndPenStartType:
            m_sep1Line->setVisible(true);
            m_startLabel->setVisible(true);
            m_maskLableStart->setVisible(false);
            m_lineStartComboBox->setVisible(true);
            m_lineStartComboBox->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_lineStartComboBox->setCurrentIndex(-1);
                m_maskLableStart->setVisible(true);
            } else {
                m_lineStartComboBox->setCurrentIndex(propertys[property].toInt());
            }
            m_lineStartComboBox->blockSignals(false);
            m_lineStartComboBox->update();
            break;
        case LineAndPenEndType:
            m_sep1Line->setVisible(true);
            m_endLabel->setVisible(true);
            m_lineEndComboBox->setVisible(true);
            m_lineEndComboBox->blockSignals(true);
            m_maskLableEnd->setVisible(false);
            if (propertys[property].type() == QVariant::Invalid) {
                m_lineEndComboBox->setCurrentIndex(-1);
                m_maskLableEnd->setVisible(true);
            } else {
                m_lineEndComboBox->setCurrentIndex(propertys[property].toInt());
            }
            m_lineEndComboBox->blockSignals(false);
            m_lineEndComboBox->update();
            break;
        default:
            break;
        }
    }
}

void LineWidget::initUI()
{
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);

    m_startLabel = new DLabel(this);
    m_endLabel = new DLabel(this);
    m_startLabel->setText(tr("Start"));
    m_startLabel->setFont(ft);
    m_endLabel->setText(tr("End"));
    m_endLabel->setFont(ft);

    m_strokeBtn = new BorderColorButton(this);
    m_strokeBtn->setButtonText(tr("Color"));
    m_sep1Line = new SeperatorLine(this);
    m_sideWidthWidget = new CSideWidthWidget(this);
    m_sideWidthWidget->setFixedWidth(100);
    m_sideWidthWidget->setFont(ft);

    m_lineStartComboBox = new DComboBox(this);
    m_lineStartComboBox->setFixedSize(QSize(90, 36));
    m_lineStartComboBox->setIconSize(QSize(34, 20));

    m_lineEndComboBox =  new DComboBox(this);
    m_lineEndComboBox->setFixedSize(QSize(90, 36));
    m_lineEndComboBox->setIconSize(QSize(34, 20));

    m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_none_arrow"), "");
    m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_circle"), "");
    m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_fill_circle"), "");
    m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_arrow"), "");
    m_lineStartComboBox->addItem(QIcon::fromTheme("ddc_right_fill_arrow"), "");
    m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_none_arrow"), "");
    m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_left_circle"), "");
    m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_left_fill_circle"), "");
    m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_left_arrow"), "");
    m_lineEndComboBox->addItem(QIcon::fromTheme("ddc_left_fill_arrow"), "");

    m_maskLableStart = new DLabel(m_lineStartComboBox);
    m_maskLableStart->setText("— —");
    m_maskLableStart->move(6, 6);
    m_maskLableStart->setFixedSize(35, 20);
    m_maskLableStart->setVisible(false);
    m_maskLableStart->setFont(m_maskLableStart->font());

    m_maskLableEnd = new DLabel(m_lineEndComboBox);
    m_maskLableEnd->setText("— —");
    m_maskLableEnd->move(6, 6);
    m_maskLableEnd->setFixedSize(35, 20);
    m_maskLableEnd->setVisible(false);
    m_maskLableEnd->setFont(m_lineEndComboBox->font());

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_strokeBtn);
    layout->addSpacing(-6); // 此处设置的原因是宽度过大
    layout->addWidget(m_sideWidthWidget);
    layout->addWidget(m_sep1Line);
    layout->addWidget(m_startLabel);
    layout->addWidget(m_lineStartComboBox);
    layout->addSpacing(12);
    layout->addWidget(m_endLabel);
    layout->addWidget(m_lineEndComboBox);

    layout->addStretch();
    setLayout(layout);

    changeButtonTheme();
}

void LineWidget::initConnection()
{
    // 线颜色
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        QPoint btnPos = mapToGlobal(m_strokeBtn->pos());
        QPoint pos(btnPos.x() + m_strokeBtn->width() / 2,
                   btnPos.y() + m_strokeBtn->height());
        showColorPanel(DrawStatus::Stroke, pos, show);
    });
    connect(this, &LineWidget::resetColorBtns, this, [ = ] {
        m_strokeBtn->resetChecked();
    });

    // 线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalLineAttributeChanged();
    });
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthMenuShow, this, [ = ] () {
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });
    //描边粗细
    connect(m_sideWidthWidget, SIGNAL(signalSideWidthChoosed(int)), this, SLOT(slotSideWidthChoosed(int)));

    // 起点箭头样式
    connect(m_lineStartComboBox, QOverload<const int>::of(&DComboBox::currentIndexChanged), this, [ = ](int index) {
        ELineType lineType = noneLine;
        switch (index) {
        case 0: {
            lineType = noneLine;
            break;
        }
        case 1: {
            lineType = normalRing;
            break;
        }
        case 2: {
            lineType = soildRing;
            break;
        }
        case 3: {
            lineType = normalArrow;
            break;
        }
        case 4: {
            lineType = soildArrow;
            break;
        }
        }
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineStartType(lineType);
        CManagerAttributeService::getInstance()->setLineStartType(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), lineType);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineAndPenStartType, lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    // 终点箭头样式
    connect(m_lineEndComboBox, QOverload<const int>::of(&DComboBox::currentIndexChanged), this, [ = ](int index) {
        ELineType lineType = noneLine;
        switch (index) {
        case 0: {
            lineType = noneLine;
            break;
        }
        case 1: {
            lineType = normalRing;
            break;
        }
        case 2: {
            lineType = soildRing;
            break;
        }
        case 3: {
            lineType = normalArrow;
            break;
        }
        case 4: {
            lineType = soildArrow;
            break;
        }
        }

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineEndType(lineType);
        CManagerAttributeService::getInstance()->setLineEndType(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), lineType);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineAndPenEndType, lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
        m_maskLableEnd->setVisible(false);
    });

    m_lineStartComboBox->setCurrentIndex(0);
    m_lineStartComboBox->setCurrentIndex(0);
}

void LineWidget::updateLineWidget()
{
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    m_lineStartComboBox->blockSignals(true);
    m_lineStartComboBox->setCurrentIndex(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineStartType());
    m_lineStartComboBox->blockSignals(false);
    m_lineEndComboBox->blockSignals(true);
    m_lineEndComboBox->setCurrentIndex(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getLineEndType());
    m_lineEndComboBox->blockSignals(false);

    m_maskLableStart->setVisible(false);
    m_maskLableEnd->setVisible(false);

    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void LineWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
}






