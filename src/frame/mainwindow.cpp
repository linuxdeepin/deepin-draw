// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mainwindow.h"
#include "widgets/dialog/drawdialog.h"
#include "../application.h"
#include "ccentralwidget.h"
#include "toptoolbar.h"
#include "clefttoolbar.h"
#include "drawshape/cdrawparamsigleton.h"
#include "cgraphicsview.h"
#include "drawshape/cdrawscene.h"
#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"
#include "utils/shortcut.h"
#include "utils/global.h"
#include "cmultiptabbarwidget.h"
#include "cdrawtoolmanagersigleton.h"
#include "cundoredocommand.h"
#include "cdrawtoolfactory.h"
#include "filehander.h"
#include "cexportimagedialog.h"

#include <DTitlebar>
#include <DFileDialog>
#include <DMessageManager>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>

#if (QT_VERSION_MAJOR == 5)
#include <QDesktopWidget>
#endif

#include <QProcess>
#include <QStandardPaths>
#include <QSettings>
#include <QScrollBar>
#include <QMimeData>
#include <QtConcurrent>


static void notifySystemBlocked(bool block)
{
    static QDBusReply<QDBusUnixFileDescriptor> m_reply;
    static QDBusInterface *m_pLoginManager = nullptr;
    static QList<QVariant> m_arg;

    if (m_arg.isEmpty()) {
        qDebug() << "Initializing system shutdown blocking";
        m_pLoginManager = new QDBusInterface("org.freedesktop.login1",
                                             "/org/freedesktop/login1",
                                             "org.freedesktop.login1.Manager",
                                             QDBusConnection::systemBus());

        m_arg << QString("shutdown")                // what
              << qApp->productName()                // who
              << QObject::tr("File not saved")      // why
              << QString("block");                  // mode

        m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);

        (void)m_reply.value().fileDescriptor();
    }

    if (!block) {
        qDebug() << "Unblocking system shutdown";
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
    } else {
        qDebug() << "Blocking system shutdown";
        m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);//阻止关机
    }
}

MainWindow::MainWindow(QStringList filePaths)
{
    qDebug() << "Initializing MainWindow";
    initUI();
    initConnection();
    setAcceptDrops(true);
}

void MainWindow::initUI()
{
    qDebug() << "Setting up MainWindow UI";
    m_drawBoard = new DrawBoard(this);
    m_drawBoard->setAutoClose(true);
    setWgtAccesibleName(this, "MainWindow");
    drawApp->setWidgetAllPosterityNoFocus(titlebar());
    setWindowTitle(tr("Draw"));
    //根据屏幕分辨率进行最小化窗口的设置
//    QDesktopWidget *desktopWidget = QApplication::desktop();
//    QRect screenRect = desktopWidget->screenGeometry();
//    int screenWidth;
//    screenWidth = screenRect.width();

    setMinimumSize(QSize(600, 300));
//    if (screenWidth < 1152) {
//        setMinimumSize(QSize(1024, 768));
//    } else {
//        setMinimumSize(QSize(1152, 768));
//    }

    if (drawApp->isTabletSystemEnvir()) {
        qDebug() << "Tablet environment detected, setting minimum width to 1080";
        setMinimumWidth(1080);
    }

    m_drawBoard->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_drawBoard);

    m_topToolbar = new TopTilte(titlebar());

    m_topToolbar->setFrameShape(DFrame::NoFrame);

    titlebar()->installEventFilter(this);
    titlebar()->setTitle("");
    titlebar()->setMenu(m_topToolbar->mainMenu());
    //titlebar()->setFocusPolicy(Qt::NoFocus);
    titlebar()->setIcon(QIcon::fromTheme("deepin-draw"));

    //ESC快捷键功能
    m_quitMode = new QAction(this);
    m_quitMode->setShortcut(QKeySequence(Qt::Key_Escape));
    this->addAction(m_quitMode);

    m_showCut = new QAction(this);
    m_showCut->setObjectName("shortCutManPannel");
    m_showCut->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
    this->addAction(m_showCut);
}

void MainWindow::initConnection()
{
    qDebug() << "Setting up MainWindow connections";
    connect(m_drawBoard, &DrawBoard::currentTitleNameChange, this, &MainWindow::setWindowTitleInfo);

    connect(m_drawBoard, &DrawBoard::modified, this, [ = ](bool modified) {
        qDebug() << "Draw board modification state changed:" << modified;
        notifySystemBlocked(modified);
    });
    connect(m_drawBoard, &DrawBoard::toClose, drawApp, &Application::quitApp);

    connect(m_drawBoard, &DrawBoard::zoomValueChanged, m_topToolbar, &TopTilte::slotSetScale);
    connect(m_topToolbar, &TopTilte::zoomTo, m_drawBoard, [ = ](qreal total) {
        qDebug() << "Zooming to scale:" << total;
        QSignalBlocker blocker(m_drawBoard);
        m_drawBoard->zoomTo(total);
    });

    connect(drawApp, &Application::quitRequest, this, [ = ] {
        qDebug() << "Application quit requested";
        if (drawBoard()->close()) {
            drawApp->quitApp();
        }
    });

    connect(m_topToolbar, &TopTilte::toOpen, this, &MainWindow::slotShowOpenFileDialog);

    connect(m_quitMode, &QAction::triggered, this, &MainWindow::slotOnEscButtonClick);

    connect(m_showCut, &QAction::triggered, this, &MainWindow::onViewShortcut);

    connect(m_topToolbar, &TopTilte::toPrint, m_drawBoard, [ = ]() {});

    connect(m_topToolbar, &TopTilte::creatOnePage, m_drawBoard, [ = ]() {
        qDebug() << "Creating new page";
        m_drawBoard->addPage();
    });

    connect(m_topToolbar, &TopTilte::toExport, this, [ = ]() {
        qDebug() << "Export action triggered";
        if (m_drawBoard->currentPage() != nullptr) {
            auto currentTool = m_drawBoard->currentTool_p();

            bool refuse = currentTool != nullptr ? currentTool->blockPageBeforeOutput(m_drawBoard->currentPage()) : false;
            if (refuse) {
                qDebug() << "Export blocked by current tool";
                return;
            }

            CExportImageDialog dialog(m_drawBoard);
            int ret = dialog.execFor(m_drawBoard->currentPage());
            if (ret != -1) {
                bool success = (ret == 1);
                qDebug() << "Export completed - success:" << success;

                if (pDFloatingMessage == nullptr) {
                    pDFloatingMessage = new DFloatingMessage(DFloatingMessage::MessageType::TransientType, drawApp->topMainWindow());
                }
                pDFloatingMessage->show();
                pDFloatingMessage->setBlurBackgroundEnabled(true);
                // Export success应改为Export successful
                pDFloatingMessage->setMessage(success ? tr("Export successful") : tr("Export failed"));
                pDFloatingMessage->setIcon(!success ? QIcon::fromTheme(":/icons/deepin/builtin/texts/warning_new_32px.svg") :
                                           QIcon(":/icons/deepin/builtin/texts/notify_success_32px.svg"));
                pDFloatingMessage->setDuration(2000); //set 2000ms to display it
                DMessageManager::instance()->sendMessage(drawApp->topMainWindow(), pDFloatingMessage);
            }
        }
    });

    m_drawBoard->setAttributionWidget(topTitle()->attributionsWgt());
}

void MainWindow::activeWindow()
{
    window()->show();
    window()->raise();
    window()->activateWindow();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);
    this->update();
}

void MainWindow::slotShowOpenFileDialog()
{
    qDebug() << "Showing open file dialog";
    DFileDialog dialog(this);
    dialog.setWindowTitle(tr("Open"));//设置文件保存对话框的标题
    dialog.setAcceptMode(QFileDialog::AcceptOpen);//设置文件对话框为保存模式
    dialog.setViewMode(DFileDialog::List);
    dialog.setFileMode(DFileDialog::ExistingFiles);
    if (Application::isTabletSystemEnvir())
        dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    else
        dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QStringList nameFilters;
    //nameFilters << "*.ddf *.png *.jpg *.bmp *.tif *.jpeg";
    auto formatsList = drawApp->readableFormats();
    auto formats = QString("(*.") + formatsList.join(" *.") + QString(")");
    nameFilters << formats;
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    dialog.setDirectory(drawApp->defaultFileDialogPath());
    QStringList picturePathList;


    if (dialog.exec()) {
        QStringList tempfilePathList = dialog.selectedFiles();
        qDebug() << "Selected files count:" << tempfilePathList.size();
        int ret = drawApp->execPicturesLimit(tempfilePathList.size());
        if (ret == 0) {
            drawBoard()->loadFiles(tempfilePathList);
        }
    }
}


void MainWindow::loadFiles(const QStringList &filePaths)
{
    qDebug() << "Loading files - count:" << filePaths.size();
    int pictureCount = 0;
    foreach (auto path, filePaths) {
        QFileInfo info(path);
        auto stuff = info.suffix().toLower();
        if (FileHander::supPictureSuffix().contains(stuff)) {
            pictureCount ++;
        }
    }

    qDebug() << "Picture files count:" << pictureCount;
    int ret = drawApp->execPicturesLimit(pictureCount);
    if (ret == 0) {
        m_drawBoard->loadFiles(filePaths, true, 0, true);
    }
}

void MainWindow::onViewShortcut()
{
    qDebug() << "Showing shortcut view";
    QRect rect = window()->geometry();
    QPoint pos(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
    Shortcut sc;
    QStringList shortcutString;
    QString param1 = "-j=" + sc.toStr();
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());
    shortcutString /*<< "-b"*/ << param1 << param2;

    QProcess *shortcutViewProc = new QProcess(this);
    //shortcutViewProc->startDetached("killall deepin-shortcut-viewer");
    shortcutViewProc->startDetached("deepin-shortcut-viewer", shortcutString);

    connect(shortcutViewProc, SIGNAL(finished(int)), shortcutViewProc, SLOT(deleteLater()));
}

void MainWindow::slotOnEscButtonClick()
{
    qDebug() << "ESC key pressed - switching to selection tool";
    drawApp->setCurrentTool(selection);
}

void MainWindow::setWindowTitleInfo()
{
    Page *current_page = m_drawBoard->currentPage();
    if (current_page) {
        qDebug() << "Setting window title for page:" << current_page->context()->file();
        if (current_page->context()->file() != "") {
            this->setWindowTitle(current_page->context()->file());
        } else {
            this->setWindowTitle(current_page->context()->name());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (event == nullptr)
        return;

//    QString fileName = Global::configPath() + "/config.conf";
//    QSettings settings(fileName, QSettings::IniFormat);
//    settings.setValue("geometry", saveGeometry());
//    settings.setValue("windowState", saveState());
//    settings.setValue("opened", "true");

    qDebug() << "Window closing - saving settings";
    drawApp->saveSettings();
    emit drawApp->quitRequest();
    event->ignore();

    closeEvent(nullptr);
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o->isWidgetType()) {
        if (titlebar() == o) {
            //实现m_topToolbar的正确位置放置
            if (e->type() == QEvent::Resize) {
                if (m_topToolbar != nullptr) {
                    qDebug() << "Resizing top toolbar";
                    m_topToolbar->setGeometry(60, 0, titlebar()->width() - 60 - 200, titlebar()->height());
                }
            }
        }
    }
    return DMainWindow::eventFilter(o, e);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        qDebug() << "Drag enter event with URLs";
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;
    }
    DWidget::dragMoveEvent(event);
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        qDebug() << "Drag move event with URLs";
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;
    }
    DWidget::dragMoveEvent(event);
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasText()) {
        QList<QUrl> urls  = e->mimeData()->urls();

        int ret = drawApp->execPicturesLimit(urls.count());

        if (ret == 0) {
            QStringList fileLists;
            foreach (auto url, urls) {
                QString filePath = url.path();
                if (!filePath.isEmpty()) {
                    fileLists.append(filePath);
                }
            }
            drawBoard()->loadFiles(fileLists);
        }
    }
    DWidget::dropEvent(e);
}

void MainWindow::readSettings()
{
//    QString fileName = Global::configPath() + "/config.conf";
//    QSettings settings(fileName, QSettings::IniFormat);

//    // [0] judge is first load draw process
//    bool opened = settings.value("opened").toBool();
//    if (!opened) {
//        //Dtk::Widget::moveToCenter(this);
//        //修复初次装机，画板不能还原窗口
//        int w = dApp->desktop()->screenGeometry().width() / 2;
//        int h = dApp->desktop()->screenGeometry().height() / 2 ;
//        resize(w, h);
//        this->showMaximized();
//    } else {
//        restoreGeometry(settings.value("geometry").toByteArray());
//        restoreState(settings.value("windowState").toByteArray());
//    }
//    QVariant var = settings.value("EnchValue");
//    if (var.isValid()) {
//        int value = var.toInt();
//        if (value >= 0 && value <= 100)
//            drawBoard()->setTouchFeelingEnhanceValue(var.toInt());
//    }
//    qDebug() << "touchFeelingEnhanceValue ============ " << drawBoard()->touchFeelingEnhanceValue();
    qDebug() << "Reading application settings";
    drawApp->readSettings();
}

bool MainWindow::openFiles(QStringList filePaths, bool bAdapt)
{
    qDebug() << "Opening files - count:" << filePaths.size() << "adapt:" << bAdapt;
    if (nullptr == m_drawBoard)
        return false;

    m_drawBoard->loadFiles(filePaths, true, bAdapt);
    return true;
}

DrawBoard *MainWindow::drawBoard() const
{
    return m_drawBoard;
}

TopTilte *MainWindow::topTitle() const
{
    return m_topToolbar;
}


MainWindow::~MainWindow()
{
    qDebug() << "Cleaning up MainWindow";
    foreach (auto tool, CDrawToolFactory::allTools()) {
        delete tool;
    }
    CDrawToolFactory::allTools().clear();
    CUndoRedoCommand::clearCommand();
}
