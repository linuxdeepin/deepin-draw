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
#include "drawshape/cdrawparamsigleton.h"



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

    //链接图片选择后相应的操作
    connect(m_topToolbar, SIGNAL(pictureMirror(bool, bool)), m_centralWidget, SIGNAL(picMirrorWidget(bool, bool)));
    connect(m_topToolbar, SIGNAL(pictureRotate(bool)), m_centralWidget, SIGNAL(picRotateWidget( bool)));

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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CDrawParamSigleton::GetInstance()->setShiftKeyStatus(true);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CDrawParamSigleton::GetInstance()->setAltKeyStatus(true);
    } else {
        ;
    }
    DMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CDrawParamSigleton::GetInstance()->setShiftKeyStatus(false);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CDrawParamSigleton::GetInstance()->setAltKeyStatus(false);
    } else {
        ;
    }
    DMainWindow::keyReleaseEvent(event);
}

MainWindow::~MainWindow()
{
}
