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

#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/csidewidthwidget.h"
#include "utils/cvalidator.h"
#include "drawshape/cdrawparamsigleton.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 12;
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

void PolygonalStarAttributeWidget::initUI()
{
    //    DFontSizeManager::instance()->bind(this, DFontSizeManager::T1);

    m_fillBtn = new BigColorButton( this);

    DLabel *fillLabel = new DLabel(this);
    fillLabel->setText(tr("填充"));
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
    fillLabel->setFont(ft);

    m_strokeBtn = new BorderColorButton(this);

    DLabel *strokeLabel = new DLabel(this);
    strokeLabel->setText(tr("描边"));
    strokeLabel->setFont(ft);

    m_sepLine = new SeperatorLine(this);
    DLabel *lwLabel = new DLabel(this);
    lwLabel->setText(tr("描边粗细"));
    lwLabel->setFont(ft);

    m_sideWidthWidget = new CSideWidthWidget(this);


    DLabel *anchorNumLabel = new DLabel(this);
    anchorNumLabel->setText(tr("锚点数"));
    anchorNumLabel->setFont(ft);

    m_anchorNumSlider = new DSlider(Qt::Horizontal, this);

    //获取屏幕分辨率
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    int width;
    width = screenRect.width();


    m_anchorNumSlider->setMinimum(3);
    m_anchorNumSlider->setMaximum(200);
    if (width < 1152) {
        m_anchorNumSlider->setFixedWidth(60);
    } else {
        m_anchorNumSlider->setFixedWidth(120);
    }

    m_anchorNumSlider->setMaximumHeight(24);

    m_anchorNumEdit = new DLineEdit(this);
    m_anchorNumEdit->lineEdit()->setValidator(new CIntValidator(3, 200));
//    m_anchorNumEdit->setValidator(new QRegExpValidator(QRegExp("^(([3-9]{1})|(^[1-4]{1}[0-9]{1}$)|(50))$"), this));
    m_anchorNumEdit->setClearButtonEnabled(false);
    m_anchorNumEdit->setFixedWidth(40);
    m_anchorNumEdit->setText(QString::number(m_anchorNumSlider->value()));
    m_anchorNumEdit->setFont(ft);

    DLabel *radiusLabel = new DLabel(this);
    radiusLabel->setText(tr("半径"));
    radiusLabel->setFont(ft);

    m_radiusNumSlider = new DSlider(Qt::Horizontal, this);

    m_radiusNumSlider->setMinimum(0);
    m_radiusNumSlider->setMaximum(100);
    if (width < 1152) {
        m_radiusNumSlider->setFixedWidth(60);
    } else {
        m_radiusNumSlider->setFixedWidth(120);
    }
    m_radiusNumSlider->setMaximumHeight(24);

    m_radiusNumEdit = new DLineEdit(this);
    QRegExp rx("^([1-9]{1}[0-9]{0,1}|0|100){0,1}%$");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    m_radiusNumEdit->lineEdit()->setValidator(validator);
    m_radiusNumEdit->setClearButtonEnabled(false);
    m_radiusNumEdit->setFixedWidth(55);
    m_radiusNumEdit->setText(QString("%1%").arg(m_radiusNumSlider->value()));
    m_radiusNumEdit->setFont(ft);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    //layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addWidget(fillLabel);
    layout->addWidget(m_strokeBtn);
    layout->addWidget(strokeLabel);
    //layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_sepLine);
    //layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(lwLabel);
    layout->addWidget(m_sideWidthWidget);
    //layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(anchorNumLabel);
    layout->addWidget(m_anchorNumSlider);
    //layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_anchorNumEdit);
    //layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(radiusLabel);
    layout->addWidget(m_radiusNumSlider);
    //layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_radiusNumEdit);

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


    ///线宽
    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthChange, this, [ = ] () {
        emit signalPolygonalStarAttributeChanged();
    });

    ///锚点数
    connect(m_anchorNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        m_anchorNumEdit->setText(QString::number(value));
        CDrawParamSigleton::GetInstance()->setAnchorNum(value);
        emit signalPolygonalStarAttributeChanged();
    });

    connect(m_anchorNumEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        if (str.isEmpty() || str == "") {
            return ;
        }
        int value = str.trimmed().toInt();
        if (value >= 0 && value <= 2) {
            return;
        }
        m_anchorNumSlider->setValue(value);
    });

    connect(m_anchorNumEdit, &DLineEdit::editingFinished, this, [ = ]() {
        QString str = m_anchorNumEdit->text().trimmed();
        int value = str.toInt();
        int minValue = m_anchorNumSlider->minimum();
        if (value == minValue && CDrawParamSigleton::GetInstance()->getAnchorNum() != minValue) {
            m_anchorNumSlider->setValue(value);
        }
    });

    ///半径
    connect(m_radiusNumSlider, &DSlider::valueChanged, this, [ = ](int value) {
        QString str = QString("%1%").arg(value);
        if (str != m_radiusNumEdit->text().trimmed()) {
            m_radiusNumEdit->setText(str);
            if (str == "0%") {
                m_radiusNumEdit->lineEdit()->setCursorPosition(1);
            }
        }
        CDrawParamSigleton::GetInstance()->setRadiusNum(value);
        emit signalPolygonalStarAttributeChanged();
    });

    connect(m_radiusNumEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        if (str.isEmpty() || str == "") {
            m_radiusNumEdit->setText("0%");
            return ;
        }

        QString tmpStr = "";
        int value = -1;
        if (str.contains("%")) {
            tmpStr = str.split("%").first();
            value = tmpStr.trimmed().toInt();
        } else {
            value = str.toInt();
            m_radiusNumEdit->setText(str + "%");
        }

        if (value < 0 || value > 100) {
            return ;
        }

        m_radiusNumSlider->setValue(value);
    });

    connect(m_radiusNumEdit, &DLineEdit::editingFinished, this, [ = ]() {
        QString str = m_radiusNumEdit->text().trimmed();

        if (str == "%") {
            m_radiusNumEdit->setText("0%");
        }
    });
}

void PolygonalStarAttributeWidget::updatePolygonalStarWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    int anchorNum = CDrawParamSigleton::GetInstance()->getAnchorNum();

    if (anchorNum != m_anchorNumSlider->value()) {
        m_anchorNumSlider->setValue(anchorNum);
        m_anchorNumEdit->setText(QString("%1").arg(anchorNum));
    }

    int radiusNum = CDrawParamSigleton::GetInstance()->getRadiusNum();

    if (radiusNum != m_anchorNumSlider->value()) {
        m_radiusNumSlider->setValue(radiusNum);
        m_radiusNumEdit->setText(QString("%1%").arg(radiusNum));
    }
}

QPoint PolygonalStarAttributeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
