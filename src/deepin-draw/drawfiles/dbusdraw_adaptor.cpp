/*
* Copyright (C) 2020 ~ 2021 Deepin Technology Co., Ltd.
*
* Author: Zhang Hao<zhanghao@uniontech.com>
*
* Maintainer: Zhang Hao <zhanghao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include "dbusdraw_adaptor.h"

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QWidget>
#include <QDebug>

dbusdraw_adaptor::dbusdraw_adaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

dbusdraw_adaptor::~dbusdraw_adaptor()
{
    // destructor
}

bool dbusdraw_adaptor::openFiles(QList<QVariant> filePaths)
{
    //将画板窗口最小化，点击启动器中的画板激活窗口
    if (filePaths.isEmpty()) {
        QWidget *pMainWidget = qobject_cast<QWidget *>(parent());
        if (pMainWidget != nullptr) {
            pMainWidget->show();
            pMainWidget->raise();
            pMainWidget->activateWindow();
        }
    }

    QStringList paths;
    foreach (QVariant path, filePaths) {
        paths << path.toString().toLocal8Bit();
    }

    QMetaObject::invokeMethod(parent(), "openFiles",
                              Q_ARG(QStringList, paths), Q_ARG(bool, true));
    return true;
}

bool dbusdraw_adaptor::openFile(QString filePath)
{
    QString _filePath;
    QFileInfo fInfo(filePath);
    if (fInfo.exists() && fInfo.isFile()) {
        _filePath = filePath;
    } else {
        QUrl url(filePath);
        if (url.isLocalFile()) {
            _filePath = url.toLocalFile();
        }
    }

    QWidget *pMainWidget = qobject_cast<QWidget *>(parent());

    if (pMainWidget != nullptr) {
        pMainWidget->show();
        pMainWidget->raise();
        pMainWidget->activateWindow();

        if (!_filePath.isEmpty()) {
            QStringList paths;
            paths.append(_filePath);
            QMetaObject::invokeMethod(parent(), "openFiles",
                                      Q_ARG(QStringList, paths), Q_ARG(bool, true));
            return true;
        }
    }
    return false;
}

