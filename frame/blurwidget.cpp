 #include "blurwidget.h"

#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>

#include "widgets/toolbutton.h"
#include "utils/configsettings.h"

BlurWidget::BlurWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel* penLabel = new QLabel(this);
    penLabel->setText(tr("Width"));
    ToolButton* fineBtn = new ToolButton(this);
    fineBtn->setObjectName("LineMostThinBtn");
    fineBtn->setCheckable(false);

    QSlider* lineWidthSlider = new QSlider(Qt::Horizontal, this);
    lineWidthSlider->setFixedWidth(120);
    lineWidthSlider->setMinimum(10);
    lineWidthSlider->setMaximum(80);

    connect(lineWidthSlider, &QSlider::valueChanged, this, [=](int val){
        ConfigSettings::instance()->setValue("blur", "index", val);
    });
    connect(ConfigSettings::instance(), &ConfigSettings::configChanged, this,
            [=](const QString &group, const QString &key){
        if (group == "blur" && key == "index")
        {
            int index = ConfigSettings::instance()->value(group, key).toInt();
            lineWidthSlider->setValue(index);
        }
    });
    lineWidthSlider->setValue(ConfigSettings::instance()->value("blur", "index").toInt());

    ToolButton* boldBtn = new ToolButton(this);
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
