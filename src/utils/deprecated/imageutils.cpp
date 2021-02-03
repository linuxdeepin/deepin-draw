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
#include "utils/baseutils.h"
#include "utils/imageutils.h"
#include "utils/imageutils_libexif.h"
#include "utils/imageutils_freeimage.h"
#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QMimeDatabase>
#include <QMutexLocker>
#include <QPixmapCache>
#include <QProcess>
#include <QReadWriteLock>
#include <QSvgRenderer>
#include <QUrl>

namespace utils {

namespace image {

bool imageSupportRead(const QString &path)
{
    const QString suffix = QFileInfo(path).suffix();

    //FIXME: ????????????freeimage?load??????????
    QStringList errorList;
    errorList << "X3F";

    if (errorList.indexOf(suffix.toUpper()) != -1) {
        return false;
    }

    if (freeimage::isSupportsReading(path))
        return true;
    else {
        return (suffix == "svg");
    }
}

bool imageSupportSave(const QString &path)
{
    const QString suffix = QFileInfo(path).suffix();

    // RAW image decode is tool slow, and mose of these was not support save
    // RAW formats render incorrect by freeimage
    const QStringList raws = QStringList()
                             << "CR2" << "CRW"   // Canon cameras
                             << "DCR" << "KDC"   // Kodak cameras
                             << "MRW"            // Minolta cameras
                             << "NEF"            // Nikon cameras
                             << "ORF"            // Olympus cameras
                             << "PEF"            // Pentax cameras
                             << "RAF"            // Fuji cameras
                             << "SRF"            // Sony cameras
                             << "X3F";           // Sigma cameras


    if (raws.indexOf(suffix.toUpper()) != -1
            || QImageReader(path).imageCount() > 1) {
        return false;
    } else {
        return freeimage::canSave(path);
    }
}

bool imageSupportWrite(const QString &path)
{
    return freeimage::isSupportsWriting(path);
}

bool rotate(const QString &path, int degree)
{
    if (degree % 90 != 0)
        return false;

    int loadFlags = 0;
    int saveFlags = 0;
    FREE_IMAGE_FORMAT fif = freeimage::fFormat(path);
    switch (int(fif)) {
    case FIF_JPEG:
        loadFlags = JPEG_ACCURATE;          // Load the file with the best quality, sacrificing some speed
        saveFlags = JPEG_QUALITYSUPERB;     // Saves with superb quality (100:1)
        break;
    case FIF_JP2:
        // Freeimage3.17 not support set special load flags for JP2
        saveFlags = JP2_DEFAULT;            // Save with a 16:1 rate
        break;
    case FIF_BMP:
        saveFlags = BMP_DEFAULT;            // Save without any compression
        break;
    case FIF_EXR:
        saveFlags = EXR_NONE;               // Save with no compression
        break;
    case FIF_PNG:
        saveFlags = PNG_DEFAULT;   // Save without ZLib compression
        break;
    }

    FIBITMAP *dib = freeimage::readFileToFIBITMAP(path, loadFlags);
    FIBITMAP *rotated = FreeImage_Rotate(dib, -degree);
    if (rotated) {
        // Regenerate thumbnail if it's exits
        // Image formats that currently support thumbnail saving are
        // JPEG (JFIF formats), EXR, TGA and TIFF.
        if (FreeImage_GetThumbnail(dib)) {
            FIBITMAP *thumb = FreeImage_GetThumbnail(dib);
            FIBITMAP *rotateThumb = FreeImage_Rotate(thumb, -degree);
            FreeImage_SetThumbnail(rotated, rotateThumb);
            FreeImage_Unload(rotateThumb);
        }
    }

    bool v = freeimage::writeFIBITMAPToFile(rotated, path, saveFlags);
    FreeImage_Unload(dib);
    FreeImage_Unload(rotated);

    return v;
}

/*!
 * \brief cutSquareImage
 * Cut square image
 * \param pixmap
 * \return
 */
const QPixmap cutSquareImage(const QPixmap &pixmap)
{
    const int minL = qMin(pixmap.width(), pixmap.height());
    QImage img = pixmap.toImage();
    img = img.copy((img.width() - minL) / 2,
                   (img.height() - minL) / 2,
                   minL, minL);
    return QPixmap::fromImage(img);
}

/*!
 * \brief cutSquareImage
 * Scale and cut a square image
 * \param pixmap
 * \param size
 * \return
 */
const QPixmap cutSquareImage(const QPixmap &pixmap, const QSize &size)
{
    QImage img = pixmap.toImage().scaled(size,
                                         Qt::KeepAspectRatioByExpanding,
                                         Qt::SmoothTransformation);

    img = img.copy((img.width() - size.width()) / 2,
                   (img.height() - size.height()) / 2,
                   size.width(), size.height());
    return QPixmap::fromImage(img);
}

/*!
 * \brief getImagesInfo
        types<< ".BMP";
        types<< ".GIF";
        types<< ".JPG";
        types<< ".JPEG";
        types<< ".PNG";
        types<< ".PBM";
        types<< ".PGM";
        types<< ".PPM";
        types<< ".XBM";
        types<< ".XPM";
        types<< ".SVG";

        types<< ".DDS";
        types<< ".ICNS";
        types<< ".JP2";
        types<< ".MNG";
        types<< ".TGA";
        types<< ".TIFF";
        types<< ".WBMP";
        types<< ".WEBP";
 * \param dir
 * \param recursive
 * \return
 */
const QFileInfoList getImagesInfo(const QString &dir, bool recursive)
{
    QFileInfoList infos;

    if (! recursive) {
        auto nsl = QDir(dir).entryInfoList(QDir::Files);
        for (QFileInfo info : nsl) {
            if (imageSupportRead(info.absoluteFilePath())) {
                infos << info;
            }
        }
        return infos;
    }

    QDirIterator dirIterator(dir,
                             QDir::Files,
                             QDirIterator::Subdirectories);
    while (dirIterator.hasNext()) {
        dirIterator.next();
        if (imageSupportRead(dirIterator.fileInfo().absoluteFilePath())) {
            infos << dirIterator.fileInfo();
        }
    }

    return infos;
}

const QString getOrientation(const QString &path)
{
    return freeimage::getOrientation(path);
}

/*!
 * \brief getRotatedImage
 * Rotate image base on the exif orientation
 * \param path
 * \return
 */
const QImage getRotatedImage(const QString &path)
{
    QImage tImg;
    QImageReader reader(path);
    reader.setAutoTransform(true);
    if (reader.canRead()) {
        tImg = reader.read();
    }
    return tImg;
}

//const QMap<QString, QString> getAllMetaData(const QString &path)
//{
//    return freeimage::getAllMetaData(path);
//}

const QPixmap cachePixmap(const QString &path)
{
    QPixmap pp;
    if (! QPixmapCache::find(path, &pp)) {
        pp = QPixmap(path);
        QPixmapCache::insert(path, pp);
    }
    return pp;
}

const QString toMd5(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

QStringList  supportedSuffixList()
{
    QStringList suffixList;
    suffixList << "*.bmp" << "*.bmp24"
               << "*.ico" << "*.jpg" << "*.jpe" << "*.jpeg" << "*.jpeg24"
               << "*.jng"  << "*.pcd " << "*.pcx "
               << "*.png" << "*.tga" << "*.tif " <<  "*.tiff" << "*.tiff24"
               << "*.psd" << "*.xpm" << "*.dds" << "*.gif" << "*.sgi"
               << "*.j2k" << "*jp2" << "*.pct" << "*.webp" << "*.wdp"
               << "*.cr2" << "*.pef" << "*.arw " << "*.nef" << "*.icb " << "*.dng"
               << "*.vda" << "*.vst" << "*.raf" << "*.orf" << "*.svg"
               << "*.ptif" << "*.mef" << "*.mrw" << "*.xbm";
    return suffixList;
}
}  // namespace image

}  //namespace utils
