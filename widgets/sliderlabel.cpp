#include "sliderlabel.h"

#include <QHBoxLayout>


Slider::Slider(QWidget *parent)
    : QLabel(parent) {
    m_slider = new QSlider(Qt::Horizontal,this);
    m_valueLabel = new QLabel(this);
    m_valueLabel->setText("100%");

    QHBoxLayout* mLayout = new QHBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addWidget(m_slider);
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
    mLayout->addWidget(m_titleLabel);
    mLayout->addWidget(m_slider);
    setLayout(mLayout);
}

void SliderLabel::setTitle(const QString &text) {
    m_text = text;
    m_titleLabel->setText(m_text);
}

SliderLabel::~SliderLabel() {}

