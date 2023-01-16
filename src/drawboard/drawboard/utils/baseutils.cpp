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
#include "baseutils.h"

#include <QPixmap>
#include <QProcess>
#include <QLayoutItem>
#include <QFile>
#include <QDebug>
#include <QImageReader>
#include <QCryptographicHash>
#include <QtMath>
#include <QApplication>
#include <QDesktopWidget>


//QSize      initArtboardSize(QPoint pos)
//{
//    int artboardActualWidth = 0, artboardActualHeight = 0;
////    QSize desktopSize = DScreenWindowsUtil::instance(pos
////                                                    )->primaryScreen()->size();
//    QSize desktopSize = QApplication::desktop()->screen()->size();

//    qDebug() << "init artboardSize:" << pos <<  desktopSize;
//    artboardActualWidth = desktopSize.width();
//    artboardActualHeight = desktopSize.height();

//    return QSize(artboardActualWidth, artboardActualHeight);
//}

//QSize      getCanvasSize(QSize artboardSize, QSize windowSize)
//{
//    qreal winWidth = qreal(windowSize.width()),
//          winHeight = qreal(windowSize.height());
//    qreal widthRation = qreal(artboardSize.width()) / qreal(windowSize.width());
//    qreal heightRation = qreal(artboardSize.height()) / qreal(windowSize.height());
//    qDebug() << "GetCanvasSize:" << widthRation << heightRation;

//    if (widthRation > heightRation) {
//        winHeight = qreal(artboardSize.height()) / widthRation;
//    } else {
//        winWidth = qreal(artboardSize.width()) / heightRation;
//    }

//    return QSize(int(winWidth), int(winHeight));
//}

//QCursor setCursorShape(QString cursorName)
//{
//    QCursor customShape = QCursor();
//    if (cursorName == "rotate") {
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/rotate_mouse.png"), 5, 5);
//    } else if (cursorName == "rectangle") {
//        qDebug() << "rectangle ...";
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/mouse_rectangle.svg"), 7, 17);
//    } else if (cursorName == "oval") {
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/mouse_oval.svg"), 7, 17);
//    } else if (cursorName == "arrow") {
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/arrow_mouse.png"), 5, 5);
//    } else if (cursorName == "text") {
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/text_mouse.png"), 5, 5);
//    } else if (cursorName == "straightLine") {
//        customShape = QCursor(QPixmap(QString(
//                                          ":/theme/light/images/mouse_style/shape/line_mouse.png")), 2, 9);
//    } else if (cursorName == "arbitraryCurve") {
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/pen_mouse.png"), 4, 13);
//    } else if (cursorName == "pickcolor") {
//        customShape = QCursor(QPixmap(
//                                  ":/theme/light/images/mouse_style/shape/pen_mouse.png"), 2, 9);
//    } else if (cursorName == "bottomleft" || cursorName == "bottomright"
//               || cursorName == "topleft" || cursorName == "topright") {
//        customShape = QCursor(QPixmap(
//                                  QString(":/theme/light/images/mouse_style/shape/%1_mouse.svg").arg(cursorName)));
//    }

//    return customShape;
//}

//int stringWidth(const QFont &f, const QString &str)
//{
//    QFontMetrics fm(f);
//    return fm.boundingRect(str).width();
//}

//QString getFileContent(const QString &file)
//{
//    QFile f(file);
//    QString fileContent = "";
//    if (f.open(QFile::ReadOnly)) {
//        fileContent = QLatin1String(f.readAll());
//        f.close();
//    }
//    return fileContent;
//}

QList<QColor> specifiedColorList()
{
    QList<QColor> colorList;
    QColor colorTrasparent(Qt::white);
    colorTrasparent.setAlpha(0);

    colorList
            << colorTrasparent  << QColor("#CCC000")  << QColor("#CC0033") << QColor("#CC0066") << QColor("#CC3300") << QColor("#FF7F50") << QColor("#FFC125") << QColor("#FFF68F") << QColor("#CCFF99")
            << QColor("#66ff00") << QColor("#99FF00") << QColor("#99FF99") << QColor("#99FFFF") << QColor("#6699FF") << QColor("#3333FF") << QColor("#6666FF") << QColor("#9933FF") << QColor("#CC33FF")
            << QColor("#FFFFFF") << QColor("#EAEAEA") << QColor("#E3E3E3") << QColor("#C9C9C9") << QColor("#ADADAD") << QColor("#6F6F6F") << QColor("#404040") << QColor("#1B1B1B") << QColor("#000000");

    return colorList;
}

QList<QColor> specifiedColorList2()
{
    QList<QColor> colorList;
    QColor colorTrasparent(Qt::white);
    colorTrasparent.setAlpha(0);
    colorList  << QColor("#ff0c0c") << QColor("#4EC918") << QColor("#0292EA")  << QColor("#FF7F50") << QColor("#fcfd0e") << QColor("#FFFFFF")  << QColor("#000000");

    return colorList;
}

//QColor colorIndexOf(int index)
//{
//    QList<QColor> colorList = specifiedColorList();

//    if (index < colorList.length())
//        return colorList[index];

//    return colorList[0];
//}

//int colorIndex(QColor color)
//{
//    QList<QColor> colorList = specifiedColorList();
//    if (colorList.contains(color))
//        return colorList.indexOf(color);

//    return 0;
//}

//bool          isValidFormat(QString suffix)
//{
//    QStringList validFormat;
//    validFormat << "bmp" << "jpg" << "jpeg" << "png" << "pbm"
//                << "pgm" << "xbm" << "xpm";
//    if (validFormat.contains(suffix)) {
//        return true;
//    } else {
//        return false;
//    }
//}

//bool          isCommandExist(QString command)
//{
//    QProcess *proc = new QProcess;
//    QString cm = QString("which %1\n").arg(command);
//    proc->start(cm);
//    proc->waitForFinished(1000);

//    if (proc->exitCode() == 0) {
//        return true;
//    } else {
//        return false;
//    }
//}

//void   paintSelectedPoint(QPainter &painter, QPointF pos, QPixmap pointImg)
//{
//    painter.drawPixmap(pos, pointImg);
//}

//QVariantList cachePixmap(const QString &path)
//{
//    QImage tImg;

//    QString format = DetectImageFormat(path);
//    if (format.isEmpty()) {
//        QImageReader reader(path);
//        reader.setAutoTransform(true);
//        if (reader.canRead()) {
//            tImg = reader.read();
//        }
//    } else {
//        QImageReader readerF(path, format.toLatin1());
//        readerF.setAutoTransform(true);
//        if (readerF.canRead()) {
//            tImg = readerF.read();
//        }
//    }

//    QPixmap p = QPixmap::fromImage(tImg);
//    QVariantList vl;
//    vl << QVariant(path) << QVariant(p);
//    return vl;
//}

// For more information about image file extension, see:
// https://en.wikipedia.org/wiki/Image_file_formats
//QString DetectImageFormat(const QString &filepath)
//{
//    QFile file(filepath);
//    if (!file.open(QIODevice::ReadOnly)) {
//        qWarning() << "DetectImageFormat() failed to open file:" << filepath;
//        return "";
//    }

//    const QByteArray data = file.read(1024);

//    // Check bmp file.
//    if (data.startsWith("BM")) {
//        return "bmp";
//    }

//    // Check dds file.
//    if (data.startsWith("DDS")) {
//        return "dds";
//    }

//    // Check gif file.
//    if (data.startsWith("GIF8")) {
//        return "gif";
//    }

//    // Check Max OS icons file.
//    if (data.startsWith("icns")) {
//        return "icns";
//    }

//    // Check jpeg file.
//    if (data.startsWith("\xff\xd8")) {
//        return "jpg";
//    }

//    // Check mng file.
//    if (data.startsWith("\x8a\x4d\x4e\x47\x0d\x0a\x1a\x0a")) {
//        return "mng";
//    }

//    // Check net pbm file (BitMap).
//    if (data.startsWith("P1") || data.startsWith("P4")) {
//        return "pbm";
//    }

//    // Check pgm file (GrayMap).
//    if (data.startsWith("P2") || data.startsWith("P5")) {
//        return "pgm";
//    }

//    // Check ppm file (PixMap).
//    if (data.startsWith("P3") || data.startsWith("P6")) {
//        return "ppm";
//    }

//    // Check png file.
//    if (data.startsWith("\x89PNG\x0d\x0a\x1a\x0a")) {
//        return "png";
//    }

//    // Check svg file.
//    if (data.indexOf("<svg") > -1) {
//        return "svg";
//    }

//    // TODO(xushaohua): tga file is not supported yet.

//    // Check tiff file.
//    if (data.startsWith("MM\x00\x2a") || data.startsWith("II\x2a\x00")) {
//        // big-endian, little-endian.
//        return "tiff";
//    }

//    // TODO(xushaohua): Support wbmp file.

//    // Check webp file.
//    if (data.startsWith("RIFFr\x00\x00\x00WEBPVP")) {
//        return "webp";
//    }

//    // Check xbm file.
//    if (data.indexOf("#define max_width ") > -1 &&
//            data.indexOf("#define max_height ") > -1) {
//        return "xbm";
//    }

//    // Check xpm file.
//    if (data.startsWith("/* XPM */")) {
//        return "xpm";
//    }

//    return "";
//}

//QString allImageformat()
//{
//    return QString("Files(*.bmp *.bmp24 *.ico *.jpg *.jpe *.jpeg *.jpeg24 *.jng *.pcd"
//                   "*.pcx *.png *.tga *.tif *.tiff *.tiff24 *.psd *.xpm *.dds *.gif *.sgi *.j2k "
//                   "*jp2 *.pct *.webp *.wdp *.cr2 *.pef *.arw *.nef *.icb *.dng *.vda "
//                   "*.vst *.raf *.orf *.svg *.ptif *.mef *.mrw *.xbm);;");
//}

//QString createHash(const QString &str)
//{
//    return QString(QCryptographicHash::hash(str.toUtf8(),
//                                            QCryptographicHash::Md5).toHex());
//}

//QString sizeToHuman(const qlonglong bytes)
//{
//    qlonglong sb = 1024;
//    if (bytes < sb) {
//        return QString::number(bytes) + " B";
//    } else if (bytes < sb * sb) {
//        QString vs = QString::number(qreal(bytes) / sb, 'f', 1);
//        if (qCeil(vs.toDouble()) == qFloor(vs.toDouble())) {
//            return QString::number(int(vs.toDouble())) + " KB";
//        } else {
//            return vs + " KB";
//        }
//    } else if (bytes < sb * sb * sb) {
//        QString vs = QString::number(int(bytes) / sb / sb, 'f', 1);
//        if (qCeil(vs.toDouble()) == qFloor(vs.toDouble())) {
//            return QString::number(int(vs.toDouble())) + " MB";
//        } else {
//            return vs + " MB";
//        }
//    } else {
//        return QString::number(bytes);
//    }
//}

//QCursor blurToolCursor(const int &lineWidth)
//{
//    QPixmap cursorPix = QPixmap(QSize(lineWidth + 2, lineWidth + 2));
//    cursorPix.fill(QColor(Qt::transparent));
//    QPen borderPen;
//    borderPen.setWidth(3);
//    borderPen.setColor(QColor(0, 0, 0, 26));
//    QPainter painter(&cursorPix);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
//    painter.setBrush(QBrush());
//    painter.setPen(borderPen);
//    painter.drawEllipse(cursorPix.rect().center(), lineWidth / 2 - 1, lineWidth / 2 - 1);

//    QPen pen;
//    pen.setWidth(1);
//    pen.setColor(QColor(Qt::white));
//    painter.setPen(pen);
//    painter.drawEllipse(cursorPix.rect().center(), lineWidth / 2 - 1, lineWidth / 2 - 1);

//    return QCursor(cursorPix, -1, -1);
//}

QCursor pickColorCursor()
{
    int tipWidth = 15;
    QPixmap cursorPix = QPixmap(QSize(tipWidth, tipWidth));
    cursorPix.fill(QColor(Qt::transparent));

    QPen whitePen;
    whitePen.setWidth(1);
    whitePen.setCapStyle(Qt::FlatCap);
    whitePen.setJoinStyle(Qt::RoundJoin);
    whitePen.setColor(QColor(255, 255, 255, 255));

    QPen blackPen;
    blackPen.setWidth(1);
    blackPen.setCapStyle(Qt::FlatCap);
    blackPen.setJoinStyle(Qt::RoundJoin);
    blackPen.setColor(QColor(0, 0, 0, 125));

    QPainter painter(&cursorPix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(blackPen);
    painter.drawEllipse(cursorPix.rect().center(), tipWidth / 2 - 1, tipWidth / 2 - 1);
    painter.drawEllipse(cursorPix.rect().center(), tipWidth / 2 - 3, tipWidth / 2 - 3);
    painter.setPen(whitePen);
    painter.drawEllipse(cursorPix.rect().center(), tipWidth / 2 - 2, tipWidth / 2 - 2);

    return QCursor(cursorPix, -1, -1);
}

