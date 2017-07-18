#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

#include <QWidget>

#include "mainwidget.h"
#include "widgets/toptoolbar.h"

DWIDGET_USE_NAMESPACE

class MainWindow: public DMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    TopToolbar* m_topToolbar;
    MainWidget* m_mainWidget;
};

#endif // MAINWINDOW_H
