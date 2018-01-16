#include "pickcolorwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#include "utils/global.h"

const QSize PICKCOLOR_WIDGET_SIZE = QSize(222, 217);

PickColorWidget::PickColorWidget(QWidget *parent)
    : QWidget(parent)
{
    DRAW_THEME_INIT_WIDGET("PickColorWidget");
    setFixedSize(222, 217);
    QLabel* titleLabel = new QLabel(this);
    titleLabel->setText("RGB");
    titleLabel->setObjectName("TitleLabel");

    m_redEditLabel = new EditLabel(this);
    m_redEditLabel->setTitle("R");
    m_redEditLabel->setEditText("255");
    connect(m_redEditLabel, &EditLabel::editTextChanged,
            this, &PickColorWidget::updateColor);

    m_greenEditLabel = new EditLabel(this);
    m_greenEditLabel->setTitle("G");
    m_greenEditLabel->setEditText("255");
    connect(m_greenEditLabel, &EditLabel::editTextChanged,
            this, &PickColorWidget::updateColor);

    m_blueEditLabel = new EditLabel(this);
    m_blueEditLabel->setTitle("B");
    m_blueEditLabel->setEditText("255");
    connect(m_blueEditLabel, &EditLabel::editTextChanged,
            this, &PickColorWidget::updateColor);

    m_picker = new PushButton(this);
    m_picker->setFixedSize(24, 24);
    m_picker->setObjectName("PickerBtn");
    QHBoxLayout* rgbLayout = new QHBoxLayout;
    rgbLayout->setMargin(0);
    rgbLayout->setSpacing(0);
    rgbLayout->addWidget(titleLabel);
    rgbLayout->addSpacing(25);
    rgbLayout->addWidget(m_redEditLabel);
    rgbLayout->addWidget(m_greenEditLabel);
    rgbLayout->addWidget(m_blueEditLabel);
    rgbLayout->addSpacing(4);
    rgbLayout->addWidget(m_picker);
    m_colorSlider = new ColorSlider(this);
    m_colorSlider->setObjectName("ColorfulSlider");
    m_colorSlider->setFixedSize(222, 14);
//    m_colorSlider->setMinimum(0);
//    m_colorSlider->setMaximum(355);

    m_colorLabel = new ColorLabel(this);
    m_colorLabel->setFixedSize(222, 136);
    connect(m_colorSlider, &ColorSlider::valueChanged, m_colorLabel, [=](int val){
        qDebug() << "pickColorLabel:" << val;
        m_colorLabel->setHue(val);
    });
    connect(m_colorLabel, &ColorLabel::pickedColor, this, &PickColorWidget::setRgbValue);
    connect(m_picker, &PushButton::clicked, this, [=]{
        if (m_picker->getChecked())
        {
            m_colorLabel->setPickColor(true);
        } else {
            m_colorLabel->setPickColor(false);
        }
    });

    QVBoxLayout* mLayout = new QVBoxLayout;
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addSpacing(16);
    mLayout->addLayout(rgbLayout);
    mLayout->addSpacing(10);
    mLayout->addWidget(m_colorLabel, 0, Qt::AlignCenter);
    mLayout->addWidget(m_colorSlider, 0, Qt::AlignCenter);
    setLayout(mLayout);
}

void PickColorWidget::setRgbValue(QColor color)
{
    m_redEditLabel->setEditText(QString("%1").arg(color.red()));
    m_greenEditLabel->setEditText(QString("%1").arg(color.green()));
    m_blueEditLabel->setEditText(QString("%1").arg(color.blue()));

    emit pickedColor(color);
}

void PickColorWidget::updateColor()
{
    int r = m_redEditLabel->editText().toInt();
    int g = m_greenEditLabel->editText().toInt();
    int b = m_blueEditLabel->editText().toInt();

    if (QColor(r, g, b).isValid())
    {
        emit pickedColor(QColor(r, g, b));
    }
}

PickColorWidget::~PickColorWidget()
{
}
