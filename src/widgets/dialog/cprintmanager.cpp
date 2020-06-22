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

DWIDGET_USE_NAMESPACE

CPrintManager::CPrintManager(QObject *parent)
    : QObject (parent)
{

}

CPrintManager::~CPrintManager()
{

}

void CPrintManager::showPrintDialog(const QImage &image, DWidget *widget)
{
    m_image = image;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPageSize(QPrinter::A4);
    printer.setPaperSize(QPrinter::Custom);
//    printer.setPaperSize(QSize(m_pixMap.width(), m_pixMap.height()),
//                         QPrinter::DevicePixel);
    //printer.setPageMargins(0., 0., 0., 0., QPrinter::DevicePixel);

    QString desktopDir = QStandardPaths::writableLocation(
                             QStandardPaths::DesktopLocation);
    QString filePath = QString("%1/%2.pdf").arg(desktopDir).arg("DeepIn");

    if (QFileInfo(filePath).exists()) {
        int num = 0;
        while (QFileInfo(filePath).exists()) {
            num++;
            filePath = QString("%1/%2_%3.pdf").arg(desktopDir).arg("DeepIn").arg(num);
        }
    }
    printer.setOutputFileName(filePath);


    QPrintPreviewDialog preview(&printer, widget);
    preview.setFixedSize(1000, 600);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(slotPrintPreview(QPrinter *)));
    preview.exec ();
}


void CPrintManager::slotPrintPreview(QPrinter *printerPixmap)
{
    QPainter painter(printerPixmap);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect wRect  = printerPixmap->pageRect();
    QImage tmpMap;

    if (m_image.width() > wRect.width() || m_image.height() > wRect.height()) {
        tmpMap = m_image.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        tmpMap = m_image;
    }

    QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
                              qreal(wRect.height() - tmpMap.height()) / 2,
                              tmpMap.width(), tmpMap.height());
    painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
                             tmpMap.height()), tmpMap);

}
