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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DWidget>
#include <DGuiApplicationHelper>
#include <DDialog>
#include <DWidgetUtil>

#include <QMouseEvent>


DWIDGET_USE_NAMESPACE

class TopToolbar;
class CCentralwidget;
class DrawDialog;

class MainWindow: public DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QStringList filePaths); // 需要打开多个文件时的构造函数
    ~MainWindow() override;

    /**
     * @brief activeWindow 激活主窗口
     */
    void activeWindow();
//    /**
//     * @brief showDrawDialog　退出询问对话框
//     */
//    void showDrawDialog();

    void readSettings();

    /**
     * @brief openFile　暴露函数接口给其它程序调用，打开多个路径文件
     * @param filePath　路径
     * @param isStartByDDF　是否以打开ＤＤＦ文件方式启动画板软件
     */
    Q_INVOKABLE bool openFiles(QStringList filePaths);
//    /**
//     * @brief openImage　暴露函数接口给其它程序调用，打开多个图像文件
//     * @param image　图片
//     * @param isStartByDDF　是否以打开ＤＤＦ文件方式启动画板软件
//     */
//    Q_INVOKABLE bool openImage(QImage image, const QByteArray &srcData);

    /**
     * @brief getCCentralwidget 获取CCentralwidget指针用于单列测试
     */
    CCentralwidget *getCCentralwidget() const;

    /**
     * @brief getTopToolbar　获取TopToolbar指针用于单列测试
     */
    TopToolbar *getTopToolbar() const;

public slots:
    /**
     * @brief slotOnThemeChanged　主题变化槽函数
     * @param type　颜色类型
     */
    void slotOnThemeChanged(DGuiApplicationHelper::ColorType type);
    /**
     * @brief slotIsNeedSave　是否需要保存槽函数
     */
    void slotIsNeedSave();
    /**
     * @brief slotContinueDoSomeThing　保存完后继续做某些事槽函数
     */
    void slotContinueDoSomeThing();
    /**
     * @brief onViewShortcut
     */
    void onViewShortcut();
    /**
     * @brief slotOnEscButtonClick　ＥＳＣ按钮槽函数
     */
    void slotOnEscButtonClick();


protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    bool event(QEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e)override;


private slots:
    /**
     * @brief slotShowOpenFileDialog　显示打开文件对话框
     */
    void slotShowOpenFileDialog();
    /**
     * @brief slotTopToolBarSaveToDDF　保存ddf文件
     */
    void slotTopToolBarSaveToDDF();
    /**
     * @brief slotLastTabBarRequestClose　最后一个标签被关闭
     */
    void slotLastTabBarRequestClose();
    /**
     * @brief slotNewDrawScence　新建一个标签
     */
    void slotNewDrawScence();

private:
    TopToolbar *m_topToolbar;
    CCentralwidget *m_centralWidget;
    int m_titlebarWidth;
    bool m_contrlKey;
    QAction *m_quitMode;
    QAction *m_showCut;
    QString tmpPictruePath;
    QStringList m_closeViews; //待关闭的标签
    QStringList m_closeUUids; //待关闭的标签uuid

private:
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();
    /**
     * @brief showSaveQuestionDialog　显示提示是否保存对话框
     */
    int showSaveQuestionDialog();
    /**
     * @brief doCloseOtherDiv　判断是否需要继续关闭页面
     */
    void closeTabViews();


    QSet<QWidget *> m_allChilds;

    QLabel *m_pWebsiteLabe = nullptr;
};

#endif // MAINWINDOW_H
