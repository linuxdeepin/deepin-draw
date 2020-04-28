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
#include "commonshapewidget.h"
#include "drawshape/cdrawparamsigleton.h"
#include "widgets/toolbutton.h"
#include "widgets/bigcolorbutton.h"
#include "widgets/bordercolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/csidewidthwidget.h"
#include "widgets/cspinbox.h"
#include "service/cmanagerattributeservice.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QDebug>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 14;

CommonshapeWidget::CommonshapeWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();
}

CommonshapeWidget::~CommonshapeWidget()
{
}

void CommonshapeWidget::changeButtonTheme()
{
    m_sideWidthWidget->changeButtonTheme();
    m_sepLine->updateTheme();
}

void CommonshapeWidget::setRectXRedius(int redius)
{
    m_rediusSpinbox->setValue(redius);
}

void CommonshapeWidget::setRectXRediusSpinboxVisible(bool visible)
{
    m_rediusLable->setVisible(visible);
    m_rediusSpinbox->setVisible(visible);
    m_sepLine->setVisible(visible);
}

void CommonshapeWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_fillBtn->setVisible(false);
    m_strokeBtn->setVisible(false);
    m_sepLine->setVisible(false);
    m_sideWidthWidget->setVisible(false);
    m_rediusLable->setVisible(false);
    m_rediusSpinbox->setVisible(false);

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
        case LineWidth:
//            m_lwLabel->setVisible(true);
            m_sideWidthWidget->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_sideWidthWidget->setMenuNoSelected(true);
            } else {
                m_sideWidthWidget->setSideWidth(propertys[property].toInt());
            }
            m_sideWidthWidget->update();
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
        case RectRadius:
            m_rediusLable->setVisible(true);
            m_rediusSpinbox->setVisible(true);
            m_sepLine->setVisible(true);
            m_rediusSpinbox->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_rediusSpinbox->setValue(-1);
            } else {
                m_rediusSpinbox->setValue(propertys[property].toInt());
            }
            m_rediusSpinbox->blockSignals(false);
            m_rediusSpinbox->update();
            break;
        default:
            break;
        }
    }
}

void CommonshapeWidget::initUI()
{
    setAttribute(Qt::WA_NoMousePropagation, true);
//    DLabel *fillLabel = new DLabel(this);
//    fillLabel->setText(tr("填充"));
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
//    fillLabel->setFont(ft);

    m_fillBtn = new BigColorButton(this);
    m_strokeBtn = new BorderColorButton(this);


//    DLabel *strokeLabel = new DLabel(this);
//    strokeLabel->setObjectName("StrokeLabel");
//    strokeLabel->setText(tr("描边"));
//    strokeLabel->setFont(ft);
    m_sepLine = new SeperatorLine(this);
//    m_lwLabel = new DLabel(this);
//    m_lwLabel->setObjectName("BorderLabel");
    //lwLabel->setText(tr("描边粗细"));
//    m_lwLabel->setText(tr("Width"));
    QFont ft1;
    ft1.setPixelSize(TEXT_SIZE - 1);
//    m_lwLabel->setFont(ft1);

    m_sideWidthWidget = new CSideWidthWidget(this);
    m_sideWidthWidget->setFixedWidth(100);
    m_sideWidthWidget->setFont(ft);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
//    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    //layout->addWidget(fillLabel);
    layout->addWidget(m_strokeBtn);
    //layout->addWidget(strokeLabel);
//    layout->addSpacing(SEPARATE_SPACING);
//    layout->addSpacing(SEPARATE_SPACING);
//    layout->addWidget(m_lwLabel);
    layout->addWidget(m_sideWidthWidget);
    layout->addWidget(m_sepLine);

    m_rediusLable = new DLabel(this);
    m_rediusLable->setText(tr("Radius"));
    m_rediusLable->setFont(ft1);
    layout->addWidget(m_rediusLable);

    m_rediusSpinbox = new CSpinBox(this);
    m_rediusSpinbox->setKeyboardTracking(false);
    m_rediusSpinbox->setRange(-1, 1000);
    m_rediusSpinbox->setFixedSize(QSize(125, 36));
    m_rediusSpinbox->setFont(ft);
    m_rediusSpinbox->setSpecialValueText("— —");
    m_rediusSpinbox->setEnabledEmbedStyle(true);

    layout->addWidget(m_rediusSpinbox);
    layout->addStretch();

    setLayout(layout);
}

void CommonshapeWidget::initConnection()
{
    connect(m_fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_strokeBtn->resetChecked();
        mapToGlobal(QPoint(1, 1));
//        emit showColorPanel(DrawStatus::Fill, cursor().pos(), show);
        emit showColorPanel(DrawStatus::Fill, getBtnPosition(m_fillBtn), show);


    });
    connect(m_strokeBtn, &BorderColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        m_fillBtn->resetChecked();
        emit showColorPanel(DrawStatus::Stroke, getBtnPosition(m_strokeBtn), show);
    });

    connect(this, &CommonshapeWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
        m_strokeBtn->resetChecked();
    });

    //描边粗细
    connect(m_sideWidthWidget, SIGNAL(signalSideWidthChoosed(int)), this, SLOT(slotSideWidthChoosed(int)));

    connect(m_sideWidthWidget, &CSideWidthWidget::signalSideWidthMenuShow, this, [ = ] () {
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
    });


    ///圆角半径
    connect(m_rediusSpinbox, SIGNAL(valueChanged(int)), this, SLOT(slotRectRediusChanged(int)));
    connect(m_rediusSpinbox, &CSpinBox::focusChanged, this, [ = ] (bool isFocus) {
        emit signalRectRediusIsfocus(isFocus);
    });
    connect(m_rediusSpinbox, &DSpinBox::editingFinished, this, [ = ] () {
        //等于0时是特殊字符，不做处理
        qDebug() << "m_rediusSpinbox->value() = " << m_rediusSpinbox->value();
        if ( m_rediusSpinbox->value() < 0) {
            return ;
        }
        m_rediusSpinbox->blockSignals(true);
        if (m_rediusSpinbox->value() < 0) {
            m_rediusSpinbox->setValue(0);
        } else if (m_rediusSpinbox->value() > 1000) {
            m_rediusSpinbox->setValue(1000);
        }
        m_rediusSpinbox->blockSignals(false);
        //隐藏调色板
        showColorPanel(DrawStatus::Stroke, QPoint(), false);
        emit signalRectRediusChanged(m_rediusSpinbox->value());
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(RectRadius, m_rediusSpinbox->value());
    });
}

void CommonshapeWidget::updateCommonShapWidget()
{
    m_fillBtn->updateConfigColor();
    m_strokeBtn->updateConfigColor();
    m_sideWidthWidget->updateSideWidth();

    m_fillBtn->setVisible(true);
    m_strokeBtn->setVisible(true);
    m_sideWidthWidget->setVisible(true);
    m_sepLine->setVisible(false);
    m_rediusLable->setVisible(false);
    m_rediusSpinbox->setVisible(false);
    //CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void CommonshapeWidget::slotRectRediusChanged(int redius)
{
    m_rediusSpinbox->blockSignals(true);
    if (m_rediusSpinbox->value() <= 0) {
        m_rediusSpinbox->setValue(0);
    } else if (m_rediusSpinbox->value() > 1000) {
        m_rediusSpinbox->setValue(1000);
    }
    redius = m_rediusSpinbox->value();
    m_rediusSpinbox->blockSignals(false);
    //隐藏调色板
    showColorPanel(DrawStatus::Stroke, QPoint(), false);
    emit signalRectRediusChanged(redius);
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(RectRadius, redius);
}

void CommonshapeWidget::slotSideWidthChoosed(int width)
{
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(LineWidth, width);
    this->setFocus();
}

QPoint CommonshapeWidget::getBtnPosition(const DPushButton *btn)
{
    QPoint btnPos = mapToGlobal(btn->pos());
    QPoint pos(btnPos.x() + btn->width() / 2,
               btnPos.y() + btn->height());

    return pos;
}
