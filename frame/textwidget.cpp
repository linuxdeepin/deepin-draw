#include "textwidget.h"

#include <DLabel>
#include "widgets/textcolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/textfontlabel.h"
#include "drawshape/cdrawparamsigleton.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

TextWidget::TextWidget(DWidget *parent)
    : DWidget(parent)
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

    DLabel *colBtnLabel = new DLabel(this);
    colBtnLabel->setText(tr("填充"));

    SeperatorLine *textSeperatorLine = new SeperatorLine(this);

    DLabel *fontFamilyLabel = new DLabel(this);
    fontFamilyLabel->setText(tr("字体"));
    m_fontComBox = new DFontComboBox(this);
    m_fontComBox->setFontFilters(DFontComboBox::AllFonts);
    m_fontComBox->setMinimumWidth(100);
    m_fontComBox->setCurrentIndex(0);
    QString strFont = m_fontComBox->currentText();
    CDrawParamSigleton::GetInstance()->setTextFont(strFont);

    DLabel *fontsizeLabel = new DLabel(this);
    fontsizeLabel->setText(tr("字号"));

    m_fontSizeSlider = new DSlider(Qt::Horizontal, this);


    m_fontSizeSlider->setMinimum(8);
    m_fontSizeSlider->setMaximum(1000);
    m_fontSizeSlider->setMinimumWidth(200);
    m_fontSizeSlider->setMaximumHeight(24);
    m_fontSizeSlider->setValue(int(CDrawParamSigleton::GetInstance()->getTextSize()));

    m_fontSizeEdit = new DLineEdit(this);
    m_fontSizeEdit->lineEdit()->setValidator(new QIntValidator(8, 1000, this));
    m_fontSizeEdit->setClearButtonEnabled(false);
    m_fontSizeEdit->setFixedWidth(55);
    m_fontSizeEdit->setText(QString::number(m_fontSizeSlider->value()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addWidget(colBtnLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(textSeperatorLine);
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
        CDrawParamSigleton::GetInstance()->setTextFont(str);
        emit signalTextAttributeChanged();
    });

    ///字体大小
    connect(m_fontSizeSlider, &DSlider::valueChanged, this, [ = ](int value) {
        if (m_isUsrDragSlider) {
            m_fontSizeEdit->setText(QString::number(value));
            CDrawParamSigleton::GetInstance()->setTextSize(value);
            emit signalTextAttributeChanged();
        }
    });

    connect(m_fontSizeEdit, &DLineEdit::textEdited, this, [ = ](const QString & str) {
        int value = str.trimmed().toInt();
        m_fontSizeSlider->setValue(value);

        m_fontSizeEdit->setText(QString::number(value));
        CDrawParamSigleton::GetInstance()->setTextSize(value);
        emit signalTextAttributeChanged();
    });

    connect(m_fontSizeSlider, &DSlider::sliderPressed, this, [ = ]() {
        m_isUsrDragSlider = true;
    });

    connect(m_fontSizeSlider, &DSlider::sliderReleased, this, [ = ]() {
        m_isUsrDragSlider = false;
    });
}

void TextWidget::updateTextWidget()
{
    m_fillBtn->updateConfigColor();
    QFont font = CDrawParamSigleton::GetInstance()->getTextFont();

    if (m_fontComBox->currentText() != font.family()) {
        m_fontComBox->setFont(font);
    }

    int fontSize = int(CDrawParamSigleton::GetInstance()->getTextSize());

    if (fontSize != m_fontSizeSlider->value()) {
        m_fontSizeSlider->setValue(fontSize);
        m_fontSizeEdit->setText(QString("%1").arg(fontSize));
    }
}
