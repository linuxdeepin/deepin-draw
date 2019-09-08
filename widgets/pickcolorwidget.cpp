#include "pickcolorwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QDBusInterface>
#include <QDebug>

#include "utils/global.h"
#include "service/colorpickerinterface.h"

const QSize PICKCOLOR_WIDGET_SIZE = QSize(222, 217);

PickColorWidget::PickColorWidget(DWidget *parent)
    : DWidget(parent)
{
    //DRAW_THEME_INIT_WIDGET("PickColorWidget");
    setFixedSize(222, 217);
    DLabel *titleLabel = new DLabel(this);
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
    QHBoxLayout *rgbLayout = new QHBoxLayout;
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
    connect(m_colorSlider, &ColorSlider::valueChanged, m_colorLabel, [ = ](int val) {
        m_colorLabel->setHue(val);
    });
    connect(m_colorLabel, &ColorLabel::pickedColor, this,  [ = ](QColor color) {
        setRgbValue(color, true);
    });
    connect(m_picker, &PushButton::clicked, this, [ = ] {
        ColorPickerInterface *cp = new ColorPickerInterface("com.deepin.Picker",
                                                            "/com/deepin/Picker", QDBusConnection::sessionBus(), this);
        cp->StartPick(QString("%1").arg(qApp->applicationPid()));
        connect(cp, &ColorPickerInterface::colorPicked, this, [ = ](QString uuid,
                                                                    QString colorName)
        {
            if (uuid == QString("%1").arg(qApp->applicationPid())) {
                setRgbValue(QColor(colorName), true);
            }
            m_picker->setChecked(false);
            cp->deleteLater();
        });
    });

    QVBoxLayout *mLayout = new QVBoxLayout;
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addSpacing(16);
    mLayout->addLayout(rgbLayout);
    mLayout->addSpacing(10);
    mLayout->addWidget(m_colorLabel, 0, Qt::AlignCenter);
    mLayout->addWidget(m_colorSlider, 0, Qt::AlignCenter);
    setLayout(mLayout);
}

void PickColorWidget::setRgbValue(QColor color, bool isPicked)
{
    m_redEditLabel->setEditText(QString("%1").arg(color.red()));
    m_greenEditLabel->setEditText(QString("%1").arg(color.green()));
    m_blueEditLabel->setEditText(QString("%1").arg(color.blue()));

    if (isPicked)
        emit pickedColor(color);
}

void PickColorWidget::updateColor()
{
    int r = m_redEditLabel->editText().toInt();
    int g = m_greenEditLabel->editText().toInt();
    int b = m_blueEditLabel->editText().toInt();

    if (QColor(r, g, b).isValid()) {
        emit pickedColor(QColor(r, g, b));
    }
}

void PickColorWidget::setPickedColor(bool picked)
{
    m_colorLabel->setPickColor(picked);
}

PickColorWidget::~PickColorWidget()
{
}
