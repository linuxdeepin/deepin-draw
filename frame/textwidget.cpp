#include "textwidget.h"

#include <QLabel>
#include <QHBoxLayout>

#include "widgets/bigcolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/textfontlabel.h"

#include "utils/configsettings.h"

TextWidget::TextWidget(QWidget *parent)
    : QWidget(parent)
{
    BigColorButton* fillBtn = new BigColorButton("text", this);
    QColor color = QColor(ConfigSettings::instance()->value(
                        "text", "fillColor").toString());
    fillBtn->setColor(color);

    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            [=](const QString &group,  const QString &key){
        if (group == "text" && key == "fillColor")
        {
            QColor color = QColor(ConfigSettings::instance()->value(
                                      "text", "fillColor").toString());
            fillBtn->setColor(color);
        }
    });
    connect(this, &TextWidget::updateColorBtn, this, [=]{
        QColor color = QColor(ConfigSettings::instance()->value(
                                  "text", "fillColor").toString());
        fillBtn->setColor(color);
    });

    connect(fillBtn, &BigColorButton::btnCheckStateChanged, this, [=](bool show){
        showColorPanel(DrawStatus::Fill, cursor().pos(), show);
    });

    connect(this, &TextWidget::resetColorBtns, this, [=]{
        fillBtn->resetChecked();
    });

    QLabel* colBtnLabel = new QLabel(this);
    colBtnLabel->setText(tr("Fill"));

    SeperatorLine* textSeperatorLine = new SeperatorLine(this);

    QLabel* fontsizeLabel = new QLabel(this);
    fontsizeLabel->setText(tr("Size"));
    TextFontLabel* fontLabel = new TextFontLabel(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(6);
    layout->addStretch();
    layout->addWidget(fillBtn);
    layout->addWidget(colBtnLabel);
    layout->addWidget(textSeperatorLine);
    layout->addWidget(fontsizeLabel);
    layout->addWidget(fontLabel);
    layout->addStretch();
    setLayout(layout);
}

TextWidget::~TextWidget()
{}
