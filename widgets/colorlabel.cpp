#include "colorlabel.h"

#include <QPainter>
#include <QDebug>

#include <cmath>

ColorLabel::ColorLabel(QWidget *parent)
    : QLabel(parent) {
}

//h∈(0, 360), s∈(0, 1), v∈(0, 1)
QColor ColorLabel::getColor(qreal h, qreal s, qreal v) {
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

void ColorLabel::setHue(int hue) {
    m_hue = hue;
    update();
}

void ColorLabel::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QImage backgroundImage(this->width(), this->height(), QImage::Format_ARGB32);
    qDebug() << "paintEvent:" << this->width() << this->height();

    for(qreal s = 0; s < this->width(); s++) {
        for(qreal v = 0; v < this->height(); v++) {
            QColor penColor = getColor(m_hue, s/this->width(), v/this->height());
            backgroundImage.setPixelColor(int(s), this->height() - 1 - int(v), penColor);
        }
    }

    painter.drawImage(this->rect(), backgroundImage);
}

ColorLabel::~ColorLabel() {
}
