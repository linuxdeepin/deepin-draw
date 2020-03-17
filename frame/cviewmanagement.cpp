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

#include <DApplication>
#include <QGuiApplication>
#include <QDebug>
#include <QString>

DWIDGET_USE_NAMESPACE

CManageViewSigleton *CManageViewSigleton::m_pInstance = nullptr;

CManageViewSigleton::CManageViewSigleton()
{
    m_thremeType = 0;
//    initBlockShutdown();
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
        if(m_allViews[i]->getDrawParam()->viewName() == name) {
            return m_allViews[i];
        }
    }

    // 返回空指针
    return nullptr;
}

CGraphicsView *CManageViewSigleton::getViewByFilePath(QString path)
{
    for (int i = 0; i < m_allViews.count(); i++) {
        if(m_allViews[i]->getDrawParam()->getDdfSavePath() == path) {
            return m_allViews[i];
        }
    }

    // 返回空指针
    return nullptr;
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

