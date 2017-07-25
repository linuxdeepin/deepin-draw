#include "colorslider.h"

#include <QStyleOptionSlider>
#include <QDebug>
#include <QPainter>
#include <QStyle>

ColorSlider::ColorSlider(QWidget *parent)
    : QSlider(parent) {
    setMinimum(0);
    setMaximum(360);
    setMinimumHeight(15);
    setFixedWidth(222);
    setOrientation(Qt::Horizontal);
}

ColorSlider::~ColorSlider() {
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
    qDebug() << groove_rect;
    QSlider::paintEvent(ev);
    QRect rect(groove_rect.left(), groove_rect.top(),  groove_rect.width(), groove_rect.height()*10);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QImage backgroundImage(rect.width(), rect.height(), QImage::Format_ARGB32);
    for(qreal s = 1; s <= rect.width(); s++) {
        for(qreal v = 1; v <= rect.height(); v++) {
            QColor penColor = getColor(qreal(int(s/rect.width()*360)), 1, 1);
            backgroundImage.setPixelColor(int(s), rect.height() - int(v), penColor);
        }
    }

    backgroundImage.save(":/theme/light/images/draw/slider_bg.png", "PNG");
    painter.drawImage(rect, backgroundImage);
    QSlider::paintEvent(ev);
}
