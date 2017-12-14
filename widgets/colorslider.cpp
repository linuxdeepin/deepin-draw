#include "colorslider.h"

#include <QStyleOptionSlider>
#include <QDebug>
#include <QPainter>
#include <QStyle>

ColorSlider::ColorSlider(QWidget *parent)
    : QSlider(parent) {
    setMinimum(0);
    setMaximum(360);
    setOrientation(Qt::Horizontal);
    this->setRange(5, 355);
    setFixedSize(222, 15);
    setStyleSheet("ColorSlider::handle:horizontal {\
                  border: 1px solid rgba(0, 0, 0, 150);\
                  width: 5px;\
                  margin: 0;}");
}

ColorSlider::~ColorSlider()
{
}

//h∈(0, 360), s∈(0, 1), v∈(0, 1)
QColor ColorSlider::getColor(qreal h, qreal s, qreal v) {
    int hi = int(h/60)%6;
    qreal f = h/60 - hi;

    qreal p = v*(1 - s);
    qreal q = v*(1 - f*s);
    qreal t = v*(1- (1 - f)*s);

    if (hi == 0) {
        return QColor(std::min(int(255*v), 255), std::min(int(255*t), 255), std::min(int(255*p), 255));
    } else if (hi == 1) {
        return QColor(std::min(int(255*q), 255), std::min(int(255*v), 255), std::min(int(255*p), 255));
    } else if(hi == 2) {
        return QColor(std::min(int(255*p), 255), std::min(int(255*v), 255), std::min(int(255*t), 255));
    } else if (hi == 3) {
        return QColor(std::min(int(255*p), 255), std::min(int(255*q), 255), std::min(int(255*v), 255));
    } else if(hi == 4) {
        return QColor(std::min(int(255*t), 255), std::min(int(255*p), 255), std::min(int(255*v), 255));
    } else {
        return QColor(std::min(int(255*v), 255), std::min(int(255*p), 255), int(255*q));
    }
}

void ColorSlider::paintEvent(QPaintEvent *ev) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
    if (tickPosition() != NoTicks) {
        opt.subControls |= QStyle::SC_SliderTickmarks;
    }

    QRect groove_rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    int spacing = 6;
    QRect rect(groove_rect.left(), groove_rect.top(),  groove_rect.width(), groove_rect.height());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QImage backgroundImage(rect.width(), rect.height() - spacing, QImage::Format_ARGB32);

    for(qreal s = 0; s < backgroundImage.width(); s++) {
        for(qreal v = 0; v < backgroundImage.height(); v++) {
            QColor penColor = getColor(qreal(int(s/rect.width()*360)), 1, 1);
            backgroundImage.setPixelColor(std::min(int(s), rect.width() - 1), backgroundImage.height() - int(v), penColor);
        }
    }

    painter.drawImage(QRect(rect.x(), rect.y() + 2,
        rect.width(), rect.height() - spacing), backgroundImage);
    QSlider::paintEvent(ev);
}
