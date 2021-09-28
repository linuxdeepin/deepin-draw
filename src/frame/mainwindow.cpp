/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
#include <QDesktopWidget>
#include <QProcess>
#include <QStandardPaths>
#include <QSettings>
#include <QScrollBar>
#include <QMimeData>


static void notifySystemBlocked(bool block)
{
    static QDBusReply<QDBusUnixFileDescriptor> m_reply;
    static QDBusInterface *m_pLoginManager = nullptr;
    static QList<QVariant> m_arg;

    if (m_arg.isEmpty()) {

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

    if (block) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
    } else {
        m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    }
}

MainWindow::MainWindow(QStringList filePaths)
{
    initUI();
    initConnection();
    setAcceptDrops(true);

    if (filePaths.isEmpty()) {
        drawBoard()->addPage("");
    } else {
        QMetaObject::invokeMethod(this, [ = ]() {
            int pictureCount = 0;
            foreach (auto path, filePaths) {
                QFileInfo info(path);
                auto stuff = info.suffix().toLower();
                if (FileHander::supPictureSuffix().contains(stuff)) {
                    pictureCount ++;
                }
            }

            int ret = drawApp->execPicturesLimit(pictureCount);
            if (ret == 0) {


                bool b = openFiles(filePaths, true);
                if (!b) {
                    drawApp->quitApp();
                }
                else
                {
                    QMetaObject::invokeMethod(this, [ = ]() {
                        m_drawBoard->currentPage()->adjustViewScaleRatio(filePaths);
                    }, Qt::QueuedConnection);
                }

            }

        }, Qt::QueuedConnection);
    }
}

void MainWindow::initUI()
{
    m_drawBoard = new DrawBoard(this);
    m_drawBoard->setAutoClose(true);
    setWgtAccesibleName(this, "MainWindow");
    drawApp->setWidgetAllPosterityNoFocus(titlebar());
    setWindowTitle(tr("Draw"));
    //根据屏幕分辨率进行最小化窗口的设置
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    int screenWidth;
    screenWidth = screenRect.width();
    if (screenWidth < 1152) {
        setMinimumSize(QSize(1024, 768));
    } else {
        setMinimumSize(QSize(1152, 768));
    }

    if (drawApp->isTabletSystemEnvir())
        setMinimumWidth(1080);

    m_drawBoard->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_drawBoard);

    m_topToolbar = new TopTilte(titlebar());

    m_topToolbar->setFrameShape(DFrame::NoFrame);

    titlebar()->installEventFilter(this);
    titlebar()->setTitle("");
    titlebar()->setMenu(m_topToolbar->mainMenu());
    //titlebar()->setFocusPolicy(Qt::NoFocus);
    titlebar()->setIcon(QIcon(":/theme/common/images/logo.svg"));

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
    connect(m_drawBoard, &DrawBoard::modified, this, [ = ](bool modified) {
        notifySystemBlocked(modified);
    });
    connect(m_drawBoard, &DrawBoard::toClose, drawApp, &Application::quitApp);

    connect(m_drawBoard, &DrawBoard::zoomValueChanged, m_topToolbar, &TopTilte::slotSetScale);
    connect(m_topToolbar, &TopTilte::zoomTo, m_drawBoard, [ = ](qreal total) {
        QSignalBlocker blocker(m_drawBoard);
        m_drawBoard->zoomTo(total);
    });

    connect(drawApp, &Application::quitRequest, this, [ = ] {
        if (drawBoard()->close())
        {
            drawApp->quitApp();
        }
    });

    connect(m_topToolbar, &TopTilte::toOpen, this, &MainWindow::slotShowOpenFileDialog);

    connect(m_quitMode, &QAction::triggered, this, &MainWindow::slotOnEscButtonClick);

    connect(m_showCut, &QAction::triggered, this, &MainWindow::onViewShortcut);

    connect(m_topToolbar, &TopTilte::toPrint, m_drawBoard, [ = ]() {});

    connect(m_topToolbar, &TopTilte::creatOnePage, m_drawBoard, [ = ]() {
        m_drawBoard->addPage();
    });

    connect(m_topToolbar, &TopTilte::toExport, this, [ = ]() {
        if (m_drawBoard->currentPage() != nullptr) {
            auto currentTool = m_drawBoard->currentTool_p();

            bool refuse = currentTool != nullptr ? currentTool->blockPageBeforeOutput(m_drawBoard->currentPage()) : false;
            if (refuse) {
                return;
            }

            CExportImageDialog dialog(m_drawBoard);
            int ret = dialog.execFor(m_drawBoard->currentPage());
            if (ret != -1) {
                bool success = (ret == 1);

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
    nameFilters << "*.ddf *.png *.jpg *.bmp *.tif";
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    QStringList picturePathList;


    if (dialog.exec()) {
        QStringList tempfilePathList = dialog.selectedFiles();
        int ret = drawApp->execPicturesLimit(tempfilePathList.size());
        if (ret == 0) {
            foreach (auto file, tempfilePathList)
                drawBoard()->load(file);
        }
    }
}


void MainWindow::onViewShortcut()
{
    QRect rect = window()->geometry();
    QPoint pos(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
    Shortcut sc;
    QStringList shortcutString;
    QString param1 = "-j=" + sc.toStr();
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());
    shortcutString << "-b" << param1 << param2;

    QProcess *shortcutViewProc = new QProcess(this);
    shortcutViewProc->startDetached("killall deepin-shortcut-viewer");
    shortcutViewProc->startDetached("deepin-shortcut-viewer", shortcutString);

    connect(shortcutViewProc, SIGNAL(finished(int)), shortcutViewProc, SLOT(deleteLater()));
}

void MainWindow::slotOnEscButtonClick()
{
    drawApp->setCurrentTool(selection);
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
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;
    }
    DWidget::dragMoveEvent(event);
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls()) {
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
            foreach (auto url, urls) {
                QString filePath = url.path();
                if (!filePath.isEmpty()) {
                    drawBoard()->load(filePath);
                }
            }
        }
    }
    DWidget::dropEvent(e);
}

void MainWindow::readSettings()
{
    QString fileName = Global::configPath() + "/config.conf";
    QSettings settings(fileName, QSettings::IniFormat);

    // [0] judge is first load draw process
    bool opened = settings.value("opened").toBool();
    if (!opened) {
        //Dtk::Widget::moveToCenter(this);
        //修复初次装机，画板不能还原窗口
        int w = dApp->desktop()->screenGeometry().width() / 2;
        int h = dApp->desktop()->screenGeometry().height() / 2 ;
        resize(w, h);
        this->showMaximized();
    } else {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());
    }
    QVariant var = settings.value("EnchValue");
    if (var.isValid()) {
        int value = var.toInt();
        if (value >= 0 && value <= 100)
            drawBoard()->setTouchFeelingEnhanceValue(var.toInt());
    }
    qDebug() << "touchFeelingEnhanceValue ============ " << drawBoard()->touchFeelingEnhanceValue();
}

bool MainWindow::openFiles(QStringList filePaths, bool bAdapt)
{
    bool loaded = false;
    foreach (auto path, filePaths) {
        if (path.isEmpty())
            return false;

        bool loadThisRet = false;
        QFileInfo info(path);
        auto stuffix = info.suffix();
        if (FileHander::supDdfStuffix().contains(stuffix)) {
            loadThisRet = drawBoard()->loadDDf(path);
        } else {
            loadThisRet = drawBoard()->loadImage(path, false, bAdapt);
        }

        if (loadThisRet) {
            loaded = true;
        }
    }

    return loaded;
}

//bool MainWindow::openImage(QImage image, const QByteArray &srcData)
//{
//    if (QPixmap::fromImage(image).isNull()) {
//        return false;
//    } else {
//        m_centralWidget->slotPastePixmap(QPixmap::fromImage(image), srcData, true, true);
//        return true;
//    }
//}

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
    foreach (auto tool, CDrawToolFactory::allTools()) {
        delete tool;
    }
    CDrawToolFactory::allTools().clear();
    CUndoRedoCommand::clearCommand();
}
