#include "mainwindow.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>

#include <DTitlebar>

const QSize WINDOW_MINISIZR = QSize(800, 500);

MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent)
{
    window()->setWindowState(Qt::WindowMaximized);
    setMinimumSize(WINDOW_MINISIZR);

    m_topToolbar = new TopToolbar(this);
    m_titlebarWidth = titlebar()->buttonAreaWidth();
    m_topToolbar->setFixedWidth(width() - m_titlebarWidth);

    titlebar()->setCustomWidget(m_topToolbar, Qt::AlignLeft);
    titlebar()->setMenu(m_topToolbar->mainMenu());


    m_mainWidget = new MainWidget(this);
    m_mainWidget->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_mainWidget);

    connect(m_topToolbar, &TopToolbar::drawShapeChanged,
            m_mainWidget, &MainWidget::drawShapeChanged);
    connect(m_topToolbar, &TopToolbar::fillShapeSelectedActive,
            m_mainWidget, &MainWidget::fillShapeSelectedActive);

    connect(m_topToolbar, &TopToolbar::openImage,
            m_mainWidget, &MainWidget::setImageInCanvas);

    connect(m_topToolbar, &TopToolbar::rotateImage,
            m_mainWidget, &MainWidget::rotateImage);

    connect(m_topToolbar, &TopToolbar::mirroredImage,
            m_mainWidget, &MainWidget::mirroredImage);

    connect(m_topToolbar, &TopToolbar::saveImage,
            m_mainWidget, &MainWidget::saveImage);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_topToolbar->setFixedWidth(this->width() -  m_titlebarWidth);

    DMainWindow::resizeEvent(event);
    this->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    DMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow()
{
}
