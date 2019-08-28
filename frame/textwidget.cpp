#include "textwidget.h"

#include <DLabel>
#include <DFontComboBox>
#include <DSlider>
#include <DLineEdit>

#include <QLabel>
#include <QHBoxLayout>


#include "widgets/bigcolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/textfontlabel.h"

#include "utils/configsettings.h"
#include "utils/global.h"

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;

TextWidget::TextWidget(QWidget *parent)
    : QWidget(parent)
{
    DRAW_THEME_INIT_WIDGET("TextWidget");
    this->setObjectName("TextWidget");

    BigColorButton *fillBtn = new BigColorButton( this);
    QColor color = QColor(ConfigSettings::instance()->value(
                              "text", "fillColor").toString());
    fillBtn->setColor(color);

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
    [ = ](const QString & group,  const QString & key) {
        if (group == "text" && key == "fillColor") {
            QColor color = QColor(ConfigSettings::instance()->value(
                                      "text", "fillColor").toString());
            fillBtn->setColor(color);
        }
    });
    connect(this, &TextWidget::updateColorBtn, this, [ = ] {
        QColor color = QColor(ConfigSettings::instance()->value(
                                  "text", "fillColor").toString());
        fillBtn->setColor(color);
    });

    connect(fillBtn, &BigColorButton::btnCheckStateChanged, this, [ = ](bool show) {
        showColorPanel(DrawStatus::Fill, cursor().pos(), show);
    });

    connect(this, &TextWidget::resetColorBtns, this, [ = ] {
        fillBtn->resetChecked();
    });

    DLabel *colBtnLabel = new DLabel(this);
    colBtnLabel->setObjectName("FillLabel");
    colBtnLabel->setText(tr("填充"));

    SeperatorLine *textSeperatorLine = new SeperatorLine(this);

    DLabel *fontFamilyLabel = new DLabel(this);
    fontFamilyLabel->setText(tr("字体"));
    DFontComboBox *fontComBox = new DFontComboBox(this);
    fontComBox->setFontFilters(DFontComboBox::AllFonts);
    fontComBox->setMinimumWidth(100);


    DLabel *fontsizeLabel = new DLabel(this);
    fontsizeLabel->setObjectName("FontsizeLabel");
    fontsizeLabel->setText(tr("字号"));

    DSlider *fontSizeSlider = new DSlider(this);
    fontSizeSlider->setOrientation(Qt::Horizontal);
    fontSizeSlider->setMinimum(0);
    fontSizeSlider->setMaximum(1000);
    fontSizeSlider->setMinimumWidth(200);
    fontSizeSlider->setMaximumHeight(24);


    DLineEdit *fontSizeEdit = new DLineEdit(this);
    fontSizeEdit->setMinimumWidth(50);
    fontSizeEdit->setMaximumWidth(50);
    fontSizeEdit->setText(QString::number(fontSizeSlider->value()));


    connect(fontSizeSlider, &DSlider::valueChanged, this, [ = ](int value) {
        fontSizeEdit->setText(QString::number(value));
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(fillBtn);
    layout->addWidget(colBtnLabel);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(textSeperatorLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(fontFamilyLabel);
    layout->addWidget(fontComBox);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(fontsizeLabel);
    layout->addWidget(fontSizeSlider);
    layout->addWidget(fontSizeEdit);
    layout->addStretch();
    setLayout(layout);
}

TextWidget::~TextWidget()
{}
