/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#include "pageview.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QDebug>
#ifdef USE_DTK
#include <DApplication>
#include <dprintpreviewdialog.h>
DWIDGET_USE_NAMESPACE
#endif

CPrintManager::CPrintManager(QObject *parent)
    : QObject(parent)
{

}

CPrintManager::~CPrintManager()
{

}

#ifdef USE_DTK
void CPrintManager::slotPaintRequest(DPrinter *_printer)
#else
void CPrintManager::slotPaintRequest(QPrinter *_printer)
#endif
{
    QPainter painter(_printer);
    QImage img = m_image;

    if (!img.isNull()) {
        //修复打印图片会模糊
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QRect wRect  = _printer->pageRect();
        qreal ratio = wRect.width() * 1.0 / img.width();

        painter.drawImage(QRectF(0, qreal(wRect.height() - img.height() * ratio) / 2,
                                 wRect.width(), img.height() * ratio), img);

    }
    painter.end();
}

void CPrintManager::showPrintDialog(const QImage &image, QWidget *widget, const QString &title)
{
#ifdef USE_DTK
    Q_UNUSED(widget)
    m_image = image;
    DPrintPreviewDialog printDialog2(nullptr);

#if (DTK_VERSION_MAJOR > 5 \
    || (DTK_VERSION_MAJOR >=5 && DTK_VERSION_MINOR > 4 ) \
    || (DTK_VERSION_MAJOR >= 5 && DTK_VERSION_MINOR >= 4 && DTK_VERSION_PATCH >= 10))//5.4.4暂时没有合入

    //增加运行时版本判断
    if (DApplication::runtimeDtkVersion() >= DTK_VERSION_CHECK(5, 4, 10, 0)) {
        //添加打印预览默认名称
        printDialog2.setDocName(title);
    }
#endif
    connect(&printDialog2, QOverload<DPrinter *>::of(&DPrintPreviewDialog::paintRequested), this, &CPrintManager::slotPaintRequest);
    printDialog2.exec();
#endif

}
