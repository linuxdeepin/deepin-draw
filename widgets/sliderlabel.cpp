#include "sliderlabel.h"

#include <QHBoxLayout>
#include <QDebug>

#include "utils/configsettings.h"

const int SLIDER_WIDTH = 125;
const QSize SLIDER_SIZE = QSize(168, 24);

Slider::Slider(QWidget *parent)
    : QLabel(parent)
{
    this->setFixedSize(SLIDER_SIZE);
    setStyleSheet("Slider{"
                  "border: 1px solid rgba(0, 0, 0, 26);"
                  "border-radius: 3px; }");
    setMinimumWidth(160);
    m_slider = new QSlider(Qt::Horizontal,this);
    m_slider->setMinimum(0);
    m_slider->setMaximum(100);
    m_slider->setValue(100);
    m_slider->setFixedWidth(SLIDER_WIDTH);
    m_valueLabel = new QLabel(this);
    m_valueLabel->setObjectName("ValueLabel");
    m_valueLabel->setStyleSheet("QLabel#ValueLabel{"
                                "color: #303030;"
                                "font-size: 11px;}");
    m_valueLabel->setText("100%");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addWidget(m_slider);
    mLayout->addSpacing(2);
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
    m_titleLabel->setFixedWidth(43);
    m_titleLabel->setObjectName("TitleLabel");
    m_titleLabel->setStyleSheet("QLabel#TitleLabel {"
                                "color: #626262;"
                                "font-size: 11px;"
                                "}");
    m_slider = new Slider(this);
    m_slider->setFixedSize(SLIDER_SIZE);
    updateDrawStatus(status, widgetStatus);
    m_titleLabel->setText(m_text);

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addSpacing(4);
    mLayout->addWidget(m_titleLabel);
    mLayout->addStretch();
    mLayout->addWidget(m_slider);
    mLayout->addSpacing(6);
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

    if (widgetStatus != MiddleWidgetStatus::DrawText)
    {
        if (m_drawStatus == DrawStatus::Fill)
        {
            colorAlpha = ConfigSettings::instance()->value("common", "fillColor_alpha").toInt();
        } else
        {
            colorAlpha = ConfigSettings::instance()->value("common", "strokeColor_alpha").toInt();
        }
    } else {
        colorAlpha = ConfigSettings::instance()->value("text", "fillColor_alpha").toInt();
    }

    m_slider->setAlphaValue(colorAlpha);
}

void SliderLabel::setTitle(const QString &text) {
    m_text = text;
    m_titleLabel->setText(m_text);
}

SliderLabel::~SliderLabel() {}

