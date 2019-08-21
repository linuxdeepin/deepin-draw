#include "blurwidget.h"

#include <DLabel>
#include <DSlider>

#include <QHBoxLayout>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"

const int BTN_SPACINT = 2;

BlurWidget::BlurWidget(QWidget *parent)
    : QWidget(parent)
{
    DLabel *penLabel = new DLabel(this);
    penLabel->setObjectName("WidthLabel");
    penLabel->setText(tr("类型"));

    DSlider *lineWidthSlider = new DSlider(Qt::Horizontal, this);
    lineWidthSlider->setTickInterval(4);
    lineWidthSlider->setSingleStep(1);
    lineWidthSlider->setMinimum(20);
    lineWidthSlider->setMaximum(160);
    lineWidthSlider->setMinimumWidth(200);
    lineWidthSlider->setOrientation(Qt::Horizontal);
    lineWidthSlider->setTickPosition(DSlider::TicksBothSides);


    DLabel *lineWidthLabel = new DLabel(this);
    lineWidthLabel->setObjectName("WidthLabel");
    lineWidthLabel->setText(QString("%1px").arg(lineWidthSlider->value()));

    connect(lineWidthSlider, &DSlider::valueChanged, this, [ = ](int value) {
        lineWidthLabel->setText(QString("%1px").arg(value));
    });


    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(penLabel);
    layout->addSpacing(BTN_SPACINT);
//    layout->addWidget(fineBtn);
    layout->addWidget(lineWidthSlider);
    layout->addWidget(lineWidthLabel);
    layout->addStretch();
    setLayout(layout);
}

BlurWidget::~BlurWidget()
{
}
