#include "colorlabel.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QImage>
#include <cmath>

#include "utils/baseutils.h"

ColorLabel::ColorLabel(QWidget *parent)
    : QLabel(parent), m_picking(true), m_pressed(false) {

    connect(this, &ColorLabel::clicked, this, [=]{
        if (m_picking) {
            pickColor(m_clickedPos);
        }
    });
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

void ColorLabel::pickColor(QPoint pos) {
    QPixmap pickPixmap;
    pickPixmap = this->grab(QRect(0, 0, this->width(), this->height()));

    QImage pickImg = pickPixmap.toImage();
    if (!pickImg.isNull()) {
        QRgb pickRgb = pickImg.pixel(pos);
        m_pickedColor = QColor(qRed(pickRgb), qGreen(pickRgb), qBlue(pickRgb));
    } else {
        m_pickedColor = QColor(0, 0, 0);
    }

    emit pickedColor(m_pickedColor);
}

QColor ColorLabel::getPickedColor() {
    return m_pickedColor;
}

void ColorLabel::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QImage backgroundImage(this->width(), this->height(), QImage::Format_ARGB32);

    for(qreal s = 0; s < this->width(); s++) {
        for(qreal v = 0; v < this->height(); v++) {
            QColor penColor = getColor(m_hue, s/this->width(), v/this->height());
            backgroundImage.setPixelColor(int(s), this->height() - 1 - int(v), penColor);
        }
    }

    painter.drawImage(this->rect(), backgroundImage);
}

void ColorLabel::enterEvent(QEvent *e) {
    m_lastCursor = this->cursor();
    qApp->setOverrideCursor(setCursorShape("pickcolor"));
    QLabel::enterEvent(e);
}

void ColorLabel::leaveEvent(QEvent *) {
    qApp->setOverrideCursor(m_lastCursor);
}

void ColorLabel::mousePressEvent(QMouseEvent *e) {
    m_pressed = true;

    QLabel::mousePressEvent(e);
}

void ColorLabel::mouseReleaseEvent(QMouseEvent *e) {
    if (m_pressed) {
        emit clicked();
    }

    m_clickedPos = e->pos();
    QLabel::mouseReleaseEvent(e);
}

ColorLabel::~ColorLabel() {
}
