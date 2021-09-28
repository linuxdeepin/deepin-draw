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

class TopTilte;
class DrawBoard;
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

    void readSettings();

    /**
     * @brief openFile　暴露函数接口给其它程序调用，打开多个路径文件
     * @param filePath　路径
     * @param isStartByDDF　是否以打开ＤＤＦ文件方式启动画板软件
	 * @param bAdapt 是否做自适?
     */
    Q_INVOKABLE bool openFiles(QStringList filePaths, bool bAdapt = false);

    /**
     * @brief drawBoard
     */
    DrawBoard *drawBoard() const;

    /**
     * @brief topTitle　获取TopToolbar指针用于单列测试
     */
    TopTilte *topTitle() const;

public slots:
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
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e)override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *e)override;

private slots:
    /**
     * @brief slotShowOpenFileDialog　显示打开文件对话框
     */
    void slotShowOpenFileDialog();

private:
    TopTilte *m_topToolbar;
    DrawBoard *m_drawBoard;
    int m_titlebarWidth;
    bool m_contrlKey;
    QAction *m_quitMode;
    QAction *m_showCut;
    QString tmpPictruePath;
    DFloatingMessage *pDFloatingMessage = nullptr;    //驻留消息提示

private:
    /**
     * @brief initConnection　初始化连接
     */
    void initConnection();
    /**
     * @brief initUI　初始化ＵＩ
     */
    void initUI();
};

#endif // MAINWINDOW_H
