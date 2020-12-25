/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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
#ifndef BASEUTILS_H
#define BASEUTILS_H

#include <QCursor>
#include <QFont>
#include <QLayout>
#include <QFontMetrics>
#include <QPainter>


//#include <dscreenwindowsutil.h>

//DWM_USE_NAMESPACE


enum CutRation {
    Ration4_3,
    Ration8_5,
    Ration16_9,
    Ration1_1,
    FreeRation,
};


QList<QColor> specifiedColorList();
//QColor       colorIndexOf(int index);
//int                colorIndex(QColor color);

//QSize      initArtboardSize(QPoint pos);
//QSize      getCanvasSize(QSize artboardSize, QSize windowSize);

//QCursor setCursorShape(QString cursorName);
int stringWidth(const QFont &f, const QString &str);
//QString     getFileContent(const QString &file);
//bool          isValidFormat(QString suffix);
//bool          isCommandExist(QString command);
//void  paintSelectedPoint(QPainter &painter, QPointF pos,
//QPixmap pointImg);
//QVariantList cachePixmap(const QString &path);
//QString DetectImageFormat(const QString &filepath);
//QString allImageformat();
//QString     createHash(const QString &str);
//QString sizeToHuman(const qlonglong bytes);
//QCursor blurToolCursor(const int &lineWidth);
QCursor pickColorCursor();


#endif // BASEUTILS_H
