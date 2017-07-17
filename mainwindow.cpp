#include "mainwindow.h"

#include <QVBoxLayout>

#include <DTitlebar>

MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent) {
    setMinimumSize(450, 450);

    if (titleBar()) titleBar()->setFixedHeight(0);

    m_mainWidget = new MainWidget(this);
    setCentralWidget(m_mainWidget);
}

MainWindow::~MainWindow() {

}
