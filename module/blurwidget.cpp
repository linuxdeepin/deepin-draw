#include "blurwidget.h"

#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"

BlurWidget::BlurWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel* penLabel = new QLabel;
    penLabel->setText(tr("Width"));
    ToolButton* fineBtn = new ToolButton;
    fineBtn->setObjectName("LineMostThinBtn");

    QSlider* lineWidthSlider = new QSlider(Qt::Horizontal);
    lineWidthSlider->setMinimum(20);
    lineWidthSlider->setMaximum(160);

    connect(lineWidthSlider, &QSlider::valueChanged, this, [=](int val){
        ConfigSettings::instance()->setValue("blur", "index", val);
    });

    lineWidthSlider->setValue(ConfigSettings::instance()->value("blur", "index").toInt());

    ToolButton* boldBtn = new ToolButton;
    boldBtn->setObjectName("LineThickLineBtn");
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(penLabel);
    layout->addSpacing(4);
    layout->addWidget(fineBtn);
    layout->addWidget(lineWidthSlider);
    layout->addSpacing(4);
    layout->addWidget(boldBtn);
    setLayout(layout);
}

BlurWidget::~BlurWidget()
{}
