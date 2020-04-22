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
#include "cpenwidget.h"

#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/ccheckbutton.h"
#include "widgets/dmenucombobox.h"

#include "drawshape/cdrawparamsigleton.h"

#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "service/cmanagerattributeservice.h"

#include <QHBoxLayout>
#include <QDebug>

#include <DGuiApplicationHelper>
#include <DComboBox>


DGUI_USE_NAMESPACE

const int BTN_SPACNT = 10;
const int TEXT_SIZE = 14;


CPenWidget::CPenWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CPenWidget::~CPenWidget()
{

}

void CPenWidget::changeButtonTheme()
{
    m_sideWidthWidget->changeButtonTheme();
    m_sep1Line->updateTheme();
}

void CPenWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_strokeBtn->setVisible(false);
    m_sideWidthWidget->setVisible(false);

    m_startLabel->setVisible(false);
    m_endLabel->setVisible(false);
    m_lineStartComboBox->setVisible(false);
    m_lineEndComboBox->setVisible(false);
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
        case LineColor: {
            m_strokeBtn->setVisible(true);
            m_strokeBtn->blockSignals(true);
            QColor color = propertys[property].value<QColor>();
            if (!color.isValid()) {
                m_strokeBtn->setIsMultColorSame(false);
            } else {
                m_strokeBtn->setColor(color);
            }
            m_strokeBtn->update();
            m_strokeBtn->blockSignals(false);
            break;
        }
        case LineWidth:
            m_sideWidthWidget->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideWidthWidget->setMenuNoSelected(true);
            } else {
                m_sideWidthWidget->setSideWidth(propertys[property].toInt());
            }
            m_sideWidthWidget->update();
            break;
        case LineAndPenStartType: {
            m_lineStartComboBox->blockSignals(true);
            m_lineStartComboBox->setCurrentIndex(propertys[property].toInt());
            m_lineStartComboBox->blockSignals(false);
            break;
        }
        case LineAndPenEndType: {
            m_lineEndComboBox->blockSignals(true);
            m_lineEndComboBox->setCurrentIndex(propertys[property].toInt());
            m_lineEndComboBox->blockSignals(false);
            break;
        }
        default:
            break;
        }
    }
}

void CPenWidget::initUI()
{
    m_strokeBtn = new BorderColorButton(this);
    m_strokeBtn->setButtonText(tr("Color"));

    m_sep1Line = new SeperatorLine(this);
    m_sideWidthWidget = new CSideWidthWidget(this);
    m_sideWidthWidget->setFixedWidth(100);

    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
//    strokeLabel->setFont(ft);
    m_strokeBtn->setButtonText(tr("Color"));

    m_startLabel = new DLabel(this);
    m_endLabel = new DLabel(this);
    m_startLabel->setText(tr("Start"));
    m_startLabel->setFont(ft);
    m_endLabel->setText(tr("End"));
    m_endLabel->setFont(ft);

    m_sideWidthWidget->setFont(ft);
    m_sideWidthWidget->setFixedWidth(100);

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

void CPenWidget::initConnection()
{
    // 线颜色
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        QPoint btnPos = mapToGlobal(m_strokeBtn->pos());
        QPoint pos(btnPos.x() + m_strokeBtn->width() / 2,
                   btnPos.y() + m_strokeBtn->height());
        showColorPanel(DrawStatus::Stroke, pos, show);
    });
    connect(this, &CPenWidget::resetColorBtns, this, [ = ] {
        m_strokeBtn->resetChecked();
    });

    // 线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalPenAttributeChanged();
    });
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthMenuShow, this, [ = ] () {
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });
    //描边粗细
    connect(m_sideWidthWidget, SIGNAL(signalSideWidthChoosed(int)), this, SLOT(slotSideWidthChoosed(int)));

    // 起点箭头样式
    connect(m_lineStartComboBox, QOverload<int>::of(&DComboBox::currentIndexChanged), this, [ = ](int index) {
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
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPenStartType(lineType);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineAndPenStartType, lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    // 终点箭头样式
    connect(m_lineEndComboBox, QOverload<int>::of(&DComboBox::currentIndexChanged), this, [ = ](int index) {
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
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPenEndType(lineType);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineAndPenEndType, lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    m_lineStartComboBox->setCurrentIndex(0);
    m_lineEndComboBox->setCurrentIndex(0);

    // 连接单个画笔点击属性更新
    connect(CManagerAttributeService::getInstance(), &CManagerAttributeService::signalPenItemPropertyUpdate, this, [ = ](QMap<EDrawProperty, QVariant> propertys) {
        QMap<EDrawProperty, QVariant>::iterator itr = propertys.begin();
        for (; itr != propertys.end(); itr++) {
            switch (itr.key()) {
            case LineAndPenStartType: {
                int startIndex = itr.value().toInt();
                qDebug() << "startIndex: " << startIndex;
                m_lineStartComboBox->blockSignals(true);
                m_lineStartComboBox->setCurrentIndex(startIndex);
                m_lineStartComboBox->blockSignals(false);
                break;
            }
            case LineAndPenEndType: {
                int endIndex = itr.value().toInt();
                qDebug() << "endIndex: " << endIndex;
                m_lineEndComboBox->blockSignals(true);
                m_lineEndComboBox->setCurrentIndex(endIndex);
                m_lineEndComboBox->blockSignals(false);
                break;
            }
            default: {
                break;
            }
            }
        }
    });
}

void CPenWidget::updatePenWidget()
{
    // 填充色更新
    m_strokeBtn->updateConfigColor();

    // 线宽度属性刷新
    m_sideWidthWidget->blockSignals(true);
    m_sideWidthWidget->updateSideWidth();
    m_sideWidthWidget->blockSignals(false);
}

void CPenWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
}
