/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     RenRan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "colorlabel.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QImage>
#include <QRgb>

#include <cmath>

#include "utils/baseutils.h"

#include "application.h"

//const QSize COLOR_TIPS_SIZE = QSize(50, 50);

ColorLabel::ColorLabel(DWidget *parent)
    : DLabel(parent)
    , m_workToPick(/*false*/ true)
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
}

//h∈(0, 360), s∈(0, 1), v∈(0, 1)
QColor ColorLabel::getColor(qreal h, qreal s, qreal v)
{
    int hi = int(h / 60) % 6;
    qreal f = h / 60 - hi;

    qreal p = v * (1 - s);
    qreal q = v * (1 - f * s);
    qreal t = v * (1 - (1 - f) * s);

//    if (hi == 0) {
//        return QColor(std::min(int(255 * v), 255), std::min(int(255 * t), 255), std::min(int(255 * p), 255));
//    } else if (hi == 1) {
//        return QColor(std::min(int(255 * q), 255), std::min(int(255 * v), 255), std::min(int(255 * p), 255));
//    } else if (hi == 2) {
//        return QColor(std::min(int(255 * p), 255), std::min(int(255 * v), 255), std::min(int(255 * t), 255));
//    } else if (hi == 3) {
//        return QColor(std::min(int(255 * p), 255), std::min(int(255 * q), 255), std::min(int(255 * v), 255));
//    } else if (hi == 4) {
//        return QColor(std::min(int(255 * t), 255), std::min(int(255 * p), 255), std::min(int(255 * v), 255));
//    } else {
//        return QColor(std::min(int(255 * v), 255), std::min(int(255 * p), 255), int(255 * q));
//    }

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

//QColor ColorLabel::getPickedColor()
//{
//    return m_pickedColor;
//}

//void ColorLabel::setPickColor(bool picked)
//{
//    m_workToPick = picked;
//}

void ColorLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QImage backgroundImage(this->width(), this->height(), QImage::Format_ARGB32);
    for (qreal s = 0; s < this->width(); s++) {
        for (qreal v = 0; v < this->height(); v++) {
            QColor penColor = getColor(m_hue, s / this->width(), v / this->height());
            if (!penColor.isValid()) {
                continue;
            }
            backgroundImage.setPixelColor(int(s), this->height() - 1 - int(v), penColor);
        }
    }

    painter.drawImage(this->rect(), backgroundImage);
}

void ColorLabel::enterEvent(QEvent *e)
{
    if (!m_workToPick)
        return;

    m_lastCursor = this->cursor();
    drawApp->setApplicationCursor(pickColorCursor());
    QLabel::enterEvent(e);
}

void ColorLabel::leaveEvent(QEvent *e)
{
    if (!m_workToPick)
        return;

    drawApp->setApplicationCursor(m_lastCursor);
    QLabel::leaveEvent(e);
}

void ColorLabel::mousePressEvent(QMouseEvent *e)
{
    if (!m_workToPick)
        return ;

    m_pressed = true;
    m_tipPoint = this->mapFromGlobal(cursor().pos());
//    pickColor(m_tipPoint, true);
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

ColorLabel::~ColorLabel()
{
}
