#include "baseutils.h"

#include <QPixmap>
#include <QProcess>
#include <QLayoutItem>
#include <QFile>
#include <QDebug>
#include <QImageReader>
#include <QCryptographicHash>

QCursor setCursorShape(QString cursorName) {
    QCursor customShape = QCursor();
    if (cursorName == "rotate") {
        customShape = QCursor(QPixmap(
                      ":/theme/light/images/mouse_style/shape/rotate_mouse.png"), 5, 5);
    } else if (cursorName == "rectangle") {
        qDebug() << "rectangle ...";
        customShape = QCursor(QPixmap(
                      ":/theme/light/images/mouse_style/shape/rectangle_mouse.png"), 0, 4);
    } else if (cursorName == "oval") {
        customShape = QCursor(QPixmap(
                      ":/theme/light/images/mouse_style/shape/ellipse_mouse.png"), 0, 4);
    } else if (cursorName == "arrow") {
        customShape = QCursor(QPixmap(
                      ":/theme/light/images/mouse_style/shape/arrow_mouse.png"), 5, 5);
    } else if (cursorName == "text") {
        customShape = QCursor(QPixmap(
                      ":/theme/light/images/mouse_style/shape/text_mouse.png"), 5, 5);
    } else if (cursorName == "straightLine") {
        customShape = QCursor(QPixmap(QString(
                    ":/theme/light/images/mouse_style/shape/line_mouse.png")), 2, 9);
    } else if (cursorName == "arbitraryCurve") {
        customShape = QCursor(QPixmap(
                    ":/theme/light/images/mouse_style/shape/pen_mouse.png"), 2, 9);
    } else if (cursorName == "pickcolor") {
        customShape = QCursor(QPixmap(
                     ":/theme/light/images/mouse_style/shape/pen_mouse.png"), 2, 9);
    }

    return customShape;
}

int stringWidth(const QFont &f, const QString &str)
{
    QFontMetrics fm(f);
    return fm.boundingRect(str).width();
}

QString getFileContent(const QString &file) {
    QFile f(file);
    QString fileContent = "";
    if (f.open(QFile::ReadOnly))
    {
        fileContent = QLatin1String(f.readAll());
        f.close();
    }
    return fileContent;
}

QColor colorIndexOf(int index) {
    switch(index) {
    case 0: { return QColor("#ffd903");}
    case 1: { return QColor("#ff5e1a");}
    case 2: { return QColor("#ff3305");}
    case 3: { return QColor("#ff1c49");}
    case 4: { return QColor("#fb00ff");}
    case 5: { return QColor("#7700ed");}
    case 6: { return QColor("#3d08ff");}
    case 7: { return QColor("#3467ff");}
    case 8: { return QColor("#00aaff");}
    case 9: { return QColor("#08ff77");}
    case 10: { return QColor("#03a60e");}
    case 11: { return QColor("#3c7d00");}
    case 12: { return QColor("#ffffff");}
    case 13: { return QColor("#666666");}
    case 14: { return QColor("#2b2b2b");}
    case 15: { return QColor("#000000");}
    default:  { return QColor("#ffd903");}
    }

    return QColor("#ffd903");
}

int colorIndex(QColor color) {
    QList<QColor> colorList;
    colorList.append(QColor("#ffd903"));
    colorList.append(QColor("#ff5e1a"));
    colorList.append(QColor("#ff3305"));
    colorList.append(QColor("#ff1c49"));
    colorList.append(QColor("#fb00ff"));
    colorList.append(QColor("#7700ed"));
    colorList.append(QColor("#3d08ff"));
    colorList.append(QColor("#3467ff"));
    colorList.append(QColor("#00aaff"));
    colorList.append(QColor("#08ff77"));
    colorList.append(QColor("#03a60e"));
    colorList.append(QColor("#3c7d00"));
    colorList.append(QColor("#ffffff"));
    colorList.append(QColor("#666666"));
    colorList.append(QColor("#2b2b2b"));
    colorList.append(QColor("#000000"));
    return colorList.indexOf(color);
}

bool          isValidFormat(QString suffix) {
    QStringList validFormat;
    validFormat << "bmp" << "jpg" << "jpeg" << "png" << "pbm" << "pgm" << "xbm" << "xpm";
    if (validFormat.contains(suffix)) {
        return true;
    } else {
        return false;
    }
}

bool          isCommandExist(QString command) {
    QProcess* proc = new QProcess;
    QString cm = QString("which %1\n").arg(command);
    proc->start(cm);
    proc->waitForFinished(1000);

    if (proc->exitCode() == 0) {
        return true;
    } else {
        return false;
    }
}

void   paintSelectedPoint(QPainter &painter, QPointF pos, QPixmap pointImg) {
    painter.drawPixmap(pos, pointImg);
}

QVariantList cachePixmap(const QString &path)
{
    QImage tImg;

    QString format = DetectImageFormat(path);
    if (format.isEmpty()) {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        if (reader.canRead()) {
            tImg = reader.read();
        }
    } else {
        QImageReader readerF(path, format.toLatin1());
        readerF.setAutoTransform(true);
        if (readerF.canRead()) {
            tImg = readerF.read();
        }
    }

    QPixmap p = QPixmap::fromImage(tImg);
    QVariantList vl;
    vl << QVariant(path) << QVariant(p);
    return vl;
}

// For more information about image file extension, see:
// https://en.wikipedia.org/wiki/Image_file_formats
QString DetectImageFormat(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "DetectImageFormat() failed to open file:" << filepath;
        return "";
    }

    const QByteArray data = file.read(1024);

    // Check bmp file.
    if (data.startsWith("BM")) {
        return "bmp";
    }

    // Check dds file.
    if (data.startsWith("DDS")) {
        return "dds";
    }

    // Check gif file.
    if (data.startsWith("GIF8")) {
        return "gif";
    }

    // Check Max OS icons file.
    if (data.startsWith("icns")) {
        return "icns";
    }

    // Check jpeg file.
    if (data.startsWith("\xff\xd8")) {
        return "jpg";
    }

    // Check mng file.
    if (data.startsWith("\x8a\x4d\x4e\x47\x0d\x0a\x1a\x0a")) {
        return "mng";
    }

    // Check net pbm file (BitMap).
    if (data.startsWith("P1") || data.startsWith("P4")) {
        return "pbm";
    }

    // Check pgm file (GrayMap).
    if (data.startsWith("P2") || data.startsWith("P5")) {
        return "pgm";
    }

    // Check ppm file (PixMap).
    if (data.startsWith("P3") || data.startsWith("P6")) {
        return "ppm";
    }

    // Check png file.
    if (data.startsWith("\x89PNG\x0d\x0a\x1a\x0a")) {
        return "png";
    }

    // Check svg file.
    if (data.indexOf("<svg") > -1) {
        return "svg";
    }

    // TODO(xushaohua): tga file is not supported yet.

    // Check tiff file.
    if (data.startsWith("MM\x00\x2a") || data.startsWith("II\x2a\x00")) {
        // big-endian, little-endian.
        return "tiff";
    }

    // TODO(xushaohua): Support wbmp file.

    // Check webp file.
    if (data.startsWith("RIFFr\x00\x00\x00WEBPVP")) {
        return "webp";
    }

    // Check xbm file.
    if (data.indexOf("#define max_width ") > -1 &&
        data.indexOf("#define max_height ") > -1) {
        return "xbm";
    }

    // Check xpm file.
    if (data.startsWith("/* XPM */")) {
        return "xpm";
    }


    return "";
}

QString allImageformat()
{
    return QString("Files(*.bmp *.bmp24 *.ico *.jpg *.jpe *.jpeg *.jpeg24 *.jng *.pcd"
                   "*.pcx *.png *.tga *.tif *.tiff *.tiff24 *.psd *.xpm *.dds *.gif *.sgi *.j2k "
                   "*jp2 *.pct *.webp *.wdp *.cr2 *.pef *.arw *.nef *.icb *.dng *.vda "
                   "*.vst *.raf *.orf *.svg *.ptif *.mef *.mrw *.xbm);;");
}

QString createHash(const QString &str)
{
    return QString(QCryptographicHash::hash(str.toUtf8(),
                                            QCryptographicHash::Md5).toHex());
}
