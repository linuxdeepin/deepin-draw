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

#include <DApplication>
#include <QGuiApplication>
#include <QDebug>
#include <QString>
#include <QFileInfo>

DWIDGET_USE_NAMESPACE

CManageViewSigleton *CManageViewSigleton::m_pInstance = nullptr;

CManageViewSigleton::CManageViewSigleton(): QObject ()
{
    m_thremeType = 0;
//    initBlockShutdown();

    QObject::connect(&m_ddfWatcher, &QFileSystemWatcher::fileChanged, this, &CManageViewSigleton::onDDfFileChanged);
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
        if (curIndex == m_curIndex) {
            if (m_allViews.isEmpty()) {
                m_curIndex = -1;
            } else {
                m_curIndex = 0;
            }
        }
    }
}

void CManageViewSigleton::CheckIsModify()
{
    initBlockShutdown();
    bool shutdownFlag = false;
    for (auto view : m_allViews) {
        if (view->getModify()) {
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

CGraphicsView *CManageViewSigleton::getViewByViewName(QString name)
{
    for (int i = 0; i < m_allViews.count(); i++) {
        if (m_allViews[i]->getDrawParam()->viewName() == name) {
            return m_allViews[i];
        }
    }

    // 返回空指针
    return nullptr;
}

CGraphicsView *CManageViewSigleton::getViewByViewModifyStateName(QString name)
{
    for (int i = 0; i < m_allViews.count(); i++) {
        if (m_allViews[i]->getDrawParam()->getShowViewNameByModifyState() == name) {
            return m_allViews[i];
        }
    }

    // 返回空指针
    return nullptr;
}

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
    bool ret = false;
    if (!file.isEmpty()) {
        if (m_ddfWatcher.files().indexOf(file) == -1)
            ret = m_ddfWatcher.addPath(file);
    }
    //qDebug() << "wacthFile file = " << file << "result = " << ret;
    return ret;
}

bool CManageViewSigleton::removeWacthedFile(const QString &file)
{
    bool ret = false;
    if (m_ddfWatcher.files().indexOf(file) != -1) {
        ret = m_ddfWatcher.removePath(file);
    }
    //qDebug() << "removeWacthedFile file = " << file << "result = " << ret;
    return ret;
}

void CManageViewSigleton::onDDfFileChanged(const QString &ddfFile)
{
    qDebug() << "onDDfFileChangedonDDfFileChangedonDDfFileChangedonDDfFileChanged" << m_ddfWatcher.files();

//    if (ignoreCount() > 0) {
//        reduceIgnoreCount();
//        return;
//    }

    CGraphicsView *pView = getViewByFilePath(ddfFile);
    if (pView != nullptr) {
        CCentralwidget *pCertralWidget = dynamic_cast<CCentralwidget *>(pView->parentWidget());

        //设置为当前view
        pCertralWidget->setCurrentViewByUUID(pView->getDrawParam()->uuid());

        QFileInfo fInfo(ddfFile);
        if (fInfo.exists()) {
            //证明只是内容修改 要提醒是否重新加载
//            DDialog dia(pView);
//            dia.setModal(true);
//            dia.setIcon(QPixmap(":/theme/common/images/deepin-draw-64.svg"));
//            dia.setMessage(tr("%1 has been modified in other programs. Do you want to reload it?").arg(ddfFile));
//            int reload  = dia.addButton(tr("reload"), false, DDialog::ButtonNormal);
//            int cancel = dia.addButton(tr("Cancel"), false, DDialog::ButtonNormal);
//            int ret = dia.exec();

//            if (ret == reload) {

//                //先关闭传入false使程序不会被关闭
//                pCertralWidget->closeCurrentScenseView(false);

//                //再加载
//                emit pView->signalLoadDragOrPasteFile(ddfFile);


//            } else if (ret == cancel) {

//                //直接关闭
//                pCertralWidget->closeCurrentScenseView();
//            }
        } else {
            //证明是被重命名或者删除
            DDialog dia(pView);
            dia.setModal(true);
            dia.setMessage(tr("%1 does not exist any longer. Do you want to keep it here?").arg(ddfFile));
            dia.setIcon(QPixmap(":/theme/common/images/deepin-draw-64.svg"));

            int keep  = dia.addButton(tr("Keep"), false, DDialog::ButtonNormal);
            int discard = dia.addButton(tr("Discard"), false, DDialog::ButtonNormal);
            int ret = dia.exec();

            if (ret == keep) {

                //不再绑定这个文件()
                removeWacthedFile(ddfFile);
                pView->getDrawParam()->setDdfSavePath("");
                pCertralWidget->updateTitle();

            } else if (ret == discard) {
                //直接关闭
                pCertralWidget->closeCurrentScenseView();
            }
        }
    }

    //DDialog dialog();
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

    int fd = -1;
    m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    if (m_reply.isValid()) {
        fd = m_reply.value().fileDescriptor();
    }
    //如果for结束则表示没有发现未保存的tab项，则放开阻塞关机
    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
        qDebug() << "Nublock shutdown.";
    }
}

void CManageViewSigleton::quitIfEmpty()
{
    if (getCurView() == nullptr) {
        qApp->quit();
    }
}

