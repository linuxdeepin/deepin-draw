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
#include "filewatcher.h"

#include <QGuiApplication>
#include <QDebug>
#include <QString>
#include <QFileInfo>

#ifdef Q_OS_LINUX
#include <sys/inotify.h>
#endif

SystemFileWatcher::SystemFileWatcher(QObject *parent)
#ifdef Q_OS_LINUX
    : QThread(parent)
#else
    : QFileSystemWatcher(parent)
#endif
{
#ifdef Q_OS_LINUX
    _handleId = inotify_init();
#endif
}

SystemFileWatcher::~SystemFileWatcher()
{
    clear();
}

bool SystemFileWatcher::isVaild()
{
#ifdef Q_OS_LINUX
    return (_handleId != -1);
#else
    return true;
#endif
}

void SystemFileWatcher::addWather(const QString &path)
{
#ifdef Q_OS_LINUX
    QMutexLocker loker(&_mutex);
    if (!isVaild())
        return;

    QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        return;
    }

    if (watchedFiles.find(path) != watchedFiles.end()) {
        return;
    }

    std::string sfile = path.toStdString();
    int fileId = inotify_add_watch(_handleId, sfile.c_str(), IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF);

    watchedFiles.insert(path, fileId);
    watchedFilesId.insert(fileId, path);

    if (!_running) {
        _running = true;
        start();
    }
#else
    if (addPath(path)) {
        watchedFiles.insert(path);
    }
#endif
}

void SystemFileWatcher::removePath(const QString &path)
{
#ifdef Q_OS_LINUX
    QMutexLocker loker(&_mutex);

    if (!isVaild())
        return;

    auto itf = watchedFiles.find(path);
    if (itf != watchedFiles.end()) {
        inotify_rm_watch(_handleId, itf.value());

        watchedFilesId.remove(itf.value());
        watchedFiles.erase(itf);
    }
#else
    if (QFileSystemWatcher::removePath(path)) {
        watchedFiles.remove(path);
    }
#endif
}

void SystemFileWatcher::clear()
{
#ifdef Q_OS_LINUX
    QMutexLocker loker(&_mutex);

    foreach (auto it, watchedFiles) {
        inotify_rm_watch(_handleId, it);
    }
    watchedFilesId.clear();
    watchedFiles.clear();
#else
    foreach (auto path, watchedFiles) {
        removePath(path);
    }
    watchedFiles.clear();
#endif
}

#ifdef Q_OS_LINUX
void SystemFileWatcher::run()
{
    doRun();
}

void SystemFileWatcher::doRun()
{

    if (!isVaild())
        return;

    char name[1024];
    auto freadsome = [ = ](void *dest, size_t remain, FILE * file) {
        char *offset = reinterpret_cast<char *>(dest);
        while (remain) {
            size_t n = fread(offset, 1, remain, file);
            if (n == 0) {
                return -1;
            }

            remain -= n;
            offset += n;
        }
        return 0;
    };

    FILE *watcher_file = fdopen(_handleId, "r");

    while (true) {
        inotify_event event;
        if (-1 == freadsome(&event, sizeof(event), watcher_file)) {
            qWarning() << "------------- freadsome error !!!!!---------- ";
        }
        if (event.len) {
            freadsome(name, event.len, watcher_file);
        } else {
            QMutexLocker loker(&_mutex);
            auto itf = watchedFilesId.find(event.wd);
            if (itf != watchedFilesId.end()) {
                qDebug() << "file = " << itf.value() << " event.wd = " << event.wd << "event.mask = " << event.mask;

                if (event.mask & IN_MODIFY) {
                    emit fileChanged(itf.value(), EFileModified);
                } else if (event.mask & IN_MOVE_SELF) {
                    emit fileChanged(itf.value(), EFileMoved);
                } else if (event.mask & IN_DELETE_SELF) {
                    emit fileChanged(itf.value(), EFileMoved);
                }
            }
        }
    }

}
#endif

