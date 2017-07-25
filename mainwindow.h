#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

#include <QWidget>
#include <QKeyEvent>

#include "mainwidget.h"
#include "widgets/toptoolbar.h"

DWIDGET_USE_NAMESPACE

class MainWindow: public DMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    TopToolbar* m_topToolbar;
    MainWidget* m_mainWidget;

    int m_titlebarWidth;
};

#endif // MAINWINDOW_H
