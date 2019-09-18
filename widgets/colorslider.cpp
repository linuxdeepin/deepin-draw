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
#include "colorslider.h"

#include <QStyleOptionSlider>
#include <QDebug>
#include <QPainter>
#include <QStyle>
//#include <std>
//DWIDGET_BEGIN_NAMESPACE
DWIDGET_USE_NAMESPACE

ColorSlider::ColorSlider(DWidget *parent)
    : DSlider(Qt::Horizontal, parent)
{
    setMinimum(5);
    setMaximum(355);


}

ColorSlider::~ColorSlider()
{
}

//h∈(0, 360), s∈(0, 1), v∈(0, 1)
QColor ColorSlider::getColor(qreal h, qreal s, qreal v)
{
    int hi = int(h / 60) % 6;
    qreal f = h / 60 - hi;

    qreal p = v * (1 - s);
    qreal q = v * (1 - f * s);
    qreal t = v * (1 - (1 - f) * s);

    if (hi == 0) {
        return QColor(std::min(int(255 * v), 255), std::min(int(255 * t), 255), std::min(int(255 * p), 255));
    } else if (hi == 1) {
        return QColor(std::min(int(255 * q), 255), std::min(int(255 * v), 255), std::min(int(255 * p), 255));
    } else if (hi == 2) {
        return QColor(std::min(int(255 * p), 255), std::min(int(255 * v), 255), std::min(int(255 * t), 255));
    } else if (hi == 3) {
        return QColor(std::min(int(255 * p), 255), std::min(int(255 * q), 255), std::min(int(255 * v), 255));
    } else if (hi == 4) {
        return QColor(std::min(int(255 * t), 255), std::min(int(255 * p), 255), std::min(int(255 * v), 255));
    } else {
        return QColor(std::min(int(255 * v), 255), std::min(int(255 * p), 255), int(255 * q));
    }
}

void ColorSlider::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)

    //    QStyleOptionSlider opt;
    //    initStyleOption(&opt);

    //    opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
    //    if (tickPosition() != NoTicks) {
    //        opt.subControls |= QStyle::SC_SliderTickmarks;
    //    }


    //    QRect groove_rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    //    QRect rect(groove_rect.left(), groove_rect.top(),  groove_rect.width() + 1, groove_rect.height() + 5);

    ////    QRect rect = QRect (this->rect().x(), this->rect().y(),
    ////                        this->rect().width() + 1, this->rect().height() + 5);

    //    QPainter painter(this);
    //    painter.setRenderHint(QPainter::Antialiasing);
    //    QImage backgroundImage(rect.width(), rect.height(), QImage::Format_ARGB32);

    //    for (qreal s = 0; s <= backgroundImage.width(); s++) {
    //        for (qreal v = 1; v <= backgroundImage.height() ; v++) {
    //            QColor penColor = getColor(qreal(int(s / rect.width() * 360)), 1, 1);
    //            backgroundImage.setPixelColor(std::min(int(s), rect.width() - 1), backgroundImage.height() - int(v), penColor);
    //        }
    //    }

    //    painter.drawImage(QRect(rect.x(), rect.y() - 5, rect.width(),
    //                            rect.height()), backgroundImage);

    //    qreal delat = rect.width() / 360.; //360.为maximum()的浮点型
    //    qreal x = value() * delat;



    //    QPen pen;
    //    pen.setWidth(1);
    //    pen.setColor(QColor(Qt::blue));
    //    painter.setPen(pen);
    //    painter.setBrush(QBrush(Qt::blue));
    ////    painter.drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    //    painter.drawRect(QRectF(QPointF(x - 3, rect.top() - 8), QPointF(x + 2, rect.bottom())));
    //    DSlider::paintEvent(ev);
}
