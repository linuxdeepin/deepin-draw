#include "colorpanel.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QGridLayout>

#include "utils/baseutils.h"
#include "utils/global.h"
#include "colorlabel.h"

ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QPushButton(parent) {
    m_color = color;
    setFixedSize(20, 20);
}

void ColorButton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(m_color);

    painter.setBrush(QBrush(m_color));

    QPen pen;
    pen.setWidth(2);
    pen.setColor(m_color);
    painter.setPen(pen);
    painter.drawRoundedRect(this->rect(), 4, 4);
}

ColorButton::~ColorButton() {}

ColorPanel::ColorPanel(QWidget *parent)
    : QWidget(parent) {
    DRAW_THEME_INIT_WIDGET("ColorPanel");
//    setMinimumHeight(400);
    QStringList colList;
    colList << QString("#ff0c0c") << QString("#fe3c3b") << QString("#fd6867") << QString("#fd9694")
            << QString("#fcc4c1") << QString("#f8e0d6") << QString("#e4c299") << QString("#f2aa46")
            << QString("#fd9d0f") << QString("#f6b443") << QString("#eecb77") << QString("#f0ee4e")
            << QString("#f4fb00") << QString("#f6f96d") << QString("#f4f6a6") << QString("f3f3d6")
            << QString("#e9eedc") << QString("#dde8cb") << QString("#ccdfb0") << QString("#9cd972")
            << QString("#4ec918") << QString("#5cc850") << QString("#6bc989") << QString("#53ac6d")
            << QString("#72b88e") << QString("#7cc8cd") << QString("#97d1d4") << QString("#c9e1e1")
            << QString("#c1dee7") << QString("#93ceed") << QString("#76c3f1") << QString("#49b2f6")
            << QString("#119fff") << QString("#0192ea") << QString("#3d7ddd") << QString("#92cdfb")
            << QString("#99cffa") << QString("#ececf8") << QString("#ccc9f9") << QString("#b2acf9")
            << QString("#958ef9") << QString("#7c6ffa") << QString("#8a47fb") << QString("#6b1aef")
            << QString("#952dfd") << QString("#af39e4") << QString("#c174da") << QString("#c587d9")
            << QString("#dbb4c1") << QString("#cf8c86") << QString("#b45f51") << QString("#865e4f")
            << QString("#694d48") << QString("#ffffff") << QString("#d4d4d4") << QString("#919191")
            << QString("#626262") << QString("#404040") << QString("#000000");

    QList<ColorButton*> cButtonList;
    QGridLayout* gLayout = new QGridLayout;
    gLayout->setSpacing(3);

    for(int i = 1; i < colList.length(); i++) {
        ColorButton* cb = new ColorButton(QColor(colList[i]), this);
        cButtonList.append(cb);
        gLayout->addWidget(cb, i/10, i%10);
    }

    m_sliderLabel = new SliderLabel("Alpha", this);

    m_editLabel = new EditLabel(this);
    m_editLabel->setTitle(tr("Color"));
    m_editLabel->setEditText("#FF0000");
    m_colorfulBtn = new PushButton(this);
    m_colorfulBtn->setObjectName("ColorFulButton");

     QHBoxLayout* colorLayout = new QHBoxLayout;
    colorLayout->setMargin(0);
    colorLayout->setSpacing(0);
    colorLayout->addWidget(m_editLabel);
    colorLayout->addWidget(m_colorfulBtn);

    QSlider* colorfulSlider = new QSlider(Qt::Horizontal, this);
    colorfulSlider->setObjectName("ColorfulSlider");
    colorfulSlider->setFixedSize(222, 20);
    colorfulSlider->setMinimum(0);
    colorfulSlider->setMaximum(360);


    ColorLabel* pickColorLabel = new ColorLabel(this);
    pickColorLabel->setFixedSize(200, 200);
    connect(colorfulSlider, &QSlider::valueChanged, pickColorLabel, &ColorLabel::setHue);

    QVBoxLayout* mLayout = new QVBoxLayout(this);
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addLayout(gLayout);
    mLayout->addWidget(m_sliderLabel);
    mLayout->addLayout(colorLayout);
    mLayout->addSpacing(20);
    mLayout->addWidget(pickColorLabel);
    mLayout->addWidget(colorfulSlider);

    setLayout(mLayout);
}

ColorPanel::~ColorPanel() {}
