// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CPRINTMANAGER_H
#define CPRINTMANAGER_H

#include <DWidget>
#include <QPixmap>
#include <dprintpreviewdialog.h>

DWIDGET_USE_NAMESPACE

class QPrinter;

class CPrintManager : public QObject
{
    Q_OBJECT

public:
    explicit CPrintManager(QObject *parent = nullptr);
    ~CPrintManager();

    void showPrintDialog(const QImage &image, QWidget *widget, const QString &title);

private slots:
    void slotPaintRequest(DPrinter *_printer);
private:
    QImage m_image;
};

#endif // CPRINTMANAGER_H
