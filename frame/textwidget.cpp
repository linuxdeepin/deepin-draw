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
    m_fillBtn = new TextColorButton( this);
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
    QString strFont = m_fontComBox->currentText();
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(strFont);

    m_fontHeavy = new DComboBox(this); // 字体类型
    m_fontHeavy->setFixedSize(QSize(100, 36));
    m_fontHeavy->setFont(ft);

    m_fontsizeLabel = new DLabel(this);
    m_fontsizeLabel->setText(tr("Size")); // 字号
    m_fontsizeLabel->setFixedSize(QSize(28, 20));
    m_fontsizeLabel->setFont(ft);
    m_fontSize = new DComboBox(this);
    m_fontSize->setEditable(true);
    m_fontSize->setFixedSize(QSize(100, 36));
    m_fontSize->setFont(ft);

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

    installEventFilter(this);
}

void TextWidget::updateTheme()
{
    m_textSeperatorLine->updateTheme();
}

void TextWidget::slotFontSizeValueChanged(int size)
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextSize(size);

    //隐藏调色板
    showColorPanel(DrawStatus::TextFill, QPoint(), false);
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextSize, size);
}

void TextWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_fillBtn->setVisible(false);
    m_textSeperatorLine->setVisible(false);
    m_fontFamilyLabel->setVisible(false);
    //m_fontComBox->setVisible(false);
    m_fontsizeLabel->setVisible(false);
    m_fontSize->setVisible(false);
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
        case TextColor:
            m_fillBtn->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fillBtn->setIsMultColorSame(false);
            } else {
                m_fillBtn->setColor(propertys[property].value<QColor>());
            }
            m_fillBtn->update();
            break;
        case TextFont:
            m_fontFamilyLabel->setVisible(true);
            m_fontComBox->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fontComBox->setCurrentText("—— ——");
            } else {
                m_fontComBox->setCurrentText(propertys[property].value<QFont>().family());
            }
            m_fontComBox->update();
            break;
        case TextSize:
            m_fontsizeLabel->setVisible(true);
            m_fontSize->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fontSize->blockSignals(true);
                m_fontSize->setCurrentText("—— ——");
                m_fontSize->blockSignals(false);
            } else {
                int size = propertys[property].toInt();
                if (size) {
                    m_fontSize->blockSignals(true);
                    m_fontSize->setCurrentText(QString::number(size) + "px");
                    m_fontSize->blockSignals(false);
                } else {
                    m_fontSize->blockSignals(true);
                    m_fontSize->setCurrentIndex(-1);
                    m_fontSize->setCurrentText("—— ——");
                    m_fontSize->blockSignals(false);
                }
            }
            break;
        default:
            break;
        }
    }
}

void TextWidget::slotTextItemPropertyUpdate(QMap<EDrawProperty, QVariant> propertys)
{
    QMap<EDrawProperty, QVariant>::iterator itr = propertys.begin();
    for (; itr != propertys.end(); itr++) {
        switch (itr.key()) {
        case TextColor: {
            QColor color = itr.value().value<QColor>();
            bool colorIsValid = color.isValid();
            m_fillBtn->setIsMultColorSame(colorIsValid);
            break;
        }
        case TextSize: {
            int size = itr.value().toInt();
            if (size) {
                m_fontSize->blockSignals(true);
                m_fontSize->setCurrentText(QString::number(size) + "px");
                m_fontSize->blockSignals(false);
            } else {
                m_fontSize->blockSignals(true);
                m_fontSize->setCurrentIndex(-1);
                m_fontSize->setCurrentText("—— ——");
                m_fontSize->blockSignals(false);
            }
            break;
        }
        case TextFont: {
            QString family = itr.value().toString();
            m_fontComBox->blockSignals(true);
            if (family.isEmpty()) {
                m_fontComBox->setCurrentIndex(-1);
                m_fontComBox->setCurrentText("—— ——");
            } else {
                m_fontComBox->setCurrentText(family);
            }
            m_fontComBox->blockSignals(false);
            m_fillBtn->setVisible(true);
            m_textSeperatorLine->setVisible(true);
            m_fontFamilyLabel->setVisible(true);
            m_fontComBox->setVisible(true);
            m_fontsizeLabel->setVisible(true);
            m_fontSize->setVisible(true);
            break;
        }
        case TextHeavy: {
            QString familyStyle = itr.value().toString();
            m_fontHeavy->blockSignals(true);
            if (familyStyle.isEmpty()) {
                qDebug() << "Content: " << "this error";
                m_fontHeavy->setCurrentIndex(-1);
                m_fontHeavy->setCurrentText("123");
            } else {
                qDebug() << "Content: " << familyStyle;
                m_fontHeavy->setCurrentText(familyStyle);
            }
            m_fontHeavy->blockSignals(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void TextWidget::slotUpdateTextFamilyStyle(QString style)
{
    QFontDatabase base; //("Medium", "Bold", "ExtraLight", "Regular", "Heavy", "Light", "SemiBold")
    QStringList listStylyName = base.styles(style);
    listStylyName.removeOne("Regular");
    m_fontHeavy->blockSignals(true);
    m_fontHeavy->clear();
    m_fontHeavy->addItem(tr("Regular"));
    for (QString style : listStylyName) {
        m_fontHeavy->addItem(style);
    }
    m_fontHeavy->setCurrentIndex(0);
    m_fontHeavy->blockSignals(false);
}

bool TextWidget::eventFilter(QObject *, QEvent *event)
{
    event->accept();
    return true;
}

void TextWidget::initConnection()
{
    connect(m_fillBtn, &TextColorButton::btnCheckStateChanged, this, [ = ](bool show) {

        QPoint btnPos = mapToGlobal(m_fillBtn->pos());
        QPoint pos(btnPos.x() + m_fillBtn->width() / 2,
                   btnPos.y() + m_fillBtn->height());

        showColorPanel(DrawStatus::TextFill, pos, show);
    });
    connect(CManagerAttributeService::getInstance(), SIGNAL(signalTextItemPropertyUpdate(QMap<EDrawProperty, QVariant>)),
            this, SLOT(slotTextItemPropertyUpdate(QMap<EDrawProperty, QVariant>)));

    connect(this, &TextWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
    });

    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::activated), this, [ = ](const QString & str) {
        m_oneItemIsHighlighted = false;
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(str);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextFont, str);
    });
    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::highlighted), this, [ = ](const QString & str) {
        m_oneItemIsHighlighted = true;
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(str);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextFont, str, false);
    });
    connect(m_fontComBox, &CFontComboBox::signalhidepopup, this, [ = ]() {
        if (m_oneItemIsHighlighted) {
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(m_oriFamily);
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextFont, m_oriFamily, false);
            m_fontComBox->setCurrentText(m_oriFamily);
            m_oneItemIsHighlighted = false;
        }
    });
    connect(m_fontComBox, &CFontComboBox::signalshowpopup, this, [ = ]() {
        m_oriFamily = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family();
    });
    connect(m_fontComBox, QOverload<const QString &>::of(&CFontComboBox::currentIndexChanged), this, [ = ](const QString & family) {
        slotUpdateTextFamilyStyle(family);
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
            slotFontSizeValueChanged(size);
            CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextSize, size);
        } else {
            qDebug() << "set error font size with str: " << str;
        }
        // 必须设置焦点到父控件，不然出现点击后更改文字的大小
        this->setFocus();
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
            slotFontSizeValueChanged(size);
        } else {
            qDebug() << "set error font size with str: " << str;
        }
        this->setFocus();
    });

    // 字体重量
    connect(m_fontHeavy, &DComboBox::currentTextChanged, this, [ = ](const QString & str) {
        // ("Black", "ExtraBold", "Bold", "DemiBold", "Medium", "Normal", "Light", "ExtraLight", "Thin")
        QString style = "Regular";
        if (str == tr("Black")) {
            style = "Black";
        } else if (str == tr("ExtraBold")) {
            style = "ExtraBold";
        } else if (str == tr("Bold")) {
            style = "Bold";
        } else if (str == tr("DemiBold")) {
            style = "DemiBold";
        } else if (str == tr("Medium")) {
            style = "Medium";
        } else if (str == tr("Normal")) {
            style = "Normal";
        } else if (str == tr("Light")) {
            style = "Light";
        } else if (str == tr("ExtraLight")) {
            style = "ExtraLight";
        } else if (str == tr("Thin")) {
            style = "Thin";
        }
        CManagerAttributeService::getInstance()->setTextFamilyStyle(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), style);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextHeavy, style);
        //隐藏调色板
        showColorPanel(DrawStatus::TextFill, QPoint(), false);
    });
}

void TextWidget::initDefaultSetting()
{
    slotUpdateTextFamilyStyle(m_fontComBox->currentText());
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

void TextWidget::updateTextWidget()
{
    m_fillBtn->updateConfigColor();
    m_fillBtn->setIsMultColorSame(true);
}

void TextWidget::updateTextColor()
{
    m_fillBtn->updateConfigColor();
//    m_fillBtn->clearFocus();
//    if (qApp->focusWidget() != nullptr) {
//        qApp->focusWidget()->hide();
//    }
}
