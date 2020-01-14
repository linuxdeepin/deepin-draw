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
#include "textwidget.h"

#include <DLabel>
#include "widgets/textcolorbutton.h"
#include "widgets/seperatorline.h"
#include "utils/cvalidator.h"
#include "drawshape/cdrawparamsigleton.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 12;
TextWidget::TextWidget(DWidget *parent)
    : DWidget(parent)
    , m_bSelect(false)
{
    initUI();
    initConnection();
}

TextWidget::~TextWidget()
{

}

void TextWidget::initUI()
{
    m_fillBtn = new TextColorButton( this);
    m_fillBtn->setFocusPolicy(Qt::NoFocus);

//    DLabel *colBtnLabel = new DLabel(this);
//    colBtnLabel->setText(tr("填充"));
    QFont ft;
    ft.setPixelSize(TEXT_SIZE);
//    colBtnLabel->setFont(ft);

    m_textSeperatorLine = new SeperatorLine(this);

    DLabel *fontFamilyLabel = new DLabel(this);
    fontFamilyLabel->setText(tr("Font"));
    fontFamilyLabel->setFont(ft);
    m_fontComBox = new CFontComboBox(this);
    m_fontComBox->setFontFilters(DFontComboBox::AllFonts);
    //m_fontComBox->setMinimumWidth(100);

    m_fontComBox->setFixedWidth(200);
    m_fontComBox->setCurrentIndex(0);
    m_fontComBox->setEditable(true);
    m_fontComBox->lineEdit()->setReadOnly(true);
    m_fontComBox->lineEdit()->setFont(ft);
    QString strFont = m_fontComBox->currentText();
    CDrawParamSigleton::GetInstance()->setTextFont(strFont);


    DLabel *fontsizeLabel = new DLabel(this);
    //fontsizeLabel->setText(tr("字号"));
    fontsizeLabel->setText(tr("Size"));
    fontsizeLabel->setFont(ft);

    m_fontSizeSlider = new DSlider(Qt::Horizontal, this);


    m_fontSizeSlider->setMinimum(8);
    m_fontSizeSlider->setMaximum(500);
    //m_fontSizeSlider->setMinimumWidth(200);
    m_fontSizeSlider->setFixedWidth(120);
    m_fontSizeSlider->setMaximumHeight(24);
    m_fontSizeSlider->setValue(int(CDrawParamSigleton::GetInstance()->getTextSize()));
    m_fontSizeSlider->slider()->setFocusPolicy(Qt::NoFocus);

    m_fontSizeEdit = new DLineEdit(this);
    m_fontSizeEdit->lineEdit()->setValidator(new CIntValidator(8, 999, this));
    m_fontSizeEdit->setClearButtonEnabled(false);
    m_fontSizeEdit->setFixedWidth(55);
    m_fontSizeEdit->setText(QString::number(m_fontSizeSlider->value()));
    m_fontSizeEdit->setFont(ft);


    m_fontSizeAddAction = new QAction(this);
    m_fontSizeAddAction->setShortcut(QKeySequence(Qt::Key_Up));
    this->addAction(m_fontSizeAddAction);
    m_fontSizeReduceAction = new QAction(this);
    m_fontSizeReduceAction->setShortcut(QKeySequence(Qt::Key_Down));
    this->addAction(m_fontSizeReduceAction);



    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
//    layout->addWidget(colBtnLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_textSeperatorLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(fontFamilyLabel);
    layout->addWidget(m_fontComBox);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(fontsizeLabel);
    layout->addWidget(m_fontSizeSlider);
    layout->addWidget(m_fontSizeEdit);
    layout->addStretch();
    setLayout(layout);
}

void TextWidget::updateTheme()
{
    m_textSeperatorLine->updateTheme();
}

void TextWidget::initConnection()
{
    connect(m_fillBtn, &TextColorButton::btnCheckStateChanged, this, [ = ](bool show) {

        QPoint btnPos = mapToGlobal(m_fillBtn->pos());
        QPoint pos(btnPos.x() + m_fillBtn->width() / 2,
                   btnPos.y() + m_fillBtn->height());

        showColorPanel(DrawStatus::TextFill, pos, show);
    });

    connect(this, &TextWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
    });

    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::activated), this, [ = ](const QString & str) {
        m_bSelect = false;
        CDrawParamSigleton::GetInstance()->setTextFont(str);
        emit signalTextFontFamilyChanged();
    });

    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::highlighted), this, [ = ](const QString & str) {

        m_bSelect = true;
        m_oriFamily = CDrawParamSigleton::GetInstance()->getTextFont().family();
        CDrawParamSigleton::GetInstance()->setTextFont(str);
        emit signalTextFontFamilyChanged();
    });

    connect(m_fontComBox, &CFontComboBox::signalhidepopup, this, [ = ]() {

        if (m_bSelect) {
            CDrawParamSigleton::GetInstance()->setTextFont(m_oriFamily);
            emit signalTextFontFamilyChanged();
            m_bSelect = false;
        }
    });

    ///字体大小
    connect(m_fontSizeSlider, &DSlider::valueChanged, this, [ = ](int value) {
        m_fontSizeEdit->setText(QString::number(value));
        CDrawParamSigleton::GetInstance()->setTextSize(value);
        emit signalTextFontSizeChanged();
    });

    connect(m_fontSizeEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        if (str.isEmpty() || str == "") {
            return ;
        }
        int value = str.trimmed().toInt();
        if (value >= 0 && value < m_fontSizeSlider->slider()->minimum()) {
            return ;
        }
        if (value > 500) {
            value = 500;
        }
        m_fontSizeEdit->setText(QString::number(value));

        m_fontSizeSlider->setValue(value);
    });

    connect(m_fontSizeEdit, &DLineEdit::editingFinished, this, [ = ]() {
        QString str = m_fontSizeEdit->text().trimmed();
        int value = str.toInt();
        int minValue = m_fontSizeSlider->minimum();

        int defaultFontSize = int(CDrawParamSigleton::GetInstance()->getTextSize());

        if (value == minValue && defaultFontSize != minValue) {
            m_fontSizeSlider->setValue(value);
        }
    });

    connect(m_fontSizeAddAction, &QAction::triggered, this, [ = ](bool) {
        if (m_fontSizeEdit->lineEdit()->hasFocus()) {
            int sideNum = m_fontSizeEdit->lineEdit()->text().trimmed().toInt();
            sideNum++;
            if (sideNum > 500) {
                return ;
            }
            QString text = QString::number(sideNum);
            m_fontSizeEdit->setText(text);
            emit m_fontSizeEdit->lineEdit()->textEdited(text);
        }
    });

    connect(m_fontSizeReduceAction, &QAction::triggered, this, [ = ](bool) {
        if (m_fontSizeEdit->lineEdit()->hasFocus()) {
            int sideNum = m_fontSizeEdit->lineEdit()->text().trimmed().toInt();
            sideNum --;
            if (sideNum < 8) {
                return ;
            }
            QString text = QString::number(sideNum);
            m_fontSizeEdit->setText(text);
            emit m_fontSizeEdit->lineEdit()->textEdited(text);
        }
    });
}

void TextWidget::updateTextWidget()
{
    m_fillBtn->updateConfigColor();
    QFont font = CDrawParamSigleton::GetInstance()->getTextFont();

    if (CDrawParamSigleton::GetInstance()->getSingleFontFlag()) {
        m_fontComBox->setCurrentText(font.family());
//        m_fontComBox->setCurrentFont(font);
    } else {
        m_fontComBox->setCurrentIndex(-1);
        m_fontComBox->setCurrentText("- -");
    }

//    if (m_fontComBox->currentText() != font.family()) {
//        //m_fontComBox->setFont(font);

//    }

    int fontSize = int(CDrawParamSigleton::GetInstance()->getTextSize());

    if (fontSize != m_fontSizeSlider->value()) {
        m_fontSizeSlider->blockSignals(true);
        m_fontSizeSlider->setValue(fontSize);
        m_fontSizeSlider->blockSignals(false);

        m_fontSizeEdit->blockSignals(true);
        m_fontSizeEdit->setText(QString("%1").arg(fontSize));
        m_fontSizeEdit->blockSignals(false);
    }
}

void TextWidget::updateTextColor()
{
    m_fillBtn->updateConfigColor();
//    m_fillBtn->clearFocus();
//    if (qApp->focusWidget() != nullptr) {
//        qApp->focusWidget()->hide();
//    }
}
