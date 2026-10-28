// SPDX-FileCopyrightText: 2020-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cviewmanagement.h"
#include "ddlog.h"
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
    qCDebug(drawFrameLog) << "Initializing CManageViewSigleton";
    Q_UNUSED(getFileSrcData(""));
}

CManageViewSigleton *CManageViewSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        qCDebug(drawFrameLog) << "Creating new CManageViewSigleton instance";
        m_pInstance  = new CManageViewSigleton();
    }
    return m_pInstance;
}

bool CManageViewSigleton::isEmpty()
{
    qCDebug(drawFrameLog) << "Checking if view list is empty - count:" << m_allViews.size();
    return m_allViews.isEmpty();
}

PageView *CManageViewSigleton::getCurView()
{
    PageView *curView = nullptr;

    //获取当前窗口
    if (!m_allViews.isEmpty() && m_curIndex >= 0 && m_curIndex < m_allViews.size()) {
        curView = m_allViews[m_curIndex];
        qCDebug(drawFrameLog) << "Getting current view - index:" << m_curIndex;
    } else {
        qCDebug(drawFrameLog) << "No current view available";
    }

    return curView;
}

PageScene *CManageViewSigleton::getCurScene()
{
    auto view = getCurView();
    if (view != nullptr) {
        qCDebug(drawFrameLog) << "Getting current scene from view";
        return view->drawScene();
    }
    qCDebug(drawFrameLog) << "No current scene available";
    return nullptr;
}

void CManageViewSigleton::addView(PageView *view)
{
    if (nullptr == view) {
        qCWarning(drawFrameLog) << "Attempted to add null view";
        return;
    }
    if (!m_allViews.contains(view)) {
        qCDebug(drawFrameLog) << "Adding new view to management";
        m_allViews.append(view);
        //emit viewAdded(view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
        qCDebug(drawFrameLog) << "Setting initial current view index to 0";
    }
}

void CManageViewSigleton::setCurView(PageView *view)
{
    if (nullptr == view) {
        qCWarning(drawFrameLog) << "Attempted to set null view as current";
        return;
    }
    auto currentView = getCurView();
    if (currentView == view) {
        qCDebug(drawFrameLog) << "View is already current view";
        return;
    }

    if (m_allViews.contains(view)) {
        m_curIndex = m_allViews.indexOf(view);
        qCDebug(drawFrameLog) << "Setting current view - new index:" << m_curIndex;
        //emit viewChanged(currentView, view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
        qCDebug(drawFrameLog) << "Setting initial current view index to 0";
    }
}

void CManageViewSigleton::removeView(PageView *view)
{
    if (nullptr == view) {
        qCWarning(drawFrameLog) << "Attempted to remove null view";
        return;
    }
    if (m_allViews.contains(view)) {
        auto curIndex = m_allViews.indexOf(view);
        qCDebug(drawFrameLog) << "Removing view - index:" << curIndex;
        m_allViews.removeAt(curIndex);

        QString ddfPath = /*view->getDrawParam()->getDdfSavePath()*/"";
        //removeWacthedFile(ddfPath);

        if (curIndex == m_curIndex) {
            if (m_allViews.isEmpty()) {
                m_curIndex = -1;
                qCDebug(drawFrameLog) << "No views remaining after removal";
                //emit viewChanged(view, nullptr);
            } else {
                m_curIndex = 0;
                qCDebug(drawFrameLog) << "Setting current view index to 0 after removal";
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
//        qCDebug(drawFrameLog) << "Nublock shutdown.";
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
    qCDebug(drawFrameLog) << "Getting view count:" << m_allViews.size();
    return m_allViews.size();
}

void CManageViewSigleton::initBlockShutdown()
{
    if (!m_arg.isEmpty() || m_reply.value().isValid()) {
        qCDebug(drawFrameLog) << "Shutdown blocking already initialized - reply valid:" << m_reply.value().isValid();
        return;
    }

    qCDebug(drawFrameLog) << "Initializing shutdown blocking";
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
        qCDebug(drawFrameLog) << "Successfully blocked shutdown";
        /*fd = */(void)m_reply.value().fileDescriptor();
    }
    //如果for结束则表示没有发现未保存的tab项，则放开阻塞关机
    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        qCDebug(drawFrameLog) << "Unblocking shutdown";
    }
}

void CManageViewSigleton::quitIfEmpty()
{
    if (getCurView() == nullptr) {
        qCDebug(drawFrameLog) << "No views available, quitting application";
        qApp->quit();
    }
}

QByteArray CManageViewSigleton::getFileSrcData(const QString &file)
{
    if (file.isEmpty()) {
        qCDebug(drawFrameLog) << "Empty file path provided";
        return QByteArray();
    }
    QFile f(file);
    if (f.exists()) {
        qCDebug(drawFrameLog) << "Reading file:" << file;
        return f.readAll();
    } else {
        qCWarning(drawFrameLog) << "Cannot read file - does not exist:" << file;
    }
    return QByteArray();
}
CFileWatcher::CFileWatcher(QObject *parent)
    : QThread(parent)
{
    qCDebug(drawFrameLog) << "Initializing file watcher";
    _handleId = inotify_init();
}

CFileWatcher::~CFileWatcher()
{
    qCDebug(drawFrameLog) << "Cleaning up file watcher";
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
        qCWarning(drawFrameLog) << "Cannot add watch - invalid watcher handle";
        return;
    }

    QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        qCWarning(drawFrameLog) << "Cannot watch path - does not exist or is not a file:" << path;
        return;
    }

    if (watchedFiles.find(path) != watchedFiles.end()) {
        qCDebug(drawFrameLog) << "Path already being watched:" << path;
        return;
    }

    qCDebug(drawFrameLog) << "Adding watch for file:" << path;
    std::string sfile = path.toStdString();
    int fileId = inotify_add_watch(_handleId, sfile.c_str(), IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF);

    watchedFiles.insert(path, fileId);
    watchedFilesId.insert(fileId, path);

    if (!_running) {
        qCDebug(drawFrameLog) << "Starting file watcher thread";
        _running = true;
        start();
    }
}

void CFileWatcher::removePath(const QString &path)
{
    QMutexLocker loker(&_mutex);

    if (!isVaild()) {
        qCWarning(drawFrameLog) << "Cannot remove watch - invalid watcher handle";
        return;
    }

    auto itf = watchedFiles.find(path);
    if (itf != watchedFiles.end()) {
        qCDebug(drawFrameLog) << "Removing watch for file:" << path;
        inotify_rm_watch(_handleId, itf.value());

        watchedFilesId.remove(itf.value());
        watchedFiles.erase(itf);
    }
}

void CFileWatcher::clear()
{
    QMutexLocker loker(&_mutex);
    qCDebug(drawFrameLog) << "Clearing all file watches";

    for (auto it : watchedFiles) {
        inotify_rm_watch(_handleId, it);
    }
    watchedFilesId.clear();
    watchedFiles.clear();
}

void CFileWatcher::run()
{
    qCDebug(drawFrameLog) << "Starting file watcher thread";
    doRun();
}

void CFileWatcher::doRun()
{
    if (!isVaild()) {
        qCWarning(drawFrameLog) << "Cannot run file watcher - invalid handle";
        return;
    }

    bool appQuit = false;
    QObject::connect(qApp, &QApplication::aboutToQuit, [&,this](){
        qCDebug(drawFrameLog) << "Application quitting, stopping file watcher";
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
            qCWarning(drawFrameLog) << "Error reading inotify event";
        }
        if (event.len) {
            freadsome(name, event.len, watcher_file);
        } else {
            QMutexLocker loker(&_mutex);
            auto itf = watchedFilesId.find(event.wd);
            if (itf != watchedFilesId.end()) {
                qCDebug(drawFrameLog) << "File event detected - file:" << itf.value() 
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

