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
#ifndef CMANAGEVIEWSIGLETON_H
#define CMANAGEVIEWSIGLETON_H

#include "globaldefine.h"

#include <QList>
#include <QFileSystemWatcher>
#include <QAtomicInt>
#include <QDebug>
#include <QThread>
#include <QMutex>

//#ifdef USE_DTK
//DWIDGET_USE_NAMESPACE
//#endif

class DRAWLIB_EXPORT SystemFileWatcher:
#ifdef Q_OS_LINUX
    public QThread
#else
    public QFileSystemWatcher
#endif
{
    Q_OBJECT
public:
    enum EFileChangedType {EFileModified, EFileMoved, EFileCount};

    explicit SystemFileWatcher(QObject *parent = nullptr);
    ~SystemFileWatcher();

    bool isVaild();

    void addWather(const QString &path);
    void removePath(const QString &path);

    void clear();

signals:
    void fileChanged(const QString &path, int tp);

#ifdef Q_OS_LINUX
protected:
    void run();

private:
    void doRun();


    int  _handleId = -1;
    bool _running = false;



    QMutex _mutex;
    QMap<int, QString> watchedFilesId;
    QMap<QString, int> watchedFiles;
#else
private:
    QSet<QString> watchedFiles;
#endif


};

#endif // CDRAWPARAMSIGLETON_H
