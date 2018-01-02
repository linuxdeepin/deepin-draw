#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>

#include "mainwidget.h"
#include "toptoolbar.h"

DWIDGET_USE_NAMESPACE

class MainWindow: public DMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void openImage(const QString &path);
    void parseDdf(const QString &path);
    void showDrawDialog();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    TopToolbar* m_topToolbar;
    MainWidget* m_mainWidget;

    int m_titlebarWidth;
};

#endif // MAINWINDOW_H
