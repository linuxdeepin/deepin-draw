/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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

#include "graphicsgloweffect.h"

#include <QDebug>

DWIDGET_USE_NAMESPACE

GraphicsGlowEffect::GraphicsGlowEffect(QObject *parent) :
    QGraphicsEffect(parent),
    m_xOffset(0),
    m_yOffset(0),
    m_distance(4.0f),
    m_blurRadius(10.0f),
    m_color(0, 0, 0, 80)
{
}

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE


void GraphicsGlowEffect::cachePixmap(const QPixmap &sourcePx)
{
    // Calculate size for the background image
    QSize scaleSize(sourcePx.size().width() + 2 * distance(), sourcePx.size().height() + 2 * distance());

    if (cacheBlurPixmap.size() == scaleSize) {
        return;
    }

    QImage tmpImg(scaleSize, QImage::Format_ARGB32_Premultiplied);
    QPixmap scaled = sourcePx.scaled(scaleSize);
    tmpImg.fill(0);
    QPainter tmpPainter(&tmpImg);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPointF(-distance(), -distance()), scaled);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmpImg.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmpImg, blurRadius(), false, true);
    blurPainter.end();

    tmpImg = blurred;

    // blacken the image...
    tmpPainter.begin(&tmpImg);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmpImg.rect(), color());
    tmpPainter.end();

    cacheBlurPixmap = QPixmap::fromImage(tmpImg);
}

void GraphicsGlowEffect::draw(QPainter *painter)
{
    // if nothing to show outside the item, just draw source
    if ((blurRadius() + distance()) <= 0) {
        drawSource(painter);
        return;
    }

    PixmapPadMode mode = QGraphicsEffect::PadToEffectiveBoundingRect;
    QPoint offset;
    const QPixmap sourcePx = sourcePixmap(Qt::DeviceCoordinates, &offset, mode);

    // return if no source
    if (sourcePx.isNull()) {
        return;
    }

    cachePixmap(sourcePx);

    // save world transform
    QTransform restoreTransform = painter->worldTransform();
    painter->setWorldTransform(QTransform());

    // draw the blurred shadow...
    painter->drawPixmap(offset, cacheBlurPixmap);

    // draw the actual pixmap...
    painter->drawPixmap(offset, sourcePx, QRectF());

    // restore world transform
    painter->setWorldTransform(restoreTransform);
}

QRectF GraphicsGlowEffect::boundingRectFor(const QRectF &rect) const
{
    qreal delta = blurRadius() + distance();
    return rect.united(rect.adjusted(-delta - xOffset(), -delta - yOffset(), delta - xOffset(), delta - yOffset()));
}
