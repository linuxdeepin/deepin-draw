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
#ifndef CMANAGEVIEWSIGLETON_H
#define CMANAGEVIEWSIGLETON_H

#include <QList>
#include <QDBusReply>
#include <QDBusInterface>
#include <QDBusUnixFileDescriptor>

class CGraphicsView;

class CManageViewSigleton
{
private :
    static CManageViewSigleton *m_pInstance;
    CManageViewSigleton();

public :
    static CManageViewSigleton *GetInstance();

public:
    /**
     * @brief getThemeType 获取主题
     */
    int getThemeType() const;
    /**
     * @brief setThemeType 设置主题
     */
    void setThemeType(const int type);
    /**
     * @brief getCurView 获取窗口
     */
    CGraphicsView *getCurView();
    /**
     * @brief addView 添加窗口
     */
    void addView(CGraphicsView *view);
    /**
     * @brief setCurView 设置当前窗口
     */
    void setCurView(CGraphicsView *view);
    /**
     * @brief setCurView 删除窗口
     */
    void removeView(CGraphicsView *view);
    /**
     * @brief CheckIsModify 检测是否修改
     */
    void CheckIsModify();
    /**
     * @brief getViewByViewName
     */
    CGraphicsView *getViewByViewName(QString name);
    /**
     * @brief getViewByFilePath
     */
    CGraphicsView *getViewByFilePath(QString path);
private:
    /**
     * @brief initBlockShutdown 柱塞关机
     */
    void initBlockShutdown();

private:
    //当前主题
    int m_thremeType = 0;
    //所有窗口
    QList<CGraphicsView *> m_allViews;
    //当前索引
    int m_curIndex = -1;

    QDBusReply<QDBusUnixFileDescriptor> m_reply;
    QDBusInterface *m_pLoginManager = nullptr;
    QList<QVariant> m_arg;
};


#endif // CDRAWPARAMSIGLETON_H
