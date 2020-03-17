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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DWidget>
#include <DGuiApplicationHelper>
#include <DDialog>

#include <QMouseEvent>


DWIDGET_USE_NAMESPACE

class TopToolbar;
class CCentralwidget;
class DrawDialog;

class MainWindow: public DMainWindow
{
    Q_OBJECT
public:
    MainWindow(DWidget *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

    /**
     * @brief activeWindow 激活主窗口
     */
    void activeWindow();
    /**
     * @brief showDrawDialog　退出询问对话框
     */
    void showDrawDialog();
    /**
     * @brief openImage　打开图片或ＤＤＦ文件
     * @param path　路劲
     * @param isStartByDDF　是否以打开ＤＤＦ文件方式启动画板软件
     */
    void openImage(QString path, bool isStartByDDF = false);
    /**
     * @brief initScene　初始化场景
     */
    void initScene();

    void readSettings();


signals:
    /**
     * @brief signalResetOriginPoint 重置原始点信号
     */
    void signalResetOriginPoint();

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
     * @brief slotLoadDragOrPasteFile　粘贴或拖拽文件槽函数
     * @param files
     */
    void slotLoadDragOrPasteFile(QStringList files);
    /**
     * @brief slotOnEscButtonClick　ＥＳＣ按钮槽函数
     */
    void slotOnEscButtonClick();


protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;


private slots:
    /**
     * @brief slotShowOpenFileDialog　显示打开文件对话框
     */
    void slotShowOpenFileDialog();
    /**
     * @brief slotDDFFileOpened　显示打开文件对话框
     */
    void slotDDFFileOpened(QString filename);
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
    DrawDialog *m_quitQuestionDialog;
    QAction *m_quitMode;
    QAction *m_showCut;
    QString tmpPictruePath;
    DDialog m_dialog; // 最后一个标签被关闭提示框


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
     * @brief showDragOrOpenFile　显示拖拽或者打开文件
     * @param files 文件列表路径
     * @param isOPenFile 是否是打开文件
     */
    void showDragOrOpenFile(QStringList files,bool isOPenFile);

};

#endif // MAINWINDOW_H
