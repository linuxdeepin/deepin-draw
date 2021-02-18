/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "cprintmanager.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QDebug>

#include <dprintpreviewdialog.h>

DWIDGET_USE_NAMESPACE

CPrintManager::CPrintManager(QObject *parent)
    : QObject(parent)
{

}

CPrintManager::~CPrintManager()
{

}

void CPrintManager::slotPaintRequest(DPrinter *_printer)
{
    QPainter painter(_printer);
    QImage img = m_image;

    if (!img.isNull()) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QRect wRect  = _printer->pageRect();
        QImage tmpMap;

        if (img.width() > wRect.width() || img.height() > wRect.height()) {
            tmpMap = img.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            tmpMap = img;
        }

        QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
                                  qreal(wRect.height() - tmpMap.height()) / 2,
                                  tmpMap.width(), tmpMap.height());
        painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
                                 tmpMap.height()), tmpMap);
    }
    painter.end();

}

void CPrintManager::showPrintDialog(const QImage &image, DWidget *widget)
{
    Q_UNUSED(widget)
    m_image = image;
    DPrintPreviewDialog printDialog2(nullptr);

    connect(&printDialog2, SIGNAL(paintRequested(DPrinter *)),
            this, SLOT(slotPaintRequest(DPrinter *)));

    //printDialog2.setFixedSize(1000, 600);
    printDialog2.exec();

}


//void CPrintManager::slotPrintPreview(QPrinter *printerPixmap)
//{
//    QPainter painter(printerPixmap);
//    painter.setRenderHints(QPainter::Antialiasing);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);

//    QRect wRect  = printerPixmap->pageRect();
//    QImage tmpMap;

//    if (m_image.width() > wRect.width() || m_image.height() > wRect.height()) {
//        tmpMap = m_image.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    } else {
//        tmpMap = m_image;
//    }

//    QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
//                              qreal(wRect.height() - tmpMap.height()) / 2,
//                              tmpMap.width(), tmpMap.height());
//    painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
//                             tmpMap.height()), tmpMap);

//}
