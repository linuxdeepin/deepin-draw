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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <DApplication>
#include <DGuiApplicationHelper>

#include "qtsingleapplication.h"

class Application;
class MainWindow;
class CColorPickWidget;
class TopToolbar;
class CLeftToolBar;
class CDrawScene;
class CGraphicsView;

#if  defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<Application *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    int  execDraw(const QStringList &paths, QString &glAppPath);

    /**
     * @brief topMainWindow 返回顶层mainwindow
     * @return
     */
    MainWindow *topMainWindow();

    /**
     * @brief colorPickWidget 返回顶层colorPickWidget
     * @return
     */
    CColorPickWidget *colorPickWidget(bool creatNew = false, QWidget *pCaller = nullptr);


    /**
     * @brief topToolbar 返回顶层topToolbar
     * @return 返回顶层工具块界面指针
     */
    TopToolbar *topToolbar();

    /**
     * @brief leftToolBar 返回顶层leftToolBar
     * @return 返回左侧工具块界面指针
     */
    CLeftToolBar *leftToolBar();

    /**
     * @brief currentDrawScence 返回当前显示的画布场景
     * @return 返回当前显示的画布场景指针
     */
    CDrawScene *currentDrawScence();

//    /**
//     * @brief currentDrawView 返回当前显示的画布视图
//     * @return 返回当前显示的画布视图指针
//     */
//    CGraphicsView *currentDrawView();

    /**
     * @brief getRightFiles 根据输入返回所有合法正确的可加载文件(并且会弹窗提示)
     * @return 返回所有合法正确的可加载文件
     */
    QStringList getRightFiles(const QStringList &files);

    /**
     * @brief isFileNameLegal 判断文件是否合法(是否可加载)
     * @return 返回文件是否合法
     */
    bool isFileNameLegal(const QString &path, int *outErrorReson = nullptr);

    /**
     * @brief setApplicationCursor 设置全局鼠标样式
     */
    void setApplicationCursor(const QCursor &cur, bool force = false);

    /**
     * @brief supPictureSuffix 返回支持的所有图片后缀名
     */
    static QStringList &supPictureSuffix();

    /**
     * @brief supDdfStuffix 返回支持的所有ddf后缀名(暂时只有.ddf)
     */
    static QStringList &supDdfStuffix();

//    /**
//     * @brief fileNameRegExp 返回不要特殊字符(不能用于文件名的字符)的正则表达式的
//     */
//    static QRegExp fileNameRegExp(bool ill = false, bool containDirDelimiter = true);


    /**
     * @brief setWidgetAllPosterityNoFocus 将widget的后代都设置为没有焦点
     */
    static void  setWidgetAllPosterityNoFocus(QWidget *pW);

private:
    enum EFileClassEnum {ENotFile    = 0,
                         ENotExist,
                         EDrawAppNotSup,
                         EDrawAppSup,
                         EDrawAppSupAndReadable,
                         EDrawAppSupButNotReadable
                        };
    typedef QMap<EFileClassEnum, QStringList> QFileClassedMap;
    QStringList doFileClassification(const QStringList &inFilesPath, QFileClassedMap &out);

signals:
    void popupConfirmDialog();

public slots:
    void onMessageRecived(const QString &message);
    void onThemChanged(DGuiApplicationHelper::ColorType themeType);

    void showMainWindow(const QStringList &paths);
    void noticeFileRightProblem(const QStringList &problemfile,
                                Application::EFileClassEnum classTp = EDrawAppNotSup,
                                bool checkQuit = true);
protected:
    bool notify(QObject *o, QEvent *e) override;
    void handleQuitAction() override;

private:
    void initI18n();
    bool isFileExist(QString &filePath);

    QString _joinFlag;

    CColorPickWidget *_colorPickWidget = nullptr;
};
#endif // APPLICATION_H
