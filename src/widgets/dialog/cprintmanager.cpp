// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cprintmanager.h"

#include <DApplication>
#include <QPrintDialog>
#include <QPrinter>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QDebug>

#include <dprintpreviewdialog.h>

#include "application.h"
#include "cviewmanagement.h"
#include "cgraphicsview.h"
#include "cgraphicsitemselectedmgr.h"

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

void CPrintManager::showPrintDialog(const QImage &image, DWidget *widget, const QString &title)
{
    Q_UNUSED(widget)
    m_image = image;

    //! \modified [bug] https://pms.uniontech.com/bug-view-131383.html
    // 设置父窗口，防止出现焦点切换时顶层窗口异常
    DPrintPreviewDialog printDialog2(drawApp->topMainWindowWidget());

#if (DTK_VERSION_MAJOR > 5 \
    || (DTK_VERSION_MAJOR >=5 && DTK_VERSION_MINOR > 4 ) \
    || (DTK_VERSION_MAJOR >= 5 && DTK_VERSION_MINOR >= 4 && DTK_VERSION_PATCH >= 10))//5.4.4暂时没有合入

    //增加运行时版本判断
    if (DApplication::runtimeDtkVersion() >= DTK_VERSION_CHECK(5, 4, 10, 0)) {
        //添加打印预览默认名称
        //QString docName = drawApp->drawBoard()->currentPage();
        printDialog2.setDocName(title);
    }
#endif


    //connect(&printDialog2, &DPrintPreviewDialog::paintRequested, this, &CPrintManager::slotPaintRequest);
    connect(&printDialog2, QOverload<DPrinter *>::of(&DPrintPreviewDialog::paintRequested), this, &CPrintManager::slotPaintRequest);

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
