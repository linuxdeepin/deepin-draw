#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

#include <QWidget>

DWIDGET_USE_NAMESPACE

class MainWindow: public DMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
