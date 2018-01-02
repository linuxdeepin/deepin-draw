#include "mainwindow.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>

#include "utils/configsettings.h"
#include "utils/drawfile.h"
#include "utils/tempfile.h"
#include "widgets/dialog/drawdialog.h"
#include "../application.h"

#include <DTitlebar>

const QSize WINDOW_MINISIZR = QSize(760, 540);
const int ARTBOARD_MARGIN = 25;
const int TITLEBAR_HEIGHT = 40;

MainWindow::MainWindow(QWidget *parent)
    :DMainWindow(parent)
{
    setMouseTracking(true);
    window()->setWindowState(Qt::WindowMaximized);
    QSize desktopSize = qApp->desktop()->size();
    ConfigSettings::instance()->setValue("artboard", "width",  desktopSize.width());
    ConfigSettings::instance()->setValue("artboard", "height", desktopSize.height());

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
    connect(m_topToolbar, &TopToolbar::rotateImage,
            m_mainWidget, &MainWidget::rotateImage);
    connect(m_topToolbar, &TopToolbar::mirroredImage,
            m_mainWidget, &MainWidget::mirroredImage);
    connect(m_topToolbar, &TopToolbar::generateSaveImage,
            m_mainWidget, &MainWidget::generateSaveImage);
    connect(m_topToolbar, &TopToolbar::printImage,
            m_mainWidget, &MainWidget::printImage);
    connect(m_topToolbar, &TopToolbar::autoCrop,
            m_mainWidget, &MainWidget::autoCrop);

    connect(m_mainWidget, &MainWidget::updateMiddleWidget,
            m_topToolbar, &TopToolbar::updateMiddleWidget);
    connect(m_mainWidget, &MainWidget::adjustArtBoardSize,
            m_topToolbar, &TopToolbar::adjustArtBoardSize);
    connect(m_mainWidget, &MainWidget::cutImageFinished,
            m_topToolbar, &TopToolbar::cutImageFinished);
    connect(m_mainWidget, &MainWidget::shapePressed,
            m_topToolbar, &TopToolbar::updateCurrentShape);
    connect(dApp, &Application::popupConfirmDialog, this, [=]{
        if (m_mainWidget->shapeNum() != 0)
        {
            showDrawDialog();
        } else {
            dApp->quit();
        }
    });
}

void MainWindow::openImage(const QString &path)
{
    QSize imageSize = QPixmap(path).size();
    QSize desktopSize = qApp->desktop()->size();
    int ww = desktopSize.width() - 2*ARTBOARD_MARGIN;
    int wh = desktopSize.height() - 2*ARTBOARD_MARGIN - TITLEBAR_HEIGHT;

    if (imageSize.width() > ww || imageSize.height() > wh)
    {
        resize(desktopSize.width(), desktopSize.height());
    } else {
        int imgW = imageSize.width() + 2*ARTBOARD_MARGIN;
        int imgH = imageSize.height() + 2*ARTBOARD_MARGIN + TITLEBAR_HEIGHT;

       ConfigSettings::instance()->setValue("artboard", "width",  imageSize.width());
       ConfigSettings::instance()->setValue("artboard", "height", imageSize.height());

        resize(imgW, imgH);
    }

    m_mainWidget->openImage(path);
}

void MainWindow::parseDdf(const QString &path)
{
    DrawFile* dFile = new DrawFile(this);
    dFile->parseddf(path);
    QSize windowSize = dFile->windowSize();
    resize(windowSize);
    QSize canvasSize = dFile->canvasSize();
    m_mainWidget->updateCanvasSize(canvasSize);
    QSize artboardSize = dFile->artboardSize();
    ConfigSettings::instance()->setValue("artboard", "width",  artboardSize.width());
    ConfigSettings::instance()->setValue("artboard", "height", artboardSize.height());
    m_mainWidget->initShapes(dFile->toolshapes());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_topToolbar->setFixedWidth(this->width() -  m_titlebarWidth);

    int ww = window()->width();
    int wh = window()->height();

    ConfigSettings::instance()->setValue("window", "width", ww);
    ConfigSettings::instance()->setValue("window", "height", wh);

    DMainWindow::resizeEvent(event);
    this->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    DMainWindow::keyPressEvent(event);
    qDebug() << "MainWindow:" << event->key();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_topToolbar->updateColorPanelVisible(event->pos());
    emit m_mainWidget->pressToCanvas(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit dApp->popupConfirmDialog();
    event->ignore();
}

void MainWindow::showDrawDialog()
{
    DrawDialog*  dd = new DrawDialog(this);
    dd->showInCenter(window());

    connect(dd, &DrawDialog::saveDrawImage, this, [=]{
        TempFile::instance()->setSaveFinishedExit(true);
        emit m_topToolbar->generateSaveImage();
    });
    dd->exec();
}

MainWindow::~MainWindow()
{
}
