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
#include <QFileSystemWatcher>
#include <QAtomicInt>
#include <QDebug>
#include <DDialog>
#include <QThread>
#include <QMutex>

DWIDGET_USE_NAMESPACE

class CGraphicsView;
class CFileWatcher;

class CDrawScene;

class CManageViewSigleton: public QObject
{
    Q_OBJECT
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
     * @brief updateTheme 主题值变化后要刷新
     */
    void updateTheme();

    /**
     * @brief isEmpty 是否是空的(是否没有view)
     */
    bool isEmpty();
    /**
     * @brief getCurView 获取窗口
     */
    CGraphicsView *getCurView();

    /**
     * @brief getCurScene 获取当前的场景
     */
    CDrawScene  *getCurScene();
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
    void updateBlockSystem();
//    /**
//     * @brief getViewByViewName
//     */
//    CGraphicsView *getViewByViewName(QString name);

//    /**
//     * @brief getViewByViewModifyStateName
//     */
//    CGraphicsView *getViewByViewModifyStateName(QString name);
    /**
     * @brief getViewByFilePath
     */
    CGraphicsView *getViewByFilePath(QString path);


    CGraphicsView *getViewByUUID(QString uuid);


    bool  isDdfFileOpened(const QString &path);


    bool wacthFile(const QString &file);
    bool removeWacthedFile(const QString &file);


    Q_SLOT void onDDfFileChanged(const QString &ddfFile);

    Q_SLOT void onDdfFileChanged(const QString &ddfFile, int tp);

    int  viewCount();

    void quitIfEmpty();

    QByteArray getFileSrcData(const QString &file);

private:

    /**
     * @brief initBlockShutdown 柱塞关机
     */
    void initBlockShutdown();

    /**
     * @brief getNoticeFileDialog
     */
    DDialog *getNoticeFileDialog(const QString &file);

    /**
     * @brief getNoticeFileDialog
     */
    DDialog *creatOneNoticeFileDialog(const QString &file, QWidget *parent = nullptr);


    void     removeNoticeFileDialog(DDialog *dialog);

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

    QFileSystemWatcher m_ddfWatcher;

    QList<DDialog *> m_noticeFileDialogs;
    DDialog       *m_pNoticeFileDialog = nullptr;


    CFileWatcher *_ddfWatcher;
};

class CFileWatcher: public QThread
{
    Q_OBJECT
public:
    enum EFileChangedType {EFileModified, EFileMoved, EFileCount};

    explicit CFileWatcher(QObject *parent = nullptr);
    ~CFileWatcher();

    bool isVaild();

    void addWather(const QString &path);
    void removePath(const QString &path);

    void clear();

signals:
    void fileChanged(const QString &path, int tp);

protected:
    void run();

private:
    void doRun();

    int  _handleId = -1;
    bool _running = false;


    QMap<QString, int> watchedFiles;
    QMap<int, QString> watchedFilesId;

    QMutex _mutex;
};

#endif // CDRAWPARAMSIGLETON_H
