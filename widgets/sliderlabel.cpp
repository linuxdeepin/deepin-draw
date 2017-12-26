#include "sliderlabel.h"

#include <QHBoxLayout>
#include <QDebug>

#include "utils/configsettings.h"

Slider::Slider(QWidget *parent)
    : QLabel(parent)
{
    setStyleSheet("Slider { border: 1px solid rgba(0, 0, 0, 100);"
                                            "border-radius: 4px;"
                             "}");
    setMinimumWidth(160);
    m_slider = new QSlider(Qt::Horizontal,this);
    m_slider->setMinimum(0);
    m_slider->setMaximum(100);
    m_slider->setValue(100);
    m_slider->setFixedWidth(110);
    m_valueLabel = new QLabel(this);
    m_valueLabel->setText("100%");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addSpacing(6);
    mLayout->addWidget(m_slider);
    mLayout->addSpacing(20);
    mLayout->addWidget(m_valueLabel);
    setLayout(mLayout);
    connect(m_slider, &QSlider::valueChanged, this, [=](int value){
        emit valueChanged(value);
        m_valueLabel->setText(QString("%1%").arg(value));
    });
}

void Slider::setAlphaValue(int value)
{
    qDebug() << "value..." << value;
    m_valueLabel->setText(QString("%1%").arg(value));
    m_slider->setValue(value);
}

int Slider::alphaValue()
{
    return m_slider->value();
}

Slider::~Slider() {}

SliderLabel::SliderLabel(QString text, DrawStatus status,
                         MiddleWidgetStatus widgetStatus, QWidget* parent)
    : QLabel(parent)
{
    m_text = text;
    m_drawStatus = status;

    m_titleLabel = new QLabel(this);
    m_slider = new Slider(this);
    updateDrawStatus(status, widgetStatus);
    m_titleLabel->setText(m_text);

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addStretch();
    mLayout->addWidget(m_titleLabel);
    mLayout->addSpacing(4);
    mLayout->addWidget(m_slider, 0, Qt::AlignRight);
    setLayout(mLayout);

    connect(m_slider, &Slider::valueChanged, this, &SliderLabel::alphaChanged);
}

int SliderLabel::alpha()
{
    return m_slider->alphaValue();
}

void SliderLabel::setAlpha(int val)
{
    m_slider->setAlphaValue(val);
}

void SliderLabel::updateDrawStatus(DrawStatus status,
                                   MiddleWidgetStatus widgetStatus)
{
    int colorAlpha;
    m_drawStatus = status;
    m_widgetStatus = widgetStatus;

    if (widgetStatus != MiddleWidgetStatus::Cut)
    {
        if (m_drawStatus == DrawStatus::Fill)
        {
            colorAlpha = ConfigSettings::instance()->value("common", "fillColor_alpha").toInt();
        } else
        {
            colorAlpha = ConfigSettings::instance()->value("common", "strokeColor_alpha").toInt();
        }
    } else {
        colorAlpha = ConfigSettings::instance()->value("common", "fillColor_alpha").toInt();
    }

    m_slider->setAlphaValue(colorAlpha);
}

void SliderLabel::setTitle(const QString &text) {
    m_text = text;
    m_titleLabel->setText(m_text);
}

SliderLabel::~SliderLabel() {}

