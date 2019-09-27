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
#ifndef TEMPFILE_H
#define TEMPFILE_H

#include <QObject>
#include <QWindow>
#include <QMap>

#include "utils/shapesutils.h"

class TempFile : public QObject
{
    Q_OBJECT
public:
    static TempFile *instance();

    QString getBlurFileName();
    QString getRandomFile(const QString &filepath,
                          const QString &imageFormat = "PNG");

    void setImageFile(QList<QPixmap> images);
    void setCanvasShapes(Toolshapes shapes);
    void setSaveFinishedExit(bool exit);

    bool saveFinishedExit();
    QList<QPixmap> savedImage();
    Toolshapes savedShapes();

signals:
    void saveDialogPopup();

private:
    static TempFile *m_tempFile;
    TempFile(QObject *parent = 0);
    ~TempFile();

    QString m_blurFile;
    QMap<QString, QString> m_pathMap;
    QList<QPixmap> m_pixmaps;
    Toolshapes m_shapes;
    bool m_exit = false;
};
#endif // TEMPFILE_H
