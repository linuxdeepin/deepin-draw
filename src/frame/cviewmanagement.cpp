// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "drawshape/cdrawparamsigleton.h"
#include "ddialog.h"
#include "frame/ccentralwidget.h"
#include "bzItems/cgraphicstextitem.h"
#include "widgets/ctextedit.h"
#include "application.h"

#include <DApplication>
#include <QGuiApplication>
#include <QDebug>
#include <QString>
#include <QFileInfo>
#include <DDialog>

#include <sys/inotify.h>

DWIDGET_USE_NAMESPACE

CManageViewSigleton *CManageViewSigleton::m_pInstance = nullptr;

CManageViewSigleton::CManageViewSigleton()
    : QObject()
{
    qDebug() << "Initializing CManageViewSigleton";
    Q_UNUSED(getFileSrcData(""));
}

CManageViewSigleton *CManageViewSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        qDebug() << "Creating new CManageViewSigleton instance";
        m_pInstance  = new CManageViewSigleton();
    }
    return m_pInstance;
}

bool CManageViewSigleton::isEmpty()
{
    qDebug() << "Checking if view list is empty - count:" << m_allViews.size();
    return m_allViews.isEmpty();
}

PageView *CManageViewSigleton::getCurView()
{
    PageView *curView = nullptr;

    //获取当前窗口
    if (!m_allViews.isEmpty() && m_curIndex >= 0 && m_curIndex < m_allViews.size()) {
        curView = m_allViews[m_curIndex];
        qDebug() << "Getting current view - index:" << m_curIndex;
    } else {
        qDebug() << "No current view available";
    }

    return curView;
}

PageScene *CManageViewSigleton::getCurScene()
{
    auto view = getCurView();
    if (view != nullptr) {
        qDebug() << "Getting current scene from view";
        return view->drawScene();
    }
    qDebug() << "No current scene available";
    return nullptr;
}

void CManageViewSigleton::addView(PageView *view)
{
    if (nullptr == view) {
        qWarning() << "Attempted to add null view";
        return;
    }
    if (!m_allViews.contains(view)) {
        qDebug() << "Adding new view to management";
        m_allViews.append(view);
        //emit viewAdded(view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
        qDebug() << "Setting initial current view index to 0";
    }
}

void CManageViewSigleton::setCurView(PageView *view)
{
    if (nullptr == view) {
        qWarning() << "Attempted to set null view as current";
        return;
    }
    auto currentView = getCurView();
    if (currentView == view) {
        qDebug() << "View is already current view";
        return;
    }

    if (m_allViews.contains(view)) {
        m_curIndex = m_allViews.indexOf(view);
        qDebug() << "Setting current view - new index:" << m_curIndex;
        //emit viewChanged(currentView, view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
        qDebug() << "Setting initial current view index to 0";
    }
}

void CManageViewSigleton::removeView(PageView *view)
{
    if (nullptr == view) {
        qWarning() << "Attempted to remove null view";
        return;
    }
    if (m_allViews.contains(view)) {
        auto curIndex = m_allViews.indexOf(view);
        qDebug() << "Removing view - index:" << curIndex;
        m_allViews.removeAt(curIndex);

        QString ddfPath = /*view->getDrawParam()->getDdfSavePath()*/"";
        //removeWacthedFile(ddfPath);

        if (curIndex == m_curIndex) {
            if (m_allViews.isEmpty()) {
                m_curIndex = -1;
                qDebug() << "No views remaining after removal";
                //emit viewChanged(view, nullptr);
            } else {
                m_curIndex = 0;
                qDebug() << "Setting current view index to 0 after removal";
                //emit viewChanged(view, m_allViews[m_curIndex]);
            }
        }
        //emit viewRemoved(view);
    }
}

//void CManageViewSigleton::updateBlockSystem()
//{
//    initBlockShutdown();
//    bool shutdownFlag = false;
//    for (auto view : m_allViews) {
//        if (view->isModified()) {
//            shutdownFlag = true;
//            break;
//        }
//    }
//    if (shutdownFlag) {
//        m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
//    } else {
//        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
//        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
//        //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
//        qDebug() << "Nublock shutdown.";
//    }
//}

//CGraphicsView *CManageViewSigleton::getViewByViewName(QString name)
//{
//    for (int i = 0; i < m_allViews.count(); i++) {
//        if (m_allViews[i]->getDrawParam()->viewName() == name) {
//            return m_allViews[i];
//        }
//    }

//    // 返回空指针
//    return nullptr;
//}

//CGraphicsView *CManageViewSigleton::getViewByViewModifyStateName(QString name)
//{
//    for (int i = 0; i < m_allViews.count(); i++) {
//        if (m_allViews[i]->getDrawParam()->getShowViewNameByModifyState() == name) {
//            return m_allViews[i];
//        }
//    }

//    // 返回空指针
//    return nullptr;
//}

int CManageViewSigleton::viewCount()
{
    qDebug() << "Getting view count:" << m_allViews.size();
    return m_allViews.size();
}

void CManageViewSigleton::initBlockShutdown()
{
    if (!m_arg.isEmpty() || m_reply.value().isValid()) {
        qDebug() << "Shutdown blocking already initialized - reply valid:" << m_reply.value().isValid();
        return;
    }

    qDebug() << "Initializing shutdown blocking";
    m_pLoginManager = new QDBusInterface("org.freedesktop.login1",
                                         "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager",
                                         QDBusConnection::systemBus());

    m_arg << QString("shutdown")             // what
          << qApp->productName()           // who
          << QObject::tr("File not saved")          // why
          << QString("block");                        // mode

    m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    if (m_reply.isValid()) {
        qDebug() << "Successfully blocked shutdown";
        /*fd = */(void)m_reply.value().fileDescriptor();
    }
    //如果for结束则表示没有发现未保存的tab项，则放开阻塞关机
    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        qDebug() << "Unblocking shutdown";
    }
}

void CManageViewSigleton::quitIfEmpty()
{
    if (getCurView() == nullptr) {
        qDebug() << "No views available, quitting application";
        qApp->quit();
    }
}

QByteArray CManageViewSigleton::getFileSrcData(const QString &file)
{
    if (file.isEmpty()) {
        qDebug() << "Empty file path provided";
        return QByteArray();
    }
    QFile f(file);
    if (f.exists()) {
        qDebug() << "Reading file:" << file;
        return f.readAll();
    } else {
        qWarning() << "Cannot read file - does not exist:" << file;
    }
    return QByteArray();
}
CFileWatcher::CFileWatcher(QObject *parent)
    : QThread(parent)
{
    qDebug() << "Initializing file watcher";
    _handleId = inotify_init();
}

CFileWatcher::~CFileWatcher()
{
    qDebug() << "Cleaning up file watcher";
    clear();
}

bool CFileWatcher::isVaild()
{
    return (_handleId != -1);
}

void CFileWatcher::addWather(const QString &path)
{
    QMutexLocker loker(&_mutex);
    if (!isVaild()) {
        qWarning() << "Cannot add watch - invalid watcher handle";
        return;
    }

    QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        qWarning() << "Cannot watch path - does not exist or is not a file:" << path;
        return;
    }

    if (watchedFiles.find(path) != watchedFiles.end()) {
        qDebug() << "Path already being watched:" << path;
        return;
    }

    qDebug() << "Adding watch for file:" << path;
    std::string sfile = path.toStdString();
    int fileId = inotify_add_watch(_handleId, sfile.c_str(), IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF);

    watchedFiles.insert(path, fileId);
    watchedFilesId.insert(fileId, path);

    if (!_running) {
        qDebug() << "Starting file watcher thread";
        _running = true;
        start();
    }
}

void CFileWatcher::removePath(const QString &path)
{
    QMutexLocker loker(&_mutex);

    if (!isVaild()) {
        qWarning() << "Cannot remove watch - invalid watcher handle";
        return;
    }

    auto itf = watchedFiles.find(path);
    if (itf != watchedFiles.end()) {
        qDebug() << "Removing watch for file:" << path;
        inotify_rm_watch(_handleId, itf.value());

        watchedFilesId.remove(itf.value());
        watchedFiles.erase(itf);
    }
}

void CFileWatcher::clear()
{
    QMutexLocker loker(&_mutex);
    qDebug() << "Clearing all file watches";

    for (auto it : watchedFiles) {
        inotify_rm_watch(_handleId, it);
    }
    watchedFilesId.clear();
    watchedFiles.clear();
}

void CFileWatcher::run()
{
    qDebug() << "Starting file watcher thread";
    doRun();
}

void CFileWatcher::doRun()
{
    if (!isVaild()) {
        qWarning() << "Cannot run file watcher - invalid handle";
        return;
    }

    bool appQuit = false;
    QObject::connect(qApp, &QApplication::aboutToQuit, [&,this](){
        qDebug() << "Application quitting, stopping file watcher";
        appQuit = true;
        clear();
    });

    char name[1024];
    auto freadsome = [ = ](void *dest, size_t remain, FILE * file) {
        char *offset = reinterpret_cast<char *>(dest);
        while (remain && !appQuit) {
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

    while (!qApp->closingDown() && !appQuit) {
        inotify_event event;
        if (-1 == freadsome(&event, sizeof(event), watcher_file)) {
            qWarning() << "Error reading inotify event";
        }
        if (event.len) {
            freadsome(name, event.len, watcher_file);
        } else {
            QMutexLocker loker(&_mutex);
            auto itf = watchedFilesId.find(event.wd);
            if (itf != watchedFilesId.end()) {
                qDebug() << "File event detected - file:" << itf.value() 
                         << "watch descriptor:" << event.wd 
                         << "event mask:" << event.mask;

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

