#include "mainwindow.h"

#include <QVBoxLayout>

#include <DTitlebar>
#include <QCheckBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent) {
    setMinimumSize(1050, 850);
    m_topToolbar = new TopToolbar(this);
    titlebar()->setCustomWidget(m_topToolbar, Qt::AlignCenter);
    m_titlebarWidth = titlebar()->width();
    m_topToolbar->setFixedWidth(this->width() - m_titlebarWidth);

    m_mainWidget = new MainWidget(this);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_mainWidget);

    connect(m_topToolbar, &TopToolbar::openImage,
                    m_mainWidget, &MainWidget::setImageInCanvas);
}

void MainWindow::resizeEvent(QResizeEvent *event) {

    DMainWindow::resizeEvent(event);
    m_topToolbar->setFixedWidth(this->width() -  m_titlebarWidth);

    this->update();
}

MainWindow::~MainWindow() {

}
