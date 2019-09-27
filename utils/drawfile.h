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
#ifndef DRAWFILE_H
#define DRAWFILE_H

#include <QObject>
#include "utils/shapesutils.h"

class DrawFile : public QObject
{
    Q_OBJECT
public:
    DrawFile(QObject *parent = 0);
    ~DrawFile();
    void createddf(QSize windowSize, QSize canvasSize,
                   QSize artboardSize, QString path,
                   Toolshapes shapes);
    void parseddf(const QString &path);
    void setItem(QSettings *settings, const QString &group,
                 const QString &key, QVariant val);
    QVariant value(QSettings *settings, const QString &group,
                   const QString &key);
    QStringList groups(QSettings *settings);

    Toolshapes toolshapes();
    QSize windowSize();
    QSize canvasSize();
    QSize artboardSize();

private:
    QSize m_windowSize;
    QSize m_canvasSize;
    QSize m_artboardSize;
    Toolshapes m_allshapes;
};

#endif // DRAWFILE_H
