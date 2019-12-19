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
#ifndef IMGUTIL_H
#define IMGUTIL_H

#include "baseutils.h"
#include <QDateTime>
#include <QFileInfo>
#include <QPixmap>

namespace utils {

namespace image {

const int THUMBNAIL_MAX_SIZE = 291;
const int THUMBNAIL_NORMAL_SIZE = 128;

const QPixmap                       cachePixmap(const QString &path);
const QPixmap                       cutSquareImage(const QPixmap &pixmap);
const QPixmap                       cutSquareImage(const QPixmap &pixmap,
                                                   const QSize &size);
const QFileInfoList                 getImagesInfo(const QString &dir,
                                                  bool recursive = true);
const QString                       getOrientation(const QString &path);
const QImage                        getRotatedImage(const QString &path);
bool                                imageSupportRead(const QString &path);
bool                                imageSupportSave(const QString &path);
bool                                imageSupportWrite(const QString &path);
bool                                rotate(const QString &path, int degree);
const QImage                        scaleImage(const QString &path,
                                               const QSize &size = QSize(384, 383));
QStringList  supportedSuffixList();
}  // namespace image

}  // namespace utils

#endif // IMGUTIL_H
