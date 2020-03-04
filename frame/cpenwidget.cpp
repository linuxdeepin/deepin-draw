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
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include <QHBoxLayout>
#include <QDebug>

#include <DGuiApplicationHelper>


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
    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
    m_straightline->setCurrentTheme(themeType);
    m_arrowline->setCurrentTheme(themeType);
    m_sep1Line->updateTheme();
}

void CPenWidget::initUI()
{
//    DLabel *strokeLabel = new DLabel(this);
//    strokeLabel->setObjectName("StrokeLabel");
//    strokeLabel->setText(tr("描边"));
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
//    strokeLabel->setFont(ft);
    m_strokeBtn = new BorderColorButton(this);

    m_sep1Line = new SeperatorLine(this);

    DLabel *lineTypeLabel = new DLabel(this);
    lineTypeLabel->setObjectName("Line Type");
    lineTypeLabel->setText(tr("Type"));
    lineTypeLabel->setFont(ft);


    QMap<int, QMap<CCheckButton::EButtonSattus, QString> > pictureMap;

    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/attribute/line tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/attribute/line tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/attribute/line tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/attribute/line tool_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/attribute/line tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/attribute/line tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/attribute/line tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/attribute/line tool_checked.svg");

    m_straightline = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_actionButtons.append(m_straightline);


    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Normal] = QString(":/theme/light/images/attribute/arrow tool_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Hover] = QString(":/theme/light/images/attribute/arrow tool_hover.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Press] = QString(":/theme/light/images/attribute/arrow tool_press.svg");
    pictureMap[DGuiApplicationHelper::LightType][CCheckButton::Active] = QString(":/theme/light/images/attribute/arrow tool_checked.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Normal] = QString(":/theme/dark/images/attribute/arrow tool_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Hover] = QString(":/theme/dark/images/attribute/arrow tool_hover.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Press] = QString(":/theme/dark/images/attribute/arrow tool_press.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CCheckButton::Active] = QString(":/theme/dark/images/attribute/arrow tool_checked.svg");

    m_arrowline = new CCheckButton(pictureMap, QSize(36, 36), this);
    m_actionButtons.append(m_arrowline);

    DLabel *lwLabel = new DLabel(this);
    lwLabel->setObjectName("Border Label");
    lwLabel->setText(tr("Width"));
    QFont ft1;
    ft1.setPixelSize(TEXT_SIZE - 1);
    lwLabel->setFont(ft1);

    m_sideWidthWidget = new CSideWidthWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(lineTypeLabel);
    layout->addWidget(m_straightline);
    layout->addWidget(m_arrowline);
    layout->setSpacing(BTN_SPACNT);
    layout->addSpacing(16);
    layout->addWidget(m_strokeBtn);
    // layout->addWidget(strokeLabel);

    layout->addWidget(m_sep1Line, 0, Qt::AlignCenter);

    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addStretch();
    setLayout(layout);
}

void CPenWidget::initConnection()
{
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {

        QPoint btnPos = mapToGlobal(m_strokeBtn->pos());
        QPoint pos(btnPos.x() + m_strokeBtn->width() / 2,
                   btnPos.y() + m_strokeBtn->height());

        showColorPanel(DrawStatus::Stroke, pos, show);
    });

    connect(this, &CPenWidget::resetColorBtns, this, [ = ] {
        m_strokeBtn->resetChecked();
    });


    connect(m_straightline, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_straightline);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentPenType(EPenType::straight);
        emit signalPenAttributeChanged();
    });

    connect(m_arrowline, &CCheckButton::buttonClick, [this]() {
        clearOtherSelections(m_arrowline);
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCurrentPenType(EPenType::arrow);
        emit signalPenAttributeChanged();
    });

    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalPenAttributeChanged();
    });
}

void CPenWidget::clearOtherSelections(CCheckButton *clickedButton)
{
    foreach (CCheckButton *button, m_actionButtons) {
        if (button->isChecked() && button != clickedButton) {
            button->setChecked(false);
            return;
        }
    };
}

void CPenWidget::updatePenWidget()
{
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();
    EPenType penType = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentPenType();
    if (penType == EPenType::straight) {
        if (!m_straightline->isChecked()) {
            m_straightline->setChecked(true);
            m_arrowline->setChecked(false);
        }
    } else if (penType == EPenType::arrow) {
        if (!m_arrowline->isChecked()) {
            m_arrowline->setChecked(true);
            m_straightline->setChecked(false);
        }
    }
}
