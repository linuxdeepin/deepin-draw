#include "mainwindow.h"

#include <DTitlebar>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>

#include "widgets/dialog/drawdialog.h"
#include "../application.h"
#include "ccentralwidget.h"
#include "toptoolbar.h"
#include "clefttoolbar.h"



const QSize WINDOW_MINISIZR = QSize(960, 540);
//const int ARTBOARD_MARGIN = 25;
//const int TITLEBAR_HEIGHT = 40;
//const int IMG_ROTATEPOINT_SPACING = 35;

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
//    setMouseTracking(true);

    initUI();
    initConnection();
}

void MainWindow::initUI()
{
    window()->setWindowState(Qt::WindowMaximized);

    setMinimumSize(WINDOW_MINISIZR);
    m_topToolbar = new TopToolbar(this);

    m_titlebarWidth = titlebar()->buttonAreaWidth();
    m_topToolbar->setFixedWidth(width());

    titlebar()->addWidget(m_topToolbar, Qt::AlignHCenter);
    titlebar()->setMenu(m_topToolbar->mainMenu());

//    titlebar()->setStyleSheet("background-color: rgb(0, 255, 0);");


    m_centralWidget = new CCentralwidget(this);
    m_centralWidget->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_centralWidget);
}

void MainWindow::initConnection()
{
    connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, m_topToolbar, &TopToolbar::updateMiddleWidget);
    connect(this, &MainWindow::signalResetOriginPoint, m_centralWidget, &CCentralwidget::slotResetOriginPoint);
    connect(dApp, &Application::popupConfirmDialog, this, [ = ] {
//        if (m_mainWidget->shapeNum() != 0)
//        {
        showDrawDialog();
//        } else {
//            dApp->quit();
//        }
    });
    connect(m_topToolbar, SIGNAL(signalAttributeChanged()), m_centralWidget, SLOT(slotAttributeChanged()));
    connect(m_centralWidget, &CCentralwidget::signalAttributeChangedFromScene, m_topToolbar, &TopToolbar::slotChangeAttributeFromScene);

}


void MainWindow::activeWindow()
{
    window()->show();
    window()->raise();
    window()->activateWindow();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_topToolbar->setFixedWidth(this->width());

    int ww = window()->width();
    int wh = window()->height();

//    ConfigSettings::instance()->setValue("window", "width", ww);
//    ConfigSettings::instance()->setValue("window", "height", wh);
    emit signalResetOriginPoint();

    DMainWindow::resizeEvent(event);
    this->update();
}


void MainWindow::showDrawDialog()
{
    DrawDialog  *dd = new DrawDialog(this);
    dd->showInCenter(window());

//    connect(dd, &DrawDialog::saveDrawImage, this, [ = ] {
//        TempFile::instance()->setSaveFinishedExit(true);
//        emit m_topToolbar->generateSaveImage();
//    });
    dd->exec();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_topToolbar->updateColorPanelVisible(event->pos());
    DMainWindow::mousePressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit dApp->popupConfirmDialog();
    event->ignore();
}


MainWindow::~MainWindow()
{
}
