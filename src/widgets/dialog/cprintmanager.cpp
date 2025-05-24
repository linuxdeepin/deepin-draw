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
    qDebug() << "Initializing CPrintManager";
}

CPrintManager::~CPrintManager()
{
    qDebug() << "Cleaning up CPrintManager";
}

// TODO: dtk问题，DPrinter在qt6中没有pageRect这个方法,备注下，后续会处理，可能会存在样式问题
void CPrintManager::slotPaintRequest(DPrinter *_printer)
{
    QPainter painter(_printer);
    QImage img = m_image;

    if (!img.isNull()) {
        //修复打印图片会模糊
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
 
#if (QT_VERSION_MAJOR == 5)
        QRect wRect  = _printer->pageRect();
#elif (QT_VERSION_MAJOR == 6)
        QRect wRect = _printer->pageLayout().paintRectPixels(_printer->resolution());
#endif

        qreal ratio = wRect.width() * 1.0 / img.width();

        painter.drawImage(QRectF(0, qreal(wRect.height() - img.height() * ratio) / 2,
                                 wRect.width(), img.height() * ratio), img);
    } else {
        qWarning() << "Null image provided for printing";
    }
    painter.end();
}

void CPrintManager::showPrintDialog(const QImage &image, DWidget *widget, const QString &title)
{
    qDebug() << "Showing print dialog for image - size:" << image.size() << "title:" << title;
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
        qDebug() << "Setting print preview document name:" << title;
        printDialog2.setDocName(title);
    }
#endif

#if (QT_VERSION_MAJOR == 5)
    connect(&printDialog2, QOverload<DPrinter *>::of(&DPrintPreviewDialog::paintRequested), this, &CPrintManager::slotPaintRequest);
#elif (QT_VERSION_MAJOR == 6)
    // TODO 暂时没找到原因，先注释掉，后续处理，先测试是否能够打包成功
    connect(&printDialog2, QOverload<DPrinter *>::of(&DPrintPreviewDialog::paintRequested), this, &CPrintManager::slotPaintRequest);
#endif
    qDebug() << "Executing print preview dialog";
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
