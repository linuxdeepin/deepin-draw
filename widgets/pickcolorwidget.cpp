#include "pickcolorwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

PickColorWidget::PickColorWidget(QWidget *parent)
    : QWidget(parent) {
    QLabel* titleLabel = new QLabel(this);
    titleLabel->setText("RGB");
    m_redEditLabel = new EditLabel(this);
    m_redEditLabel->setTitle("R");
    m_redEditLabel->setEditText("255");

    m_greenEditLabel = new EditLabel(this);
    m_greenEditLabel->setTitle("G");
    m_greenEditLabel->setEditText("255");

    m_blueEditLabel = new EditLabel(this);
    m_blueEditLabel->setTitle("B");
    m_blueEditLabel->setEditText("255");

    m_picker = new PushButton(this);
    m_picker->setObjectName("PickerBtn");
    m_picker->setFixedSize(24, 24);

    QHBoxLayout* rgbLayout = new QHBoxLayout;
    rgbLayout->setMargin(0);
    rgbLayout->setSpacing(0);
    rgbLayout->addWidget(titleLabel);
    rgbLayout->addSpacing(5);
    rgbLayout->addWidget(m_redEditLabel);
    rgbLayout->addWidget(m_greenEditLabel);
    rgbLayout->addWidget(m_blueEditLabel);
    rgbLayout->addWidget(m_picker);

    m_colorSlider = new ColorSlider(this);
    m_colorSlider->setObjectName("ColorfulSlider");
    m_colorSlider->setFixedSize(222, 15);
    m_colorSlider->setMinimum(0);
    m_colorSlider->setMaximum(355);

    m_colorLabel = new ColorLabel(this);
    m_colorLabel->setFixedSize(222, 136);
    connect(m_colorSlider, &ColorSlider::valueChanged, m_colorLabel, [=](int val){
        qDebug() << "pickColorLabel:" << val;
        m_colorLabel->setHue(val);
    });
    connect(m_colorLabel, &ColorLabel::pickedColor, this, &PickColorWidget::setRgbValue);

    QVBoxLayout* mLayout = new QVBoxLayout;
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addLayout(rgbLayout);
    mLayout->addWidget(m_colorLabel);
    mLayout->addWidget(m_colorSlider);
    setLayout(mLayout);
}

void PickColorWidget::setRgbValue(QColor color) {
    m_redEditLabel->setEditText(QString("%1").arg(color.red()));
    m_greenEditLabel->setEditText(QString("%1").arg(color.green()));
    m_blueEditLabel->setEditText(QString("%1").arg(color.blue()));

    emit pickedColor(color);
}

PickColorWidget::~PickColorWidget() {
}
