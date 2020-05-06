/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
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
#include "colorpanel.h"

#include <DGuiApplicationHelper>

#include <QPainter>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QDebug>
#include <QRegExpValidator>

#include "utils/global.h"
#include "utils/baseutils.h"
#include "drawshape/cdrawparamsigleton.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "service/cmanagerattributeservice.h"

#include "ciconbutton.h"
#include "colorlabel.h"
#include "colorslider.h"
#include "pickcolorwidget.h"
#include "calphacontrolwidget.h"


DGUI_USE_NAMESPACE

const int ORGIN_WIDTH = 314;
const int PANEL_WIDTH = 294;
const int ORIGIN_HEIGHT = 250;
const int EXPAND_HEIGHT = 475;
const int RADIUS = 8;
const QSize COLOR_BORDER_SIZE = QSize(34, 34);


ColorButton::ColorButton(const QColor &color, DWidget *parent)
    : DPushButton(parent)
    , m_disable(false)
{
    m_color = color;
    setFixedSize(COLOR_BORDER_SIZE);
    setCheckable(true);

    connect(this, &ColorButton::clicked, this, [ = ] {
        setChecked(true);
        if (m_disable)
        {
            emit colorButtonClicked(Qt::transparent);
        } else
        {
            emit colorButtonClicked(m_color);
        }
    });
}

void ColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setBrush(QBrush(m_color));

    QPen pen;
    pen.setWidth(1);
    if (m_color == QColor(Qt::transparent)) {
        pen.setColor(QColor("#cccccc"));
    } else {
        pen.setColor(QColor(0, 0, 0, 55));
    }
    painter.setPen(pen);
    painter.drawRoundedRect(QRect(3, 3, this->width() - 6,
                                  this->height() - 6), RADIUS, RADIUS);
    if (m_color == QColor(Qt::transparent)) {
        pen.setColor(QColor("#ff804d"));
        painter.setPen(pen);
        painter.drawLine(6, this->height() - 6, this->width() - 6, 6);
    }

    if (isChecked()) {
        painter.setBrush(QBrush());
        QPen borderPen;
        borderPen.setWidth(2);
        borderPen.setColor("#008eff");
        painter.setPen(borderPen);
        painter.drawRoundedRect(QRect(1, 1, this->width() - 2,
                                      this->height() - 2), RADIUS, RADIUS);
    }
}

void ColorButton::setDisableColor(bool disable)
{
    m_disable = disable;
}

ColorButton::~ColorButton()
{
}

ColorPanel::ColorPanel(DWidget *parent)
    : DWidget(parent)
    , m_drawstatus(DrawStatus::Fill)
    , m_expand(false)
{
    initUI();
    initConnection();
}

ColorPanel::~ColorPanel()
{

}

////颜色按钮点击处理
void ColorPanel::setConfigColor(QColor color)
{
    /// 颜色名字
    QString colorName = "";
    if (color.name().contains("#")) {
        colorName = color.name().split("#").last();
    }
    m_colLineEdit->blockSignals(true);
    m_colLineEdit->setText(colorName);
    m_colLineEdit->blockSignals(false);
    /// 颜色Alpha值
    m_alphaControlWidget->updateAlphaControlWidget(color.alpha());
    ///更新RBG值
    m_pickColWidget->setRgbValue(color);

    emit signalColorChanged();
    //写入参数
    if (m_drawstatus == Fill) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setFillColor(color);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::FillColor, color);
    } else if (m_drawstatus == Stroke) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineColor(color);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineColor, color);
    } else if (m_drawstatus == TextFill) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextColor(color);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextColor, color);
    }
}

////颜色按钮点击处理
void ColorPanel::setConfigColorByColorName(QColor color)
{
    /// 颜色Alpha值
    m_alphaControlWidget->updateAlphaControlWidget(color.alpha());

    ///更新RBG值
    m_pickColWidget->setRgbValue(color);

    emit signalColorChanged();
    ///写入参数
    if (m_drawstatus == Fill) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setFillColor(color);
    } else if (m_drawstatus == Stroke) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setLineColor(color);
    } else if (m_drawstatus == TextFill) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextColor(color);
    }

    resetColorBtn();
    update();
}

void ColorPanel::changeButtonTheme()
{
    int themeType = CManageViewSigleton::GetInstance()->getThemeType();
    m_colorfulBtn->setCurrentTheme(themeType);
    m_pickColWidget->updateButtonTheme(themeType);
}

void ColorPanel::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void ColorPanel::slotPickedColorChanged(QColor newColor)
{
    resetColorBtn();
    setConfigColor(newColor);
}

void ColorPanel::initUI()
{
    DWidget *colorBtnWidget = new DWidget(this);
    colorBtnWidget->setFixedSize(ORGIN_WIDTH, ORIGIN_HEIGHT);

    m_colList = specifiedColorList();

    m_colorsButtonGroup = new QButtonGroup(this);
    m_colorsButtonGroup->setExclusive(true);

    QGridLayout *gLayout = new QGridLayout;
    gLayout->setVerticalSpacing(4);
    gLayout->setHorizontalSpacing(4);

    for (int i = 0; i < m_colList.length(); i++) {
        ColorButton *cb = new ColorButton(m_colList[i], this);
        cb->setFocusPolicy(Qt::NoFocus);
        if (i == 0)
            cb->setDisableColor(true);
        m_cButtonList.append(cb);
        gLayout->addWidget(cb, i / 8, i % 8);
        m_colorsButtonGroup->addButton(cb, i);
    }

    m_alphaControlWidget = new CAlphaControlWidget(this);
    m_alphaControlWidget->setFocusPolicy(Qt::NoFocus);
//    m_alphaControlWidget->setFixedHeight(24);
//    m_alphaControlWidget->setFixedWidth(236);

    DWidget *colorValueWidget = new DWidget;
    colorValueWidget->setFocusPolicy(Qt::NoFocus);
    colorValueWidget->setFixedWidth(PANEL_WIDTH);
    DLabel *colLabel = new DLabel(colorValueWidget);
    QFont colLabelFont = colLabel->font();
    colLabelFont.setPixelSize(13);
    colLabel->setFixedWidth(35);
    colLabel->setText(tr("Color"));
    colLabel->setFont(colLabelFont);

    m_colLineEdit = new DLineEdit(colorValueWidget);
    m_colLineEdit->setObjectName("Color Line Edit");
    m_colLineEdit->setFixedSize(180, 36);
    m_colLineEdit->setClearButtonEnabled(false);
    m_colLineEdit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[0-9A-Fa-f]{6,8}"), this));
    //m_colLineEdit->lineEdit()->setEnabled(false);

    QMap<int, QMap<CIconButton::EIconButtonSattus, QString> > pictureMap;
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Normal] = QString(":/theme/light/images/draw/palette_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Hover] = QString(":/theme/light/images/draw/palette_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Press] = QString(":/theme/light/images/draw/palette_normal.svg");
    pictureMap[DGuiApplicationHelper::LightType][CIconButton::Active] = QString(":/theme/light/images/draw/palette_normal.svg");

    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Normal] = QString(":/theme/dark/images/draw/palette_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Hover] = QString(":/theme/dark/images/draw/palette_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Press] = QString(":/theme/dark/images/draw/palette_normal.svg");
    pictureMap[DGuiApplicationHelper::DarkType][CIconButton::Active] = QString(":/theme/dark/images/draw/palette_normal.svg");

    m_colorfulBtn = new CIconButton(pictureMap, QSize(55, 36), colorValueWidget, false);
    m_colorfulBtn->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *colorLayout = new QHBoxLayout(colorValueWidget);
//    colorValueWidget->setStyleSheet("background-color: rgb(0, 255, 0);");
    colorLayout->setMargin(0);
    colorLayout->setSpacing(0);
    colorLayout->addWidget(colLabel);
    colorLayout->addSpacing(14);
    colorLayout->addWidget(m_colLineEdit);
    colorLayout->addSpacing(10);
    colorLayout->addWidget(m_colorfulBtn);

    m_pickColWidget = new PickColorWidget(this);
    m_pickColWidget->setFocusPolicy(Qt::NoFocus);


    QVBoxLayout *vLayout = new QVBoxLayout(colorBtnWidget);
//    vLayout->setContentsMargins(10, 0, 10, 0);
    vLayout->setSpacing(0);
    vLayout->addSpacing(10);
    vLayout->addLayout(gLayout);
    vLayout->addSpacing(10);
    vLayout->addWidget(m_alphaControlWidget, 0, Qt::AlignCenter);
    vLayout->addSpacing(10);
    vLayout->addWidget(colorValueWidget, 0, Qt::AlignCenter);
//    vLayout->addSpacing(10);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(colorBtnWidget);
    layout->addWidget(m_pickColWidget, 0, Qt::AlignCenter);

    if (!m_expand)
        m_pickColWidget->hide();
}

void ColorPanel::initConnection()
{
    ///颜色按钮
    foreach (ColorButton *cb, m_cButtonList) {
        connect(cb, &ColorButton::colorButtonClicked, this, &ColorPanel::setConfigColor);
    }

    ///调试板改变
    connect(m_pickColWidget, &PickColorWidget::pickedColor, this, &ColorPanel::slotPickedColorChanged);

    ///调色板颜色预览
    connect(m_pickColWidget, &PickColorWidget::signalPreSetColorName, [ = ](QColor color) {
        QString colorName = "";
        if (color.name().contains("#")) {
            colorName = color.name().split("#").last();
        }
        m_colLineEdit->blockSignals(true);
        m_colLineEdit->setText(colorName);
        m_colLineEdit->blockSignals(false);
    });

    connect(m_colLineEdit, &DLineEdit::textChanged,  this, [ = ](QString text) {
        if (text.length() < 6) {
            return ;
        }

        QString colorName = text.insert(0, "#");
        QColor newColor(colorName);

        if (newColor.isValid()) {
            setConfigColorByColorName(newColor);

        }
        ////更新颜色按钮
        if (m_colList.contains(newColor)) {
            m_cButtonList[m_colList.indexOf(newColor)]->setChecked(true);
        } else {
            resetColorBtn();
        }
    });


    // Alpha 值实时改变处理
    connect(m_alphaControlWidget, &CAlphaControlWidget::signalAlphaChanged, this, [ = ] (int alphaValue) {
        if (m_drawstatus == Fill) {
            if (m_colorsButtonGroup->button(0)->isChecked()) {
                return;
            }
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::FillColorAlpha, alphaValue, false);
        } else if (m_drawstatus == Stroke) {
            if (m_colorsButtonGroup->button(0)->isChecked()) {
                return;
            }
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineColorAlpha, alphaValue, false);
        } else if (m_drawstatus == TextFill) {
            if (m_colorsButtonGroup->button(0)->isChecked()) {
                return;
            }
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextColorAlpha, alphaValue, false);
        }
        resetColorBtn();
        update();
    });
    // Alpha 值确定改变写入缓存处理
    connect(m_alphaControlWidget, &CAlphaControlWidget::signalFinishChanged, this, [ = ] () {
        int alpha = m_alphaControlWidget->getCurrentAlphaValue();
        if (m_drawstatus == Fill) {
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::LineColorAlpha, alpha, true);
            QColor color = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getFillColor();
            color.setAlpha(alpha);
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setFillColor(color);
        } else if (m_drawstatus == Stroke) {
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::FillColorAlpha, alpha, true);
            QPen pen = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getPen();
            QColor color = pen.color();
            color.setAlpha(alpha);
            pen.setColor(color);
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setPen(pen);
        } else if (m_drawstatus == TextFill) {
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextColorAlpha, alpha, true);
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextColorAlpha(alpha);
        }
        resetColorBtn();
        update();
    });

    ///展开按钮
    connect(m_colorfulBtn, &CIconButton::buttonClick, this, [ = ] {
        if (m_expand)
        {
            m_pickColWidget->hide();
            m_pickColWidget->setPickedColor(false);
            setFixedHeight(ORIGIN_HEIGHT);
            updateGeometry();
        } else
        {
            m_pickColWidget->show();
            m_pickColWidget->setPickedColor(true);
            setFixedHeight(EXPAND_HEIGHT);
            updateGeometry();
        }
        emit updateHeight();

        m_expand = !m_expand;
    });
}

void ColorPanel::resetColorBtn()
{
    m_colorsButtonGroup->setExclusive(false);
    foreach (ColorButton *cb, m_cButtonList) {
        cb->setChecked(false);
    }
    m_colorsButtonGroup->setExclusive(true);
}


void ColorPanel::updateColorPanel(DrawStatus status, QColor color, int alpha)
{
    m_drawstatus = status;

    QColor configColor = color;
    ////更新颜色按钮
    if (m_colList.contains(configColor)) {
        m_cButtonList[m_colList.indexOf(configColor)]->setChecked(true);
    } else {
        resetColorBtn();
    }

    ///更新颜色名字
    QString colorName = "";
    if (configColor.name().contains("#")) {
        colorName = configColor.name().split("#").last();
    }
    m_colLineEdit->blockSignals(true);
    m_colLineEdit->setText(colorName);
    m_colLineEdit->blockSignals(false);
    /// 颜色Alpha值
    m_alphaControlWidget->updateAlphaControlWidget(alpha);

    ///更新RBG值
    m_pickColWidget->setRgbValue(configColor);
}
