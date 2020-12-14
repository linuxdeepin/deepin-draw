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

#define protected public
#include <DApplication>
#undef protected

#include <DGuiApplicationHelper>

class MainWindow;
class CColorPickWidget;
class TopToolbar;
class CLeftToolBar;
class CDrawScene;
class CGraphicsView;

#if  defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<DApplication *>(QCoreApplication::instance()))
#define drawApp (Application::drawApplication())

DWIDGET_USE_NAMESPACE

class Application : public QObject
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    /**
     * @brief drawApplication 返回顶层topToolbar
     * @return 返回画板程序的指针
     */
    static Application *drawApplication();


    DApplication *dApplication();


    int  execDraw(const QStringList &paths);

    /**
     * @brief topMainWindow 返回顶层mainwindow
     * @return
     */
    MainWindow *topMainWindow();
    QWidget *topMainWindowWidget();

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

    /**
     * @brief getRightFiles 根据输入返回所有合法正确的可加载文件(并且会弹窗提示)
     * @return 返回所有合法正确的可加载文件
     */
    QStringList getRightFiles(const QStringList &files, bool notice = true);

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
     * @brief setTouchFeelingEnhanceValue 设置触控感受的增强值
     */
    void setTouchFeelingEnhanceValue(int value);

    /**
     * @brief touchFeelingEnhanceValue 当前触控感受的增强值
     */
    int  touchFeelingEnhanceValue();

    /**
     * @brief supPictureSuffix 返回支持的所有图片后缀名
     */
    static QStringList &supPictureSuffix();

    /**
     * @brief supDdfStuffix 返回支持的所有ddf后缀名(暂时只有.ddf)
     */
    static QStringList &supDdfStuffix();

    /**
     * @brief isFileExist 文件是否存在，如果是资源型路径那么会将其替换成本地路径
     */
    bool isFileExist(QString &filePath);

    /**
     * @brief setWidgetAllPosterityNoFocus 将widget的后代都设置为没有焦点
     */
    static void  setWidgetAllPosterityNoFocus(QWidget *pW);

    /**
     * @brief setWidgetAccesibleName 设置widget对象外界可访问的名字属性
     */
    static void setWidgetAccesibleName(QWidget *w, const QString &name);

    /**
     * @brief 当程序退出时会调用
     */
    void onAppQuit();

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
    void onThemChanged(DGuiApplicationHelper::ColorType themeType);
    void activateWindow();
    void showMainWindow(const QStringList &paths);
    void noticeFileRightProblem(const QStringList &problemfile,
                                Application::EFileClassEnum classTp = EDrawAppNotSup,
                                bool checkQuit = true);


public:
    enum   EMessageType {ENormalMsg, EWarningMsg, EQuestionMsg};
    Q_SLOT int exeMessage(const QString &message, EMessageType msgTp = EWarningMsg,
                          bool autoFitDialogWidth = true,
                          const QStringList &moreBtns = QStringList() << tr("OK"),
                          const QList<int> &btnType = QList<int>() << 0);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    void initI18n();
    MainWindow *actWin = nullptr;

    CColorPickWidget *_colorPickWidget = nullptr;

    DApplication *_dApp = nullptr;

    int  _touchEnchValue = 7;

    static Application *s_drawApp;
};
#endif // APPLICATION_H
