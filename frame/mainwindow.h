/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXing
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

    void activeWindow();

    void showDrawDialog();
    void openImage(QString path);
    void initScene();

signals:
    void signalResetOriginPoint();

public slots:
    void slotOnThemeChanged(DGuiApplicationHelper::ColorType type);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    TopToolbar *m_topToolbar;
    CCentralwidget *m_centralWidget;
    int m_titlebarWidth;
    bool m_contrlKey;
    DrawDialog *m_quitQuestionDialog;


private:
    void initConnection();
    void initUI();
};

#endif // MAINWINDOW_H
