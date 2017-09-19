#include "mainwindow.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>

#include <DTitlebar>

MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent)
{
    setMinimumSize(1050, 850);

    m_topToolbar = new TopToolbar(this);

    titlebar()->setCustomWidget(m_topToolbar, Qt::AlignCenter);
    titlebar()->setMenu(m_topToolbar->mainMenu());

    m_titlebarWidth = titlebar()->width();
    m_topToolbar->setFixedWidth(this->width() - m_titlebarWidth);

    m_mainWidget = new MainWidget(this);
    m_mainWidget->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_mainWidget);

    connect(m_topToolbar, &TopToolbar::openImage,
            m_mainWidget, &MainWidget::setImageInCanvas);

    connect(m_topToolbar, &TopToolbar::initShapeWidgetAction,
            m_mainWidget, &MainWidget::initShapesWidget);

    connect(m_topToolbar, &TopToolbar::rotateImage,
            m_mainWidget, &MainWidget::rotateImage);

    connect(m_topToolbar, &TopToolbar::mirroredImage,
            m_mainWidget, &MainWidget::mirroredImage);

    connect(m_topToolbar, &TopToolbar::saveImage,
            m_mainWidget, &MainWidget::saveImage);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);
    m_topToolbar->setFixedWidth(this->width() -  m_titlebarWidth);

    this->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    DMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow() {
}
