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
#include "service/cmanagerattributeservice.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QLineEdit>
#include <QAbstractItemView>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 14;
TextWidget::TextWidget(DWidget *parent)
    : DWidget(parent)
    , m_oneItemIsHighlighted(false)
{
    initUI();
    initConnection();
    initDefaultSetting();
}

TextWidget::~TextWidget()
{

}

void TextWidget::initUI()
{
    m_fillBtn = new TextColorButton(this);
    m_fillBtn->setFocusPolicy(Qt::NoFocus);

    QFont ft;
    ft.setPixelSize(TEXT_SIZE);

    m_textSeperatorLine = new SeperatorLine(this);

    m_fontFamilyLabel = new DLabel(this);
    m_fontFamilyLabel->setText(tr("Font"));
    m_fontFamilyLabel->setFont(ft);
    m_fontComBox = new CFontComboBox(this);
    m_fontComBox->setFont(ft);
    m_fontComBox->setFontFilters(DFontComboBox::AllFonts);

    m_fontComBox->setFixedSize(QSize(240, 36));
    m_fontComBox->setCurrentIndex(0);
    m_fontComBox->setEditable(true);
    m_fontComBox->lineEdit()->setReadOnly(true);
    m_fontComBox->lineEdit()->setFont(ft);
    m_fontComBox->setFocusPolicy(Qt::NoFocus);

    m_fontHeavy = new DComboBox(this); // 字体类型
    m_fontHeavy->setFixedSize(QSize(130, 36));
    m_fontHeavy->setFont(ft);
    m_fontHeavy->setEditable(true);
    m_fontHeavy->lineEdit()->setReadOnly(true);
    m_fontHeavy->setFocusPolicy(Qt::NoFocus);

    m_fontsizeLabel = new DLabel(this);
    m_fontsizeLabel->setText(tr("Size")); // 字号
    m_fontsizeLabel->setFixedSize(QSize(28, 20));
    m_fontsizeLabel->setFont(ft);
    m_fontSize = new DComboBox(this);
    m_fontSize->setEditable(true);
    m_fontSize->setFixedSize(QSize(100, 36));
    m_fontSize->setFont(ft);
    m_fontSize->setProperty("preValue", 14); //默认大小
    m_fontSize->setFocusPolicy(Qt::NoFocus);
    m_fontSize->lineEdit()->setFocusPolicy(Qt::ClickFocus);
    m_fontSize->installEventFilter(this);

    QRegExp regx("[0-9]*p?x?");
    QValidator *validator = new QRegExpValidator(regx, m_fontSize);
    m_fontSize->setValidator(validator);
    addFontPointSize();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_textSeperatorLine);
    layout->addWidget(m_fontFamilyLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_fontComBox);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_fontHeavy);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_fontsizeLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_fontSize);
    layout->addStretch();
    setLayout(layout);

    //installEventFilter(this);
}

void TextWidget::updateTheme()
{
    m_textSeperatorLine->updateTheme();
}

void TextWidget::setFontSize(int size, bool emitSig)
{
//    QVariant preValue = m_fontSize->property("preValue");

//    if (preValue.isValid()) {
//        int preIntValue = preValue.toInt();
//        int curValue    = (size < 0 ? -1 : size);
//        if (preIntValue == curValue)
//            return;
//    }
//    m_fontSize->setProperty("preValue", size);

    QString findTex = QString("%1px").arg(size);

    int index = m_fontSize->findText(findTex);

    m_fontSize->blockSignals(true);
    if (index >= 0) {
        m_fontSize->setCurrentIndex(index);
    } else {
        m_fontSize->setCurrentText(findTex);
    }
    m_fontSize->blockSignals(false);

    if (emitSig)
        emit fontSizeChanged(size);
}

void TextWidget::setTextFamilyStyle(const QString &family, const QString &style, bool emitSig,
                                    bool isPreview, bool firstPreview)
{
    // ("Regular", "Black", "ExtraBold", "Bold", "DemiBold", "Medium", "Normal", "Light", "ExtraLight", "Thin")
    // 只显示：("Regular", "Black", "SemiBold", "Bold", "Medium", "Light", "ExtraLight")

    QFontDatabase base;
    QStringList listStylyName = base.styles(family.trimmed());

    QStringList showStyle;
    showStyle.append("Regular");
    showStyle.append("Black");
    showStyle.append("DemiBold");
    showStyle.append("Bold");
    showStyle.append("Medium");
    showStyle.append("Light");
    showStyle.append("ExtraLight");

    m_fontHeavy->blockSignals(true);
    m_fontHeavy->clear();
    for (QString style : showStyle) {
        if (listStylyName.contains(style)) {
            m_fontHeavy->addItem(style);
        }
    }

    // 存在特殊字体则只显示常规
    if (m_fontHeavy->count() == 0) {
        m_fontHeavy->addItem("Regular");
    } else {
        int i = m_fontHeavy->findText(style);
        if (i != -1)
            m_fontHeavy->setCurrentIndex(i);
    }

    m_fontHeavy->blockSignals(false);

    m_fontComBox->blockSignals(true);
    m_fontComBox->setCurrentIndex(m_fontComBox->findText(family));
    m_fontComBox->blockSignals(false);

    if (emitSig) {
        fontFamilyChanged(family, isPreview, firstPreview);
    }
}

void TextWidget::setVaild(bool color, bool size, bool Family, bool Style)
{
    m_fillBtn->setIsMultColorSame(color);

    if (!size) {
        m_fontSize->blockSignals(true);
        m_fontSize->lineEdit()->setText("— —");
        m_fontSize->blockSignals(false);
    }
    if (!Family) {
        m_fontComBox->blockSignals(true);
        m_fontComBox->lineEdit()->setText("— —");
        m_fontComBox->blockSignals(false);
    }

    if (!Style) {
        m_fontHeavy->blockSignals(true);
        m_fontHeavy->lineEdit()->setText("— —");
        m_fontHeavy->blockSignals(false);
    }
}

void TextWidget::setColorNull()
{
    m_fillBtn->setIsMultColorSame(false);
}

void TextWidget::setSizeNull()
{
    m_fontSize->blockSignals(true);
    m_fontSize->lineEdit()->setText("— —");
    m_fontSize->blockSignals(false);
}

void TextWidget::setFamilyNull()
{
    m_fontComBox->blockSignals(true);
    m_fontComBox->lineEdit()->setText("— —");
    m_fontComBox->blockSignals(false);
}

void TextWidget::setStyleNull()
{
    m_fontHeavy->blockSignals(true);
    m_fontHeavy->lineEdit()->setText("— —");
    m_fontHeavy->blockSignals(false);
}

bool TextWidget::eventFilter(QObject *o, QEvent *event)
{
    if (o == m_fontSize) {
        if (event->type() == QEvent::FocusIn) {
            QFocusEvent *focuEvent = static_cast<QFocusEvent *>(event);
            //qDebug() << "focuEvent->reason() = " << focuEvent->reason();
            if (focuEvent->reason() != Qt::MouseFocusReason) {
                return true;
            }
        } else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (Qt::Key_Up == keyEvent->key() || Qt::Key_Down == keyEvent->key() ||
                    Qt::Key_PageUp == keyEvent->key() || Qt::Key_PageDown == keyEvent->key()) {
                m_keyPressed = true;
            }
        } else if (event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (Qt::Key_Up == keyEvent->key() || Qt::Key_Down == keyEvent->key() ||
                    Qt::Key_PageUp == keyEvent->key() || Qt::Key_PageDown == keyEvent->key()) {
                m_keyPressed = false;
            }
        }
    }
    return DWidget::eventFilter(o, event);
}

void TextWidget::initConnection()
{
    connect(m_fillBtn, &TextColorButton::colorChanged, this, &TextWidget::colorChanged);

    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::activated), this, [ = ](const QString & str) {
        qDebug() << "do Active ====== " << str;
        m_oneItemIsHighlighted = false;
        setTextFamilyStyle(str, m_fontHeavy->currentText(), true);
    });
    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::highlighted), this, [ = ](const QString & str) {
        m_oneItemIsHighlighted = true;
        setTextFamilyStyle(str, "Regular", true, true, oneComboxFirstPopUp);
        oneComboxFirstPopUp = false;
    });
    connect(m_fontComBox, &CFontComboBox::signalhidepopup, this, [ = ]() {
        qDebug() << "do signalhidepopup ====== " << !m_oneItemIsHighlighted << "m_oriFamily = " << m_oriFamily;
        bool doChecked = !m_oneItemIsHighlighted;
        emit fontFamilyChangeFinished(doChecked);
        if (m_oneItemIsHighlighted) {
            if (m_oriFamily != "— —") {
                setTextFamilyStyle(m_oriFamily, m_fontHeavy->currentText(), true, true);
            } else {
                qDebug() << "setFamilyNull-------------";
                this->setFamilyNull();
            }
            m_oneItemIsHighlighted = false;
        }
        oneComboxFirstPopUp = false;

    }, Qt::QueuedConnection);
    connect(m_fontComBox, &CFontComboBox::signalshowpopup, this, [ = ]() {
        m_oriFamily = m_fontComBox->currentText();
        oneComboxFirstPopUp = true;
    });

    // 字体大小
    m_fontSize->setCurrentText("14px");
    connect(m_fontSize->lineEdit(), &QLineEdit::returnPressed, this, [ = ]() {
        QString str = m_fontSize->currentText();
        if (str.contains("p")) {
            str = str.replace("p", "");
        }
        if (str.contains("x")) {
            str = str.replace("x", "");
        }
        str = str.replace("px", "");

        bool flag = false;
        int size = str.toInt(&flag);
        m_fontSize->blockSignals(true);
        if (size < 8) {
            m_fontSize->setCurrentText("8px");
            addFontPointSize();
            size = 8;
        } else if (size > 500) {
            addFontPointSize();
            m_fontSize->setCurrentIndex(-1);
            m_fontSize->setCurrentText("500px");
            size = 500;
        } else {
            addFontPointSize();
            m_fontSize->setCurrentIndex(-1);
            m_fontSize->setEditText(QString::number(size) + "px");
        }
        m_fontSize->blockSignals(false);

        if (flag) {
            emit fontSizeChanged(size, true);
        } else {
            qDebug() << "set error font size with str: " << str;
        }
    });
    connect(m_fontSize, QOverload<const QString &>::of(&DComboBox::currentIndexChanged), this, [ = ](QString str) {
        if (!str.contains("px") && !m_fontSize->findText(str)) {
            m_fontSize->setCurrentIndex(-1);
            return ;
        }

        str = str.replace("px", "");
        bool flag = false;
        int size = str.toInt(&flag);
        if (flag) {
            emit fontSizeChanged(size, !m_keyPressed);
        } else {
            qDebug() << "set error font size with str: " << str;
        }
    });

    // 字体重量
    connect(m_fontHeavy, &DComboBox::currentTextChanged, this, [ = ](const QString & str) {
        // ("Black", "ExtraBold", "Bold", "DemiBold", "Medium", "Normal", "Light", "ExtraLight", "Thin")
        QString style = "Regular";
        if (str == "Black") {
            style = "Black";
        } else if (str == "ExtraBold") {
            style = "ExtraBold";
        } else if (str == "Bold") {
            style = "Bold";
        } else if (str == "DemiBold") {
            style = "DemiBold";
        } else if (str == "Medium") {
            style = "Medium";
        } else if (str == "Normal") {
            style = "Normal";
        } else if (str == "Light") {
            style = "Light";
        } else if (str == "ExtraLight") {
            style = "ExtraLight";
        } else if (str == "Thin") {
            style = "Thin";
        }
        emit fontStyleChanged(style);
    });
}

void TextWidget::initDefaultSetting()
{
//    setTextFamilyStyle(m_fontComBox->currentText());
}

void TextWidget::addFontPointSize()
{
    m_fontSize->clear();
    m_fontSize->addItem("8px");
    m_fontSize->addItem("10px");
    m_fontSize->addItem("12px");
    m_fontSize->addItem("14px");
    m_fontSize->addItem("16px");
    m_fontSize->addItem("18px");
    m_fontSize->addItem("24px");
    m_fontSize->addItem("36px");
    m_fontSize->addItem("48px");
    m_fontSize->addItem("60px");
    m_fontSize->addItem("72px");
    m_fontSize->addItem("100px");
}

void TextWidget::setTextColor(const QColor &color, bool emitSig)
{
    m_fillBtn->setColor(color, emitSig ? EChanged : EChangedUpdate, emitSig);
}
