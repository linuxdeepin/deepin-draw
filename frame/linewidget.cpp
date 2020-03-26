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

const int BTN_SPACNT = 10;
const int TEXT_SIZE = 12;

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

    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
//    m_straightline->setCurrentTheme(themeType);
//    m_arrowline->setCurrentTheme(themeType);
    // lock signal
    m_lineStartComboBox->blockSignals(true);
    m_lineEndComboBox->blockSignals(true);
    m_lineStartComboBox->cleanAllMenuItem();
    m_lineEndComboBox->cleanAllMenuItem();
    if (1 == themeType) { // deep style
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/combobox_startline_none_light.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_line_arrow_light_start.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_line_solid arrow_light_start.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_line_ring_light_start.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_line_round_light_start.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/combobox_startline_none_light.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_finishline_arrow_light_end.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_finishline_solid arrow_light_end.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_finishline_ring_light_end.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/light/images/menu_finishline_round_light_end.svg")));
    } else { // clearly
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/combobox_startline_none_black.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/menu_line_arrow_black_start.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/menu_line_solid arrow_black_start.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/menu_line_ring_black_start.svg")));
        m_lineStartComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/menu_line_round_black_start.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/combobox_startline_none_black.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/menu_finishline_arrow_light_end.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/combobox_finishline_solid arrow_light_end.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/combobox_finishline_ring_light_end.svg")));
        m_lineEndComboBox->addItem("", QIcon(QPixmap(":/theme/dark/images/combobox_finishline_round_light_end.svg")));
    }
    m_lineStartComboBox->blockSignals(false);
    m_lineEndComboBox->blockSignals(false);
}

void LineWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
//    m_lineTypeLabel->setVisible(false);
//    m_straightline->setVisible(false);
//    m_arrowline->setVisible(false);
    m_strokeBtn->setVisible(false);
//    m_lwLabel->setVisible(false);
    m_sideWidthWidget->setVisible(false);
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
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideWidthWidget->setMenuButtonICon("—— ——", QIcon());
            } else {
                m_sideWidthWidget->setSideWidth(propertys[property].toInt());
            }
            m_sideWidthWidget->update();
            break;
        case LineArrowType:
//            m_lineTypeLabel->setVisible(true);
//            m_straightline->setVisible(true);
//            m_arrowline->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
//                m_straightline->setChecked(false);
//                m_arrowline->setChecked(false);
            } else {
                if (propertys[property].value<ELineType>() == straightType) {
//                    m_straightline->setChecked(true);
                } else {
//                    m_arrowline->setChecked(true);
                }
            }
            m_sideWidthWidget->update();
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

    DLabel *startLabel = new DLabel(this);
    DLabel *endLabel = new DLabel(this);
    startLabel->setText(tr("start"));
    startLabel->setFont(ft);
    endLabel->setText(tr("end"));
    endLabel->setFont(ft);

    m_strokeBtn = new BorderColorButton(this);
    m_sep1Line = new SeperatorLine(this);
    m_sideWidthWidget = new CSideWidthWidget(this);
    m_lineStartComboBox = new DMenuComboBox(this);
    m_lineStartComboBox->setFixedSize(QSize(70, 36));
    m_lineStartComboBox->setMenuMaxWidth(70);
    m_lineEndComboBox =  new DMenuComboBox(this);
    m_lineEndComboBox->setFixedSize(QSize(70, 36));
    m_lineEndComboBox->setMenuMaxWidth(70);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_strokeBtn);
    layout->addWidget(m_sideWidthWidget);
    layout->addWidget(m_sep1Line, 0, Qt::AlignCenter);
    layout->addWidget(startLabel);
    layout->addWidget(m_lineStartComboBox);
    layout->addWidget(endLabel);
    layout->addWidget(m_lineEndComboBox);
    layout->setSpacing(BTN_SPACNT);
    layout->addSpacing(16);
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

//    connect(m_straightline, &CCheckButton::buttonClick, [this]() {
//        clearOtherSelections(m_straightline);
//        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineType(straightType);
//        emit signalLineAttributeChanged();
//        //隐藏调色板
//        showColorPanel(DrawStatus::Stroke, QPoint(), false);
//    });

    // 起点箭头样式
    connect(m_lineStartComboBox, &DMenuComboBox::signalCurrentIndexChanged, this, [ = ](int index) {
        ELineType lineType = noneLine;
        switch (index) {
        case 0: {
            lineType = noneLine;
            break;
        }
        case 1: {
            lineType = normalArrow;
            break;
        }
        case 2: {
            lineType = soildArrow;
            break;
        }
        case 3: {
            lineType = normalRing;
            break;
        }
        case 4: {
            lineType = soildRing;
            break;
        }
        }

        CManagerAttributeService::getInstance()->setLineStartType(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), lineType);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineStartType(lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    // 终点箭头样式
    connect(m_lineEndComboBox, &DMenuComboBox::signalCurrentIndexChanged, this, [ = ](int index) {
        ELineType lineType = noneLine;
        switch (index) {
        case 0: {
            lineType = noneLine;
            break;
        }
        case 1: {
            lineType = normalArrow;
            break;
        }
        case 2: {
            lineType = soildArrow;
            break;
        }
        case 3: {
            lineType = normalRing;
            break;
        }
        case 4: {
            lineType = soildRing;
            break;
        }
        }

        CManagerAttributeService::getInstance()->setLineEndType(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), lineType);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineEndType(lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    m_lineStartComboBox->setCurrentIndex(0);
    m_lineStartComboBox->setCurrentIndex(0);
}

void LineWidget::updateLineWidget()
{
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void LineWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
}






