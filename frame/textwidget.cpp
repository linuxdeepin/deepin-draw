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
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

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

    QFont ft;
    ft.setPixelSize(TEXT_SIZE);

    m_textSeperatorLine = new SeperatorLine(this);

    DLabel *fontFamilyLabel = new DLabel(this);
    fontFamilyLabel->setText(tr("Font"));
    fontFamilyLabel->setFont(ft);
    m_fontComBox = new CFontComboBox(this);
    m_fontComBox->setFontFilters(DFontComboBox::AllFonts);

    m_fontComBox->setFixedSize(QSize(240, 36));
    m_fontComBox->setCurrentIndex(0);
    m_fontComBox->setEditable(true);
    m_fontComBox->lineEdit()->setReadOnly(true);
    m_fontComBox->lineEdit()->setFont(ft);
    QString strFont = m_fontComBox->currentText();
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(strFont);

    m_fontHeavy = new DComboBox(this); // 字体类型
    m_fontHeavy->setFixedSize(QSize(100, 36));
    m_fontHeavy->addItems(QStringList{tr("Normal"), tr("Bold"), tr("Thin")});

    DLabel *fontsizeLabel = new DLabel(this);
    fontsizeLabel->setText(tr("Size")); // 字号
    fontsizeLabel->setFixedSize(QSize(28, 20));
    fontsizeLabel->setFont(ft);
    m_fontSize = new DSpinBox(this);
    m_fontSize->setFixedSize(QSize(100, 36));
    m_fontSize->setRange(8, 500);
    m_fontSize->setSuffix("px");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_textSeperatorLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(fontFamilyLabel);
    layout->addWidget(m_fontComBox);
    layout->addWidget(m_fontHeavy);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(fontsizeLabel);
    layout->addWidget(m_fontSize);
    layout->addStretch();
    setLayout(layout);
}

void TextWidget::updateTheme()
{
    m_textSeperatorLine->updateTheme();
}

void TextWidget::slotFontSizeValueChanged(int size)
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextSize(size);
    emit signalTextFontSizeChanged();
    //隐藏调色板
    showColorPanel(DrawStatus::TextFill, QPoint(), false);
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
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(str);
        emit signalTextFontFamilyChanged();
    });

    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::highlighted), this, [ = ](const QString & str) {
//        qDebug() << "weight:" << m_fontComBox->font().
        m_bSelect = true;
        m_oriFamily = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family();
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(str);
        emit signalTextFontFamilyChanged();
    });

    connect(m_fontComBox, &CFontComboBox::signalhidepopup, this, [ = ]() {

        if (m_bSelect) {
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(m_oriFamily);
            emit signalTextFontFamilyChanged();
            m_bSelect = false;
        }
    });

    // 字体大小
    connect(m_fontSize, SIGNAL(valueChanged(int)), this, SLOT(slotFontSizeValueChanged(int)));
    m_fontSize->setValue(14);

    // 字体重量
    connect(m_fontHeavy, QOverload<const QString &>::of(&DComboBox::currentTextChanged), this, [ = ](const QString & str) {
        // tr("Normal"), tr("Bold"), tr("Thin")
        if (str == tr("Bold")) {
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextWeight(QFont::Bold);
        } else if (str == tr("Thin")) {
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextWeight(QFont::Thin);
        } else {
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextWeight(QFont::Normal);
        }
        emit signalTextFontSizeChanged();
        //隐藏调色板
        showColorPanel(DrawStatus::TextFill, QPoint(), false);
    });
}

void TextWidget::updateTextWidget()
{
    m_fillBtn->updateConfigColor();
    QFont font = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont();

    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSingleFontFlag()) {
        m_fontComBox->setCurrentText(font.family());
//        m_fontComBox->setCurrentFont(font);
    } else {
        m_fontComBox->setCurrentIndex(-1);
        m_fontComBox->setCurrentText("- -");
    }

    int fontSize = int(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextSize());
    if (fontSize != m_fontSize->value()) {
        m_fontSize->setValue(fontSize);
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
