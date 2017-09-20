#include "sliderlabel.h"

#include <QHBoxLayout>

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
}

Slider::~Slider() {}

SliderLabel::SliderLabel(QString text, QWidget* parent)
    : QLabel(parent)
{
    m_text = text;

    m_titleLabel = new QLabel(this);
    m_slider = new Slider(this);

    m_titleLabel->setText(m_text);

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addStretch();
    mLayout->addWidget(m_titleLabel);
    mLayout->addSpacing(4);
    mLayout->addWidget(m_slider, 0, Qt::AlignRight);
    setLayout(mLayout);
}

void SliderLabel::setTitle(const QString &text) {
    m_text = text;
    m_titleLabel->setText(m_text);
}

SliderLabel::~SliderLabel() {}

