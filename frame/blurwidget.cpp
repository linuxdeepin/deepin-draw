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
    fineBtn->setCheckable(false);

    QSlider* lineWidthSlider = new QSlider(Qt::Horizontal);
    lineWidthSlider->setFixedWidth(120);
    lineWidthSlider->setMinimum(10);
    lineWidthSlider->setMaximum(80);

    connect(lineWidthSlider, &QSlider::valueChanged, this, [=](int val){
        ConfigSettings::instance()->setValue("blur", "index", val);
    });

    lineWidthSlider->setValue(ConfigSettings::instance()->value("blur", "index").toInt());

    ToolButton* boldBtn = new ToolButton;
    boldBtn->setObjectName("LineThickLineBtn");
    boldBtn->setCheckable(false);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(penLabel);
    layout->addSpacing(2);
    layout->addWidget(fineBtn);
    layout->addWidget(lineWidthSlider);
    layout->addSpacing(2);
    layout->addWidget(boldBtn);
    layout->addStretch();
    setLayout(layout);
}

BlurWidget::~BlurWidget()
{
}
