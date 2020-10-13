/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#ifndef CPRINTMANAGER_H
#define CPRINTMANAGER_H

#include <DWidget>

#include <QPixmap>

class QPrinter;

class CPrintManager : public QObject
{
    Q_OBJECT

public:
    explicit CPrintManager(QObject *parent = nullptr);
    ~CPrintManager();

    void showPrintDialog(const QImage &image, QWidget *widget);

//private slots:
//    void slotPrintPreview(QPrinter *printerPixmap);
private:
    QImage m_image;
};

#endif // CPRINTMANAGER_H
