/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     WangXin
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

#include <DTitlebar>
#include <DFileDialog>

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


//const QSize WINDOW_MINISIZR = QSize(1280, 800);

//const QSize WINDOW_MINISIZR = QSize(1024, 768);

const int TITLBAR_MENU = 150;
//const int TITLEBAR_HEIGHT = 40;
//const int IMG_ROTATEPOINT_SPACING = 35;

MainWindow::MainWindow(DWidget *parent)
    : DMainWindow(parent)
{
//    setMouseTracking(true);

    m_centralWidget = new CCentralwidget(this);

    initUI();
    initConnection();
}

MainWindow::MainWindow(QStringList filePaths)
{
    m_centralWidget = new CCentralwidget(filePaths);

    initUI();
    initConnection();
}


void MainWindow::initUI()
{
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

    m_centralWidget->setFocusPolicy(Qt::StrongFocus);
    setContentsMargins(QMargins(0, 0, 0, 0));
    setCentralWidget(m_centralWidget);

    m_topToolbar = new TopToolbar(this);
    m_topToolbar->setFrameShape(DFrame::NoFrame);
    m_topToolbar->setFixedWidth(width() - TITLBAR_MENU);
    m_topToolbar->setFixedHeight(titlebar()->height());
    //qDebug() << "titlebar()->height()" << titlebar()->height() << endl;

//    titlebar()->setIcon(QIcon (QPixmap(":/theme/common/images/logo.svg").scaled(QSize(32, 32))));
    titlebar()->addWidget(m_topToolbar, Qt::AlignLeft);
    titlebar()->setMenu(m_topToolbar->mainMenu());
    titlebar()->setFocusPolicy(Qt::NoFocus);

//    titlebar()->setStyleSheet("background-color: rgb(0, 255, 0);");

    //ESC快捷键功能
    m_quitMode = new QAction(this);
    m_quitMode->setShortcut(QKeySequence(Qt::Key_Escape));
    this->addAction(m_quitMode);

    m_showCut = new QAction(this);
    m_showCut->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
    this->addAction(m_showCut);

    // 标签关闭提示框
    m_dialog.setIconPixmap(QPixmap(":/theme/common/images/deepin-draw-64.svg"));
    m_dialog.setMessage(tr("Is Close Draw?"));
    m_dialog.addButton(tr("OK"), true, DDialog::ButtonNormal);
    m_dialog.addButton(tr("Cancel"), false, DDialog::ButtonNormal);
    connect(&m_dialog, &DDialog::buttonClicked, this, [ = ](int id) {
        if (0 == id) {
            qApp->quit();
        }
    });
}

void MainWindow::showDragOrOpenFile(QStringList files, bool isOPenFile)
{
    QString ddfPath = "";
    QStringList picturePathList;
    for (int i = 0; i < files.size(); i++) {
        //if (tempfilePathList[i].endsWith(".ddf")) {
        if (QFileInfo(files[i]).suffix().toLower() == ("ddf")) {
            ddfPath = files[i].replace("file://", "");
            if (!ddfPath.isEmpty()) {
                // 创建一个新的窗口用于显示拖拽的图像
                m_centralWidget->createNewScenseByscencePath(ddfPath);

                if (isOPenFile) {
                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::StartByDDF);
                } else {
                    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
                }

                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath(ddfPath);
                slotIsNeedSave();
            }
        } else if (files[i].endsWith(".png") || files[i].endsWith(".jpg")
                   || files[i].endsWith(".bmp") || files[i].endsWith(".tif") ) {
            //图片格式："*.png *.jpg *.bmp *.tif"
            picturePathList.append(files[i].replace("file://", ""));
        }
    }

    if (cut == CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode()) {
        m_centralWidget->getGraphicsView()->slotQuitCutMode();
    }

    if (picturePathList.count() > 0) {
        m_centralWidget->slotPastePicture(picturePathList);
    }
}

void MainWindow::showSaveQuestionDialog()
{
    DrawDialog quitQuestionDialog;
    connect(&quitQuestionDialog, &DrawDialog::signalSaveToDDF, this, [ = ]() {
        m_centralWidget->slotSaveToDDF(true);
    });

    connect(&quitQuestionDialog, &DrawDialog::singalDoNotSaveToDDF, this, [ = ]() {
        m_centralWidget->slotDoNotSaveToDDF();
        doCloseOtherDiv();
    });

    quitQuestionDialog.exec();
}

void MainWindow::doCloseOtherDiv()
{
//    qDebug() << "views:" << m_closeViews;

    // 此函数的作用是关闭 m_closeTabList 中的标签
    // 需要每次在保存或者不保存后进行调用判断
    int count = m_closeViews.size();
    for (int i = 0; i < count; i++) {
        QString current_name = m_closeViews.first();
        m_closeViews.removeFirst();
        m_centralWidget->setCurrentView(current_name);
        CManageViewSigleton::GetInstance()->setCurView(CManageViewSigleton::GetInstance()->getViewByViewName(current_name));
        CGraphicsView *closeView = CManageViewSigleton::GetInstance()->getViewByViewName(current_name);
        if (closeView == nullptr) {
            qDebug() << "close error view:" << current_name;
            continue;
        } else {
            bool editFlag = closeView->getDrawParam()->getModify();
//            qDebug() << "Close Edit TabBar:" << current_name << editFlag;
            if (editFlag) {
                showSaveQuestionDialog();
                break;
            } else {
                m_centralWidget->closeCurrentScenseView();
            }
        }
    }
}

void MainWindow::initConnection()
{
    connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, m_topToolbar, &TopToolbar::updateMiddleWidget);
    connect(m_centralWidget->getLeftToolBar(), &CLeftToolBar::setCurrentDrawTool, m_topToolbar, &TopToolbar::slotHideColorPanel);

    connect(this, &MainWindow::signalResetOriginPoint, m_centralWidget, &CCentralwidget::slotResetOriginPoint);
    connect(dApp, &Application::popupConfirmDialog, this, [ = ] {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::QuitApp);
        // 关闭所有标签
        QStringList divs = m_centralWidget->getAllTabBarName();
        if (divs.count())
        {
            m_closeViews = divs;
            doCloseOtherDiv();
        }
    });
    connect(m_topToolbar, SIGNAL(signalAttributeChanged()), m_centralWidget, SLOT(slotAttributeChanged()));
    connect(m_topToolbar, SIGNAL(signalTextFontFamilyChanged()), m_centralWidget, SLOT(slotTextFontFamilyChanged()));
    connect(m_topToolbar, SIGNAL(signalTextFontSizeChanged()), m_centralWidget, SLOT(slotTextFontSizeChanged()));

    connect(m_centralWidget, &CCentralwidget::signalAttributeChangedFromScene, m_topToolbar, &TopToolbar::slotChangeAttributeFromScene);

    //链接图片选择后相应的操作
    connect(m_topToolbar, SIGNAL(signalPassPictureOperation(int)), m_centralWidget, SIGNAL(signalPassPictureOper(int)));


    connect(m_topToolbar, SIGNAL(signalZoom(qreal)), m_centralWidget, SLOT(slotZoom(qreal)));
    connect(m_centralWidget, SIGNAL(signalSetScale(qreal)), m_topToolbar, SLOT(slotSetScale(qreal)));

    connect(m_topToolbar, SIGNAL(signalShowExportDialog()), m_centralWidget, SLOT(slotShowExportDialog()));

    connect(m_topToolbar, SIGNAL(signalPrint()), m_centralWidget, SLOT(slotPrint()));

    connect(m_topToolbar, SIGNAL(signalNew()), this, SLOT(slotNewDrawScence()));

    connect(m_centralWidget, SIGNAL(signalUpdateCutSize()), m_topToolbar, SLOT(slotSetCutSize()));

    connect(m_topToolbar, SIGNAL(signalSaveToDDF()), this, SLOT(slotTopToolBarSaveToDDF()));

    connect(m_topToolbar, SIGNAL(signalSaveAs()), m_centralWidget, SLOT(slotSaveAs()));

    connect(m_topToolbar, SIGNAL(signalImport()), this, SLOT(slotShowOpenFileDialog()));

//    connect(m_quitQuestionDialog, SIGNAL(singalDoNotSaveToDDF()), this, SLOT(slotContinueDoSomeThing()));

    connect(m_centralWidget, SIGNAL(signalUpdateTextFont()), m_topToolbar, SLOT(slotSetTextFont()));

    connect(m_centralWidget, SIGNAL(signalContinueDoOtherThing()), this, SLOT(slotContinueDoSomeThing()));

    connect(m_quitMode, SIGNAL(triggered()), this, SLOT(slotOnEscButtonClick()));

    connect(m_topToolbar, SIGNAL(signalQuitCutModeFromTopBarMenu()), m_centralWidget, SIGNAL(signalTransmitQuitCutModeFromTopBarMenu()));

    connect(m_topToolbar, SIGNAL(signalCutLineEditIsfocus(bool)), m_centralWidget, SLOT(slotCutLineEditeFocusChange(bool)));

    connect(m_showCut, SIGNAL(triggered()), this, SLOT(onViewShortcut()));

    //每次添加标签需要连接
//    if (nullptr != CManageViewSigleton::GetInstance()->getCurView()->scene()) {
//        auto curScene = static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene());
//        connect(curScene, SIGNAL(signalUpdateColorPanelVisible(QPoint)), m_topToolbar, SLOT(updateColorPanelVisible(QPoint)));
//    }

    connect(m_centralWidget, SIGNAL(signalTransmitLoadDragOrPasteFile(QStringList)), this, SLOT(slotLoadDragOrPasteFile(QStringList)));

    // 有新的场景创建后需要都进行连接的信号
    connect(m_centralWidget, &CCentralwidget::signalAddNewScence, this, [ = ](CDrawScene * sence) {
        connect(sence, SIGNAL(signalUpdateColorPanelVisible(QPoint)), m_topToolbar, SLOT(updateColorPanelVisible(QPoint)));
    });

    // 关闭当前窗口提示是否需要进行保存
    connect(m_centralWidget, &CCentralwidget::signalCloseModifyScence, this, &MainWindow::slotIsNeedSave);

    // 连接ddf文件已经被打开信号
    connect(m_centralWidget, &CCentralwidget::signalDDFFileOpened, this, &MainWindow::slotDDFFileOpened);

    // 连接最后一个标签被关闭
    connect(m_centralWidget, &CCentralwidget::signalLastTabBarRequestClose, this, &MainWindow::slotLastTabBarRequestClose);

    // 连接需要关闭多个标签信号
    connect(m_centralWidget, &CCentralwidget::signalTabItemsCloseRequested, this, [ = ](QStringList views) {
        m_closeViews = views;
        doCloseOtherDiv();
    });

    // 连接文件保存状态信号
    connect(m_centralWidget, &CCentralwidget::signalSaveFileStatus, this, [ = ](bool status) {
        qDebug() << "save status:" << status;
        doCloseOtherDiv();
    });
}

void MainWindow::activeWindow()
{
    window()->show();
    window()->raise();
    window()->activateWindow();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_topToolbar->setFixedWidth(width() - TITLBAR_MENU);

    emit signalResetOriginPoint();

    DMainWindow::resizeEvent(event);
    this->update();
}

void MainWindow::slotIsNeedSave()
{
    if (CManageViewSigleton::GetInstance()->getCurView()->getModify()) {
        // 此处需要进行适当延时显示才不会出错
        QMetaObject::invokeMethod(this, [ = ]() {
            showSaveQuestionDialog();
        }, Qt::QueuedConnection);

//        QTimer::singleShot(100,this,[=](){
//            showSaveQuestionDialog();
//        });
    } else {
        slotContinueDoSomeThing();
    }
}

void MainWindow::slotContinueDoSomeThing()
{
    ESaveDDFTriggerAction triggerType =  CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSaveDDFTriggerAction();
    switch (triggerType) {
    case QuitApp:
        qApp->quit();
        break;
    case LoadDDF:
        m_centralWidget->getGraphicsView()->importData(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getDdfSavePath());
        break;
    case StartByDDF:
        m_centralWidget->getGraphicsView()->importData(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getDdfSavePath(), true);
        break;
    case NewDrawingBoard:
        m_centralWidget->slotNew();
        break;
    case ImportPictrue:
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath("");
        m_centralWidget->getGraphicsView()->clearScene();
        m_centralWidget->getLeftToolBar()->slotShortCutSelect();
        m_centralWidget->openPicture(tmpPictruePath);
        break;
    default:
        break;
    }
}

void MainWindow::slotShowOpenFileDialog()
{
    DFileDialog dialog(this);
    dialog.setWindowTitle(tr("Open"));//设置文件保存对话框的标题
    dialog.setAcceptMode(QFileDialog::AcceptOpen);//设置文件对话框为保存模式
    dialog.setViewMode(DFileDialog::List);
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    //dialog.set
    QStringList nameFilters;
    nameFilters << "*.ddf *.png *.jpg *.bmp *.tif";
    dialog.setNameFilters(nameFilters);//设置文件类型过滤器
    QStringList picturePathList;
    if (dialog.exec()) {
        QStringList tempfilePathList = dialog.selectedFiles();
        m_centralWidget->slotLoadDragOrPasteFile(tempfilePathList);
    }
}

void MainWindow::slotDDFFileOpened(QString filename)
{
    Q_UNUSED(filename)
//    Dtk::Widget::DDialog dialog(this);
//    dialog.setTextFormat(Qt::RichText);
//    dialog.addButton(tr("OK"));
//    dialog.setIcon(QIcon(":/icons/deepin/builtin/Bullet_window_warning.svg"));
//    dialog.setMessage(filename + tr(" Opened"));
//    dialog.exec();
}

void MainWindow::slotTopToolBarSaveToDDF()
{
    // ctrl+s 直接保存ddf文件
    qDebug() << "ctrl+s";
    m_centralWidget->slotSaveToDDF();
}

void MainWindow::slotLastTabBarRequestClose()
{
    qDebug() << "slotLastTabBarRequestClose: not show quit dialog";
    // 退出程序
    qApp->quit();
    //    m_dialog.exec();
}

void MainWindow::slotNewDrawScence()
{
    m_centralWidget->slotNew();
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

void MainWindow::slotLoadDragOrPasteFile(QStringList files)
{
    showDragOrOpenFile(files, false);
}

void MainWindow::slotOnEscButtonClick()
{
    m_topToolbar->slotHideColorPanel();
    m_centralWidget->onEscButtonClick();
}

void MainWindow::showDrawDialog()
{
    if (CManageViewSigleton::GetInstance()->getCurView()->getModify()) {
        showSaveQuestionDialog();
    } else {
        qApp->quit();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_topToolbar->updateColorPanelVisible(event->pos());
    DMainWindow::mousePressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString fileName = Global::configPath() + "/config.conf";
    QSettings settings(fileName, QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());


    emit dApp->popupConfirmDialog();
    event->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setShiftKeyStatus(true);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAltKeyStatus(true);
    } else if (event->key() == Qt::Key_Control) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCtlKeyStatus(true);
    } else {
    }

    //    if (qApp->focusWidget() != nullptr) {
    //        qApp->focusWidget()->clearFocus();
    //        //qApp->focusWidget()->hide();
    //    }
    //    if (qApp->focusWidget() != nullptr) {
    //        qApp->focusWidget()->hide();
    //    }

    DMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Shift) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setShiftKeyStatus(false);
    }
    //先按下SHIFT再按下ALT 会出现 Key_Meta按键值
    else if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Meta) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setAltKeyStatus(false);
    } else if (event->key() == Qt::Key_Control) {
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setCtlKeyStatus(false);
        m_contrlKey = true;
    }  else {
        ;
    }
    DMainWindow::keyReleaseEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    //如果按住CTRL
    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCtlKeyStatus()) {
        if (event->delta() > 0) {
            m_centralWidget->getGraphicsView()->zoomOut();
        } else {
            m_centralWidget->getGraphicsView()->zoomIn();
        }
    }
    DMainWindow::wheelEvent(event);
}

void MainWindow::openImage(QString path, bool isStartByDDF)
{
    // 此函数是命令行调用进行处理的相关代码
    if (!path.isEmpty()) {
        // 新建一个标签页
        showDragOrOpenFile(QStringList(path), isStartByDDF);

//        if (cut == CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCurrentDrawToolMode()) {
//            m_centralWidget->getGraphicsView()->slotQuitCutMode();
//        }
//        if (QFileInfo(path).suffix().toLower()  == "ddf") {
//            if (isStartByDDF) {
//                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::StartByDDF);
//            } else {
//                CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::LoadDDF);
//            }
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setDdfSavePath(path);
//        } else {
//            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setSaveDDFTriggerAction(ESaveDDFTriggerAction::ImportPictrue);
//            tmpPictruePath = path;
//        }
//        slotIsNeedSave();
    }
}

void MainWindow::initScene()
{
    QSize size = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getCutDefaultSize();
    QRectF rect(0, 0, 0, 0);
    rect.setSize(size);
    m_centralWidget->getDrawScene()->setSceneRect(rect);
    emit m_centralWidget->getDrawScene()->signalUpdateCutSize();
}

void MainWindow::readSettings()
{
    QString fileName = Global::configPath() + "/config.conf";
    QSettings settings(fileName, QSettings::IniFormat);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::slotOnThemeChanged(DGuiApplicationHelper::ColorType type)
{
    CManageViewSigleton::GetInstance()->setThemeType(type);
    ///改变场景的主题
    m_centralWidget->switchTheme(type);
    //改变左边工具栏按钮主题
    m_centralWidget->getLeftToolBar()->changeButtonTheme();
    //改变顶部属性栏按钮主题
    m_topToolbar->changeTopButtonsTheme();
}

MainWindow::~MainWindow()
{

}
