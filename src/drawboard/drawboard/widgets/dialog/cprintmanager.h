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
#ifndef CPRINTMANAGER_H
#define CPRINTMANAGER_H
#include <globaldefine.h>

#include <QWidget>
#include <QPixmap>


#ifdef USE_DTK
#include <dprintpreviewdialog.h>
DWIDGET_USE_NAMESPACE
#endif

class QPrinter;

class DRAWLIB_EXPORT CPrintManager : public QObject
{
    Q_OBJECT

public:
    explicit CPrintManager(QObject *parent = nullptr);
    ~CPrintManager();

    void showPrintDialog(const QImage &image, QWidget *widget, const QString &title);

private slots:
#ifdef USE_DTK
    void slotPaintRequest(DPrinter *_printer);
#else
    void slotPaintRequest(QPrinter *_printer);
#endif
private:
    QImage m_image;
};

#endif // CPRINTMANAGER_H
