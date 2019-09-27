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
#include "tempfile.h"

#include <QTemporaryFile>
#include <QDebug>

#include "baseutils.h"

TempFile *TempFile::m_tempFile = nullptr;
TempFile *TempFile::instance()
{
    if (!m_tempFile) {
        m_tempFile = new TempFile();
    }

    return m_tempFile;
}

TempFile::TempFile(QObject *parent)
    : QObject(parent)
{
    QTemporaryFile blurFile;
    if (blurFile.open()) {
        m_blurFile = blurFile.fileName() + ".png";
    }
    qDebug() << "Blur File:" << m_blurFile;
}

TempFile::~TempFile() {}

QString TempFile::getBlurFileName()
{
    if (!m_blurFile.isEmpty()) {
        return m_blurFile;
    } else {
        return "/tmp/deepin-draw-blur.png";
    }
}

QString TempFile::getRandomFile(const QString &filepath,
                                const QString &imageFormat)
{
    QString hashKey = createHash(filepath);
    QTemporaryFile randomFile;
    QString randomFilename;
    if (randomFile.open()) {
        if (imageFormat == ".png" || imageFormat == ".pdf")
            randomFilename = randomFile.fileName() + ".png";
        else
            randomFilename = randomFile.fileName() + imageFormat;
        m_pathMap.insert(hashKey, randomFilename);
    }

    return randomFilename;
}

void TempFile::setImageFile(QList<QPixmap> images)
{
    m_pixmaps = images;
    emit saveDialogPopup();
}

void TempFile::setCanvasShapes(Toolshapes shapes)
{
    m_shapes = shapes;
}

void TempFile::setSaveFinishedExit(bool exit)
{
    m_exit = exit;
}

bool TempFile::saveFinishedExit()
{
    return m_exit;
}

QList<QPixmap> TempFile::savedImage()
{
    return m_pixmaps;
}

Toolshapes TempFile::savedShapes()
{
    return m_shapes;
}
