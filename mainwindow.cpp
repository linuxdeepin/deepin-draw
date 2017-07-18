#include "mainwindow.h"

#include <QVBoxLayout>

#include <DTitlebar>
#include <QCheckBox>
MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent) {
    setMinimumSize(450, 450);
    m_topToolbar = new TopToolbar(this);
    titleBar()->setCustomWidget(m_topToolbar, Qt::AlignCenter);

    m_mainWidget = new MainWidget(this);
    setCentralWidget(m_mainWidget);
}

MainWindow::~MainWindow() {

}
