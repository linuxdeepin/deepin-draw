#include "mainwindow.h"

#include <QVBoxLayout>

#include <DTitlebar>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent) {
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

    connect(m_topToolbar, &TopToolbar::shapesColorChanged,
            m_mainWidget, &MainWidget::shapesColorChanged);

    connect(m_topToolbar, &TopToolbar::shapesLineWidthChanged,
            m_mainWidget, &MainWidget::shapesLineWidthChanged);

    connect(m_topToolbar, &TopToolbar::lineShapeChanged,
            m_mainWidget, &MainWidget::lineShapeChanged);

     connect(m_topToolbar, &TopToolbar::textFontsizeChanged,
             m_mainWidget, &MainWidget::textFontsizeChanged);

     connect(m_topToolbar, &TopToolbar::blurLineWidthChanged,
             m_mainWidget, &MainWidget::blurLinewidthChanged);

     connect(m_topToolbar, &TopToolbar::rotateImage,
             m_mainWidget, &MainWidget::rotateImage);

     connect(m_topToolbar, &TopToolbar::mirroredImage,
             m_mainWidget, &MainWidget::mirroredImage);
     connect(m_topToolbar, &TopToolbar::cutImage,
             m_mainWidget, &MainWidget::cutImage);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    DMainWindow::resizeEvent(event);
    m_topToolbar->setFixedWidth(this->width() -  m_titlebarWidth);

    this->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    DMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow() {
}
