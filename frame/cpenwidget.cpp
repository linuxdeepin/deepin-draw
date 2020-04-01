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
const int TEXT_SIZE = 12;


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

    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
    // lock signal
    m_lineStartComboBox->blockSignals(true);
    m_lineEndComboBox->blockSignals(true);
    qDebug() << "theme: " << themeType;
    if (1 == themeType) { // light style
        m_lineStartComboBox->setItemIcon(0, QIcon(QPixmap(":/theme/light/images/combobox_startline_none_light.svg")));
        m_lineStartComboBox->setItemIcon(1, QIcon(QPixmap(":/theme/light/images/menu_line_arrow_light_start.svg")));
        m_lineStartComboBox->setItemIcon(2, QIcon(QPixmap(":/theme/light/images/menu_line_solid arrow_light_start.svg")));
        m_lineStartComboBox->setItemIcon(3, QIcon(QPixmap(":/theme/light/images/menu_line_ring_light_start.svg")));
        m_lineStartComboBox->setItemIcon(4, QIcon(QPixmap(":/theme/light/images/menu_line_round_light_start.svg")));
        m_lineEndComboBox->setItemIcon(0, QIcon(QPixmap(":/theme/light/images/combobox_startline_none_light.svg")));
        m_lineEndComboBox->setItemIcon(1, QIcon(QPixmap(":/theme/light/images/menu_finishline_arrow_light_end.svg")));
        m_lineEndComboBox->setItemIcon(2, QIcon(QPixmap(":/theme/light/images/menu_finishline_solid arrow_light_end.svg")));
        m_lineEndComboBox->setItemIcon(3, QIcon(QPixmap(":/theme/light/images/menu_finishline_ring_light_end.svg")));
        m_lineEndComboBox->setItemIcon(4, QIcon(QPixmap(":/theme/light/images/menu_finishline_round_light_end.svg")));
    } else { // dark style
        m_lineStartComboBox->setItemIcon(0, QIcon(QPixmap(":/theme/dark/images/combobox_startline_none_black.svg")));
        m_lineStartComboBox->setItemIcon(1, QIcon(QPixmap(":/theme/dark/images/menu_line_arrow_black_start.svg")));
        m_lineStartComboBox->setItemIcon(2, QIcon(QPixmap(":/theme/dark/images/menu_line_solid arrow_black_start.svg")));
        m_lineStartComboBox->setItemIcon(3, QIcon(QPixmap(":/theme/dark/images/menu_line_ring_black_start.svg")));
        m_lineStartComboBox->setItemIcon(4, QIcon(QPixmap(":/theme/dark/images/menu_line_round_black_start.svg")));
        m_lineEndComboBox->setItemIcon(0, QIcon(QPixmap(":/theme/dark/images/combobox_startline_none_black.svg")));
        m_lineEndComboBox->setItemIcon(1, QIcon(QPixmap(":/theme/dark/images/menu_finishline_arrow_light_end.svg")));
        m_lineEndComboBox->setItemIcon(2, QIcon(QPixmap(":/theme/dark/images/combobox_finishline_solid arrow_light_end.svg")));
        m_lineEndComboBox->setItemIcon(3, QIcon(QPixmap(":/theme/dark/images/combobox_finishline_ring_light_end.svg")));
        m_lineEndComboBox->setItemIcon(4, QIcon(QPixmap(":/theme/dark/images/combobox_finishline_round_light_end.svg")));
    }
    m_lineStartComboBox->blockSignals(false);
    m_lineEndComboBox->blockSignals(false);
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
        case PenStartArrowType:
            m_startLabel->setVisible(true);
            m_lineStartComboBox->setVisible(true);
            m_lineStartComboBox->setCurrentIndex(propertys[property].toInt());
            m_sideWidthWidget->update();
            break;
        case PenEndArrowType:
            m_endLabel->setVisible(true);
            m_lineEndComboBox->setVisible(true);
            m_lineEndComboBox->setCurrentIndex(propertys[property].toInt());
            m_lineEndComboBox->update();
            break;
        default:
            break;
        }
    }
}

void CPenWidget::initUI()
{
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);

    m_startLabel = new DLabel(this);
    m_endLabel = new DLabel(this);
    m_startLabel->setText(tr("start"));
    m_startLabel->setFont(ft);
    m_endLabel->setText(tr("end"));
    m_endLabel->setFont(ft);

    m_strokeBtn = new BorderColorButton(this);
    m_sep1Line = new SeperatorLine(this);
    m_sideWidthWidget = new CSideWidthWidget(this);
    m_sideWidthWidget->setFont(ft);
    m_sideWidthWidget->setFixedWidth(100);

    m_lineStartComboBox = new DComboBox(this);
    m_lineStartComboBox->setFixedSize(QSize(70, 36));
    m_lineStartComboBox->setIconSize(QSize(24, 10));
    m_lineStartComboBox->addItem("");
    m_lineStartComboBox->addItem("");
    m_lineStartComboBox->addItem("");
    m_lineStartComboBox->addItem("");
    m_lineStartComboBox->addItem("");

    m_lineEndComboBox =  new DComboBox(this);
    m_lineEndComboBox->setFixedSize(QSize(70, 36));
    m_lineEndComboBox->setIconSize(QSize(24, 10));
    m_lineEndComboBox->addItem("");
    m_lineEndComboBox->addItem("");
    m_lineEndComboBox->addItem("");
    m_lineEndComboBox->addItem("");
    m_lineEndComboBox->addItem("");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_strokeBtn);
    layout->addWidget(m_sideWidthWidget);
    layout->addWidget(m_sep1Line, 0, Qt::AlignCenter);
    layout->addWidget(m_startLabel);
    layout->addWidget(m_lineStartComboBox);
    layout->addWidget(m_endLabel);
    layout->addWidget(m_lineEndComboBox);
//    layout->setSpacing(BTN_SPACNT);
    layout->addSpacing(16);
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

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPenStartType(lineType);
        CManagerAttributeService::getInstance()->setPenStartType(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), lineType);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::PenStartArrowType, lineType);
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

        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPenEndType(lineType);
        CManagerAttributeService::getInstance()->setPenEndType(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), lineType);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::PenEndArrowType, lineType);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });

    m_lineStartComboBox->setCurrentIndex(0);
    m_lineStartComboBox->setCurrentIndex(0);
}

void CPenWidget::updatePenWidget()
{
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void CPenWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
}
