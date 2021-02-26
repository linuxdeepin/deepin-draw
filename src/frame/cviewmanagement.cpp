/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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

#include <sys/inotify.h>

DWIDGET_USE_NAMESPACE

CManageViewSigleton *CManageViewSigleton::m_pInstance = nullptr;

CManageViewSigleton::CManageViewSigleton()
    : QObject()
{
    m_thremeType = 0;

    _ddfWatcher = new CFileWatcher(this);
    connect(_ddfWatcher, &CFileWatcher::fileChanged, this, &CManageViewSigleton::onDdfFileChanged);

    Q_UNUSED(getFileSrcData(""));
}

CManageViewSigleton *CManageViewSigleton::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new CManageViewSigleton();
    }
    return m_pInstance;
}

int CManageViewSigleton::getThemeType() const
{
    return m_thremeType;
}

void CManageViewSigleton::setThemeType(const int type)
{
    m_thremeType = type;
}

void CManageViewSigleton::updateTheme()
{
    for (int i = 0; i < m_allViews.size(); ++i) {
        CGraphicsView *pView         = m_allViews[i];
        QList<QGraphicsItem *> items = pView->items();

        for (int j = 0; j < items.size(); ++j) {
            QGraphicsItem *pItem = items[j];
            if (pItem->type() == TextType) {
                CGraphicsTextItem *pTextItem = dynamic_cast<CGraphicsTextItem *>(pItem);
                if (pTextItem != nullptr) {
                    CTextEdit *pEdit = pTextItem->textEditor();
                    if (pEdit != nullptr) {
                        pEdit->updateBgColorTo();
                    }
                }
            }
        }
    }
}

bool CManageViewSigleton::isEmpty()
{
    return m_allViews.isEmpty();
}

CGraphicsView *CManageViewSigleton::getCurView()
{
    CGraphicsView *curView = nullptr;

    //获取当前窗口
    if (!m_allViews.isEmpty() && m_curIndex >= 0 && m_curIndex < m_allViews.size()) {
        curView = m_allViews[m_curIndex];
    }

    return curView;
}

CDrawScene *CManageViewSigleton::getCurScene()
{
    auto view = getCurView();
    if (view != nullptr)
        return view->drawScene();
    return nullptr;
}

void CManageViewSigleton::addView(CGraphicsView *view)
{
    if (nullptr == view) {
        return;
    }
    if (!m_allViews.contains(view)) {
        m_allViews.append(view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
    }
}

void CManageViewSigleton::setCurView(CGraphicsView *view)
{
    if (nullptr == view) {
        return;
    }
    if (m_allViews.contains(view)) {
        m_curIndex = m_allViews.indexOf(view);
    }
    if (m_curIndex == -1 && !m_allViews.isEmpty()) {
        m_curIndex = 0;
    }
}

void CManageViewSigleton::removeView(CGraphicsView *view)
{
    if (nullptr == view) {
        return;
    }
    if (m_allViews.contains(view)) {
        auto curIndex = m_allViews.indexOf(view);
        m_allViews.removeAt(curIndex);

        QString ddfPath = view->getDrawParam()->getDdfSavePath();
        removeWacthedFile(ddfPath);

        if (curIndex == m_curIndex) {
            if (m_allViews.isEmpty()) {
                m_curIndex = -1;
            } else {
                m_curIndex = 0;
            }
        }
    }
}

void CManageViewSigleton::updateBlockSystem()
{
    initBlockShutdown();
    bool shutdownFlag = false;
    for (auto view : m_allViews) {
        if (view->isModified()) {
            shutdownFlag = true;
            break;
        }
    }
    if (shutdownFlag) {
        m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    } else {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
        qDebug() << "Nublock shutdown.";
    }
}

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

CGraphicsView *CManageViewSigleton::getViewByFilePath(QString path)
{
    for (int i = 0; i < m_allViews.count(); i++) {
        if (m_allViews[i]->getDrawParam()->getDdfSavePath() == path) {
            return m_allViews[i];
        }
    }

    // 返回空指针
    return nullptr;
}

CGraphicsView *CManageViewSigleton::getViewByUUID(QString uuid)
{
    for (int i = 0; i < m_allViews.count(); i++) {
        if (m_allViews[i]->getDrawParam()->uuid() == uuid) {
            return m_allViews[i];
        }
    }

    // 返回空指针
    return nullptr;
}

bool CManageViewSigleton::isDdfFileOpened(const QString &path)
{
    return (getViewByFilePath(path) != nullptr);
}

//void CManageViewSigleton::blockWacthFile(const QString &file)
//{
//    QMutexLocker locker(&m_mutex);
//    m_ddfWatcher.remov(file);
//}

bool CManageViewSigleton::wacthFile(const QString &file)
{
//    if (file.isEmpty())
//        return false;

//    bool ret = false;
//    if (!file.isEmpty()) {
//        if (m_ddfWatcher.files().indexOf(file) == -1)
//            ret = m_ddfWatcher.addPath(file);
//    }
//    return ret;

    _ddfWatcher->addWather(file);
    return true;
}

bool CManageViewSigleton::removeWacthedFile(const QString &file)
{
//    if (file.isEmpty())
//        return false;

//    bool ret = false;
//    if (m_ddfWatcher.files().indexOf(file) != -1) {
//        ret = m_ddfWatcher.removePath(file);
//    }
//    return ret;

    _ddfWatcher->removePath(file);
    return true;
}

void CManageViewSigleton::onDDfFileChanged(const QString &ddfFile)
{
    //qDebug() << "onDDfFileChangedonDDfFileChangedonDDfFileChangedonDDfFileChanged" << m_ddfWatcher.files() << "cur file = " << ddfFile;

    CGraphicsView *pView = getViewByFilePath(ddfFile);
    if (pView != nullptr) {
        CCentralwidget *pCertralWidget = dynamic_cast<CCentralwidget *>(pView->parentWidget());

        //设置为当前view
        pCertralWidget->setCurrentViewByUUID(pView->getDrawParam()->uuid());

        QFileInfo fInfo(ddfFile);
        if (fInfo.exists()) {
            qDebug() << "onDDfFileChanged(----modifyed----) file = " << ddfFile;
            //证明只是内容修改 要提醒是否重新加载
            //先判断是否已经存在对这个文件已修改的提示
            DDialog *dia = getNoticeFileDialog(ddfFile);
            if (dia != nullptr) {
                //证明已经监视到文件的改动了有弹出床提示了那么就不用再弹出一个同一样的了

                Application *pApp = drawApp;
                if (pApp != nullptr) {
                    pApp->activateWindow();
                }
                return;
            } else {
                dia = creatOneNoticeFileDialog(ddfFile, pView->parentWidget());


                //设置显示文字与交互按钮
                QString shortenFileName = QFontMetrics(dia->font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia->width() / 2);
                dia->setMessage(tr("%1 has been modified in other programs. Do you want to reload it?").arg(shortenFileName));
                int reload  = dia->addButton(tr("Reload"), false, DDialog::ButtonNormal);
                int cancel  = dia->addButton(tr("Cancel"), false, DDialog::ButtonWarning);


                //设置message的两边间隙为10
                QWidget *pWidget = dia->findChild<QWidget *>("MessageLabel"); //如果dtk修改了object名字，记得修改
                if (pWidget != nullptr) {
                    QMargins margins = pWidget->contentsMargins();
                    margins.setLeft(10);
                    margins.setRight(10);
                    pWidget->setContentsMargins(margins);
                }

                //运行交互窗口
                int ret = dia->exec();

                //交互窗口结束
                removeNoticeFileDialog(dia);
                if (ret == reload) {

                    //先关闭传入false使程序不会被关闭
                    pCertralWidget->closeSceneView(pView, false, false);

                    //再加载
                    emit pView->signalLoadDragOrPasteFile(ddfFile);


                } else if (ret == cancel) {
                    //直接关闭
                    pCertralWidget->closeSceneView(pView);
                }
            }
        } else {

            qDebug() << "onDDfFileChanged(----deleted----) file = " << ddfFile;

            //不再绑定这个文件()
            removeWacthedFile(ddfFile);

            /*            //证明是被重命名或者删除
                        DDialog dia(pView->parentWidget());
                        dia.setFixedSize(404, 163);
                        dia.setModal(true);
                        QString shortenFileName = QFontMetrics(dia.font()).elidedText(fInfo.fileName(), Qt::ElideMiddle, dia.width() / 2);
                        dia.setMessage(tr("%1 does not exist any longer. Do you want to keep it here?").arg(shortenFileName));
                        dia.setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));

                        //设置message的两边间隙为10
                        QWidget *pWidget = dia.findChild<QWidget *>("MessageLabel"); //如果dtk修改了object名字，记得修改
                        if (pWidget != nullptr) {
                            QMargins margins = pWidget->contentsMargins();
                            margins.setLeft(10);
                            margins.setRight(10);
                            pWidget->setContentsMargins(margins);
                        }

                        int keep  = dia.addButton(tr("Keep"), false, DDialog::ButtonNormal);
                        int discard = dia.addButton(tr("Discard"), false, DDialog::ButtonWarning);
                        int ret = dia.exec();

                        Q_UNUSED(keep);

                        if (ret == discard) {
                            //直接关闭
                            pCertralWidget->closeSceneView(pView);
                        } else*/ {
                pView->getDrawParam()->setDdfSavePath("");
                pView->setModify(true);
                pCertralWidget->updateTitle();
            }
        }
    }

    //qDebug() << QString("file(%1) changed slot end current watched files = ").arg(ddfFile) << m_ddfWatcher.files();
}

void CManageViewSigleton::onDdfFileChanged(const QString &ddfFile, int tp)
{
    Q_UNUSED(tp);
    onDDfFileChanged(ddfFile);
}

int CManageViewSigleton::viewCount()
{
    return m_allViews.size();
}

void CManageViewSigleton::initBlockShutdown()
{
    if (!m_arg.isEmpty() || m_reply.value().isValid()) {
        qDebug() << "m_reply.value().isValid():" << m_reply.value().isValid();
        return;
    }

    m_pLoginManager = new QDBusInterface("org.freedesktop.login1",
                                         "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager",
                                         QDBusConnection::systemBus());

    m_arg << QString("shutdown")             // what
          << qApp->productName()           // who
          << QObject::tr("File not saved")          // why
          << QString("block");                        // mode

    //int fd = -1;
    m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    if (m_reply.isValid()) {
        /*fd = */(void)m_reply.value().fileDescriptor();
    }
    //如果for结束则表示没有发现未保存的tab项，则放开阻塞关机
    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
        qDebug() << "Nublock shutdown.";
    }
}

DDialog *CManageViewSigleton::getNoticeFileDialog(const QString &file)
{
    for (DDialog *pDialog : m_noticeFileDialogs) {
        if (pDialog->property("file").toString() == file) {
            return pDialog;
        }
    }
    return nullptr;
}

DDialog *CManageViewSigleton::creatOneNoticeFileDialog(const QString &file, QWidget *parent)
{
    DDialog *pNoticeFileDialog = new DDialog(parent == nullptr ? qApp->activeWindow() : parent);
    pNoticeFileDialog->setFixedSize(404, 163);
    pNoticeFileDialog->setModal(true);
    pNoticeFileDialog->setIcon(QPixmap(":/icons/deepin/builtin/Bullet_window_warning.svg"));
    pNoticeFileDialog->setProperty("file", file);
    m_noticeFileDialogs.append(pNoticeFileDialog);
    return pNoticeFileDialog;
}

void CManageViewSigleton::removeNoticeFileDialog(DDialog *dialog)
{
    m_noticeFileDialogs.removeOne(dialog);
    dialog->deleteLater();
}

void CManageViewSigleton::quitIfEmpty()
{
    if (getCurView() == nullptr) {
        qApp->quit();
    }
}

QByteArray CManageViewSigleton::getFileSrcData(const QString &file)
{
    if (file.isEmpty())
        return QByteArray();
    QFile f(file);
    if (f.exists()) {
        return f.readAll();
    } else {
        qWarning() << QString("can not read, %1 is not exists!").arg(file);
    }
    return QByteArray();
}
CFileWatcher::CFileWatcher(QObject *parent)
    : QThread(parent)
{
    _handleId = inotify_init();
}

CFileWatcher::~CFileWatcher()
{
    clear();
}

bool CFileWatcher::isVaild()
{
    return (_handleId != -1);
}

void CFileWatcher::addWather(const QString &path)
{
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
}

void CFileWatcher::removePath(const QString &path)
{
    QMutexLocker loker(&_mutex);

    if (!isVaild())
        return;

    auto itf = watchedFiles.find(path);
    if (itf != watchedFiles.end()) {
        inotify_rm_watch(_handleId, itf.value());

        watchedFilesId.remove(itf.value());
        watchedFiles.erase(itf);
    }
}

void CFileWatcher::clear()
{
    QMutexLocker loker(&_mutex);

    for (auto it : watchedFiles) {
        inotify_rm_watch(_handleId, it);
    }
    watchedFilesId.clear();
    watchedFiles.clear();
}

void CFileWatcher::run()
{
    doRun();
}

void CFileWatcher::doRun()
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

