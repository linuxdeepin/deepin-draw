// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "colorlabel.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QImage>
#include <QRgb>

#include <cmath>

#include "utils/baseutils.h"


ColorLabel::ColorLabel(QWidget *parent)
    : QLabel(parent)
    , m_workToPick(true)
    , m_picking(true)
    , m_pressed(false)
    , m_tipPoint(this->rect().center())
{
    setMouseTracking(true);
    connect(this, &ColorLabel::clicked, this, [ = ] {
        if (m_picking && m_workToPick)
        {
            pickColor(m_clickedPos, true);
        }
    });
    setCursor(pickColorCursor());
}

//h∈(0, 360), s∈(0, 1), v∈(0, 1)
QColor ColorLabel::getColor(qreal h, qreal s, qreal v)
{
    int hi = int(h / 60) % 6;
    qreal f = h / 60 - hi;

    qreal p = v * (1 - s);
    qreal q = v * (1 - f * s);
    qreal t = v * (1 - (1 - f) * s);

    if (hi == 0) {
        return QColor(std::min(int(255 * p), 255), std::min(int(255 * q), 255), std::min(int(255 * v), 255));
    } else if (hi == 1) {
        return QColor(std::min(int(255 * t), 255), std::min(int(255 * p), 255), std::min(int(255 * v), 255));
    } else if (hi == 2) {
        return QColor(std::min(int(255 * v), 255), std::min(int(255 * p), 255), int(255 * q));
    } else if (hi == 3) {
        return QColor(std::min(int(255 * v), 255), std::min(int(255 * t), 255), std::min(int(255 * p), 255));
    } else if (hi == 4) {
        return QColor(std::min(int(255 * q), 255), std::min(int(255 * v), 255), std::min(int(255 * p), 255));
    } else {
        return QColor(std::min(int(255 * p), 255), std::min(int(255 * v), 255), std::min(int(255 * t), 255));
    }

}

void ColorLabel::setHue(int hue)
{
    m_hue = hue;
    //calImage();
    update();
}

void ColorLabel::pickColor(QPoint pos, bool picked)
{
    if (pos.x() < 0 || pos.y() < 0 || pos.x() >= this->width() || pos.y() >= this->height()) {
        return;
    }

    QPixmap pickPixmap;
    pickPixmap = this->grab(QRect(0, 0, this->width(), this->height()));
    QImage pickImg = pickPixmap.toImage();

    if (!pickImg.isNull()) {
        QRgb pickRgb = pickImg.pixel(pos);
        m_pickedColor = QColor(qRed(pickRgb), qGreen(pickRgb), qBlue(pickRgb));
    } else {
        m_pickedColor = QColor(0, 0, 0);
    }

    if (picked) {
        emit pickedColor(m_pickedColor);
    } else {
        emit signalPreViewColor(m_pickedColor);
    }
}

QSize ColorLabel::sizeHint() const
{
    return QSize(200, 200);
}

QSize ColorLabel::minimumSizeHint() const
{
    return QSize(200, 140);
}

void ColorLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    calImage();
    painter.drawImage(this->rect(), backgroundImage);
}

void ColorLabel::resizeEvent(QResizeEvent *event)
{
    //calImage();
    QLabel::resizeEvent(event);
}

void ColorLabel::enterEvent(QEvent *e)
{
    if (!m_workToPick)
        return;

    m_lastCursor = this->cursor();
    QLabel::enterEvent(e);
}

void ColorLabel::leaveEvent(QEvent *e)
{
    if (!m_workToPick)
        return;

    QLabel::leaveEvent(e);
}

void ColorLabel::mousePressEvent(QMouseEvent *e)
{
    if (!m_workToPick)
        return ;

    m_pressed = true;
    m_tipPoint = this->mapFromGlobal(cursor().pos());
    QLabel::mousePressEvent(e);
}

void ColorLabel::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_workToPick)
        return;

    if (m_pressed) {
        m_tipPoint = this->mapFromGlobal(cursor().pos());
        pickColor(m_tipPoint, false);
    }
    update();
    QLabel::mouseMoveEvent(e);
}

void ColorLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_pressed) {
        m_clickedPos = e->pos();
        emit clicked();
    }

    m_pressed = false;
    QLabel::mouseReleaseEvent(e);
}

void ColorLabel::calImage()
{
    QColor penColor;
    backgroundImage = QImage(this->width(), this->height(), QImage::Format_ARGB32);
    for (int s = 0; s < this->width(); ++s) {
        for (int v = 0; v < this->height(); ++v) {
            penColor = getColor(m_hue, qreal(s) / this->width(), qreal(v) / this->height());
            if (!penColor.isValid()) {
                continue;
            }
            backgroundImage.setPixelColor(s, this->height() - 1 - v, penColor);
        }
    }
}

ColorLabel::~ColorLabel()
{
}
